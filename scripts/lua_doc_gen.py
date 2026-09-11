import re, os, shutil
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class Parameter:
	name: str
	type: str
	description: str = ""


@dataclass
class ReturnValue:
	type: str
	description: str = ""


@dataclass
class ApiMember:
	kind: str
	name: str
	owner: str | None = None
	description: str = ""
	type: str | None = None
	parameters: list[Parameter] = field(default_factory=list)
	returns: list[ReturnValue] = field(default_factory=list)


@dataclass
class ApiLibrary:
	name: str
	kind: str  # "class" | "table"
	description: str = ""
	members: list[ApiMember] = field(default_factory=list)


@dataclass
class ApiModel:
	libraries: list[ApiLibrary] = field(default_factory=list)


NAMESPACES: list[str] = []
UNRESOLVED_MEMBERS: list[ApiMember] = []
GLOBAL_TABLE: ApiLibrary = ApiLibrary(name="Global Table", kind="table")
ANNOTATION_RE = re.compile(r"/\*\s?@ylp\.(?P<header>.+?)[\r\n](?P<body>.*?)[\r\n]\s*@\*/", re.DOTALL,)
HEADER_RE = re.compile(r"^(?P<kind>\w+)\s+(?P<name>\S+).*?$")
PARAM_RE = re.compile(r"^(?P<name>[^<\s]+)<(?P<type>[^>]+)>(?:\s+(?P<description>.*))?$")


def clear_dir(dir: Path):
	for p in dir.iterdir():
		shutil.rmtree(str(p), ignore_errors=True) # don't care, this is just to keep the docs clean on Github


def clean_line(line: str) -> str:
	line = line.strip()
	if line.startswith("*"):
		line = line[1:].lstrip()

	return line


def split_member_name(name: str) -> tuple[str | None, str]:
	if "." not in name:
		return None, name

	owner, member = name.rsplit(".", 1)
	if not owner or not member:
		raise ValueError(f"Invalid member name: {name!r}")

	return owner, member


def parse_param(value: str) -> Parameter:
	match = PARAM_RE.match(value.strip())
	if not match:
		raise ValueError(f"Invalid parameter declaration: {value!r}")

	return Parameter(
		name=match.group("name"),
		type=match.group("type"),
		description=(match.group("description") or "").strip(),
	)


def parse_return(value: str) -> ReturnValue:
	"""
	Return syntax is currently:

		return Pointer
		return integer address The pointer's memory address. Notice the added "address" between the type and the description, that's because without it LuaLS will use "The" as the name of the return
	"""

	parts = value.strip().split(maxsplit=1)
	if not parts:
		raise ValueError("Return declaration has no type")

	return ReturnValue(
		type=parts[0],
		description=parts[1].strip() if len(parts) > 1 else "",
	)


def parse_annotation(header: str, body: str) -> ApiMember | ApiLibrary:
	header_match = HEADER_RE.match(header.strip())
	if not header_match:
		raise ValueError(f"Invalid annotation header: {header!r}")

	kind = header_match.group("kind")
	name = header_match.group("name")
	owner = None

	if kind not in {"class", "table"}:
		owner, name = split_member_name(name)
		result = ApiMember(kind, name, owner)
	else:
		result = ApiLibrary(name=name, kind=kind)

	lines = [clean_line(line) for line in body.splitlines()]
	description_lines = []
	in_description = False
	current_member = result if isinstance(result, ApiMember) else None

	for line in lines:
		if not line:
			if in_description:
				description_lines.append("")
			continue

		if line == "description":
			in_description = True
			continue

		if in_description:
			if re.match(r"^(constructor|operator|method|function|table|class|field|param|return|type)\b", line):
				in_description = False
			else:
				description_lines.append(line)
				continue

		member_match = re.match(r"^(constructor|operator|method|function|field)\s+?(.+?)$", line)
		if member_match:
			member_name, _, maybe_desc = member_match.group(2).partition(" ")
			current_member = ApiMember(
				kind=member_match.group(1),
				name= member_name,
				description=maybe_desc or ""
			)

			result.members.append(current_member)
			continue

		if current_member:
			if line.startswith("param "):
				current_member.parameters.append(
					parse_param(line[6:])
				)
				continue

			if line.startswith("return "):
				current_member.returns.append(
					parse_return(line[7:])
				)
				continue

	result.description = "\n".join(description_lines).strip()
	return result


def resolve_members(model: ApiModel) -> None:
	libraries = { lib.name: lib for lib in model.libraries }

	for member in UNRESOLVED_MEMBERS:
		if member.owner is None:
			member.owner = "Global Table"

		library = libraries.get(member.owner)
		if library is not None:
			library.members.append(member)
			UNRESOLVED_MEMBERS.remove(member)



def parse_source(source: str) -> ApiModel:
	model = ApiModel()
	model.libraries.append(GLOBAL_TABLE)

	for match in ANNOTATION_RE.finditer(source):
		header = match.group("header")
		body = match.group("body")

		item = parse_annotation(header, body)
		if isinstance(item, ApiLibrary):
			model.libraries.append(item)

		elif isinstance(item, ApiMember):
			UNRESOLVED_MEMBERS.append(item)

	resolve_members(model)
	return model


def parse_lua(lib: ApiLibrary, write_path: Path):
	docstring = "---@meta\n\n"
	methods: list[ApiMember] = []

	if lib.description and lib.name != "Global Table":
		docstring += f"-- {"\n-- ".join(line for line in lib.description.split("\n"))}\n"
		docstring += f"---@class {lib.name}\n"

	for member in lib.members:
		if member.kind in ("function", "method"):
			methods.append(member)
		elif member.kind == "operator":
			docstring += f"---@operator {member.name}({"| ".join(p.type for p in member.parameters)}): {member.returns[0].type}\n" # are there even Lua operators that have multiple returns? eh, I can't be arsed
		elif member.kind == "field":
			docstring += f"---@field {member.name}: {member.type} {member.description}"
		elif member.kind == "constructor":
			if member.name == "__call":
				docstring += f"---@overload fun({", ".join(f"{p.name}: {p.type}" for p in member.parameters)}): {lib.name}\n"
			else:
				methods.append(member)

	if lib.name != "Global Table":
		docstring += f"{lib.name} = {{}}\n\n"

	index_char = ":" if lib.kind == "class" else "."
	for method in methods:
		if method.description:
			docstring += f"-- {"\n--\n-- ".join(l for l in method.description.split("~~"))}\n"

		for p in method.parameters:
			docstring += f"---@param {p.name} {p.type} {p.description}\n"

		for r in method.returns:
			docstring += f"---@return {r.type} {r.description}\n"

		prefix = lib.name + index_char if lib.name != "Global Table" else ""
		docstring += f"function {prefix}{method.name}({", ".join(p.name for p in method.parameters)}) end\n\n"

	libpath = write_path / (lib.name + ".d.lua")
	with libpath.open(mode="w", encoding="utf-8", newline="\n") as f:
		f.write(docstring.strip() + "\n")


def gen_luals_defs(model: ApiModel, docs_path: Path):
	libs = model.libraries
	if not libs:
		return

	for lib in libs:
		parse_lua(lib, docs_path)


def md_escape(value: str) -> str:
    return value.replace("|", "\\|").replace("\n", " ")


def md_description(description: str) -> str:
    if not description:
        return ""

    paragraphs = [paragraph.strip() for paragraph in description.split("~~") if paragraph.strip()]
    return "\n\n".join(paragraphs)


def md_type(type_name: str) -> str:
    return f"`{type_name}`"


def md_params(parameters: list[Parameter]) -> str:
    if not parameters:
        return ""

    output = [
        "### Parameters",
        "",
        "| Name | Type | Description |",
        "| --- | --- | --- |",
    ]

    for param in parameters:
        description = md_escape(param.description)
        output.append(f"| `{param.name}` | {md_type(param.type)} | {description} |")

    return "\n".join(output)


def md_returns(returns: list[ReturnValue]) -> str:
    if not returns:
        return ""

    output = [
        "### Returns",
        "",
        "| Type | Description |",
        "| --- | --- |",
    ]

    for ret in returns:
        description = md_escape(ret.description)
        output.append(f"| {md_type(ret.type)} | {description} |")

    return "\n".join(output)


def md_method(method: ApiMember, lib: ApiLibrary) -> str:
    index_char = ":" if lib.kind == "class" else "."
    params = ", ".join(f"{param.name}" for param in method.parameters)
    prefix = lib.name + index_char if lib.name != "Global Table" else ""
    signature = f"{prefix}({params})"
    output = [
        f"## `{method.name}`",
        "",
        "```lua",
        f"function {signature} end",
        "```",
    ]

    description = md_description(method.description)
    if description:
        output.extend(["", description])

    params_doc = md_params(method.parameters)
    if params_doc:
        output.extend(["", params_doc])

    returns_doc = md_returns(method.returns)
    if returns_doc:
        output.extend(["", returns_doc])

    return "\n".join(output)


def md_operator(operator: ApiMember) -> str:
    params = " | ".join(param.type for param in operator.parameters)

    return_type = (operator.returns[0].type if operator.returns else "any")
    output = [
        f"### `{operator.name}`",
        "",
        "```lua",
        f"---@operator __{operator.name}({params}): {return_type}",
        "```",
    ]

    description = md_description(operator.description)
    if description:
        output.extend(["", description])

    return "\n".join(output)


def md_field(field: ApiMember) -> str:
    output = [
        f"| `{field.name}` | {md_type(field.type)} | "
        f"{md_escape(field.description)} |"
    ]

    return "".join(output)


def parse_markdown(lib: ApiLibrary, write_path: Path):
    docstring = f"# {lib.name}\n\n"
    description = md_description(lib.description)
    if description:
        docstring += description + "\n\n"

    methods: list[ApiMember] = []
    operators: list[ApiMember] = []
    fields: list[ApiMember] = []
    constructors: list[ApiMember] = []

    for member in lib.members:
        if member.kind in ("function", "method"):
            methods.append(member)
        elif member.kind == "operator":
            operators.append(member)
        elif member.kind == "field":
            fields.append(member)
        elif member.kind == "constructor":
            constructors.append(member)

    if constructors and lib.name != "Global Table":
        docstring += "## Constructors\n\n"
        for constructor in constructors:
            if constructor.name == "__call":
                params = ", ".join(param.name for param in constructor.parameters)
                docstring += ("```lua\n" f"{lib.name}({params})\n" "```\n")
            else:
                docstring += md_method(constructor, lib) + "\n"

            description = md_description(constructor.description)
            if description:
                docstring += f"\n{description}\n"

            params_doc = md_params(constructor.parameters)
            if params_doc:
                docstring += f"\n{params_doc}\n"

    if operators:
        docstring += "## Operators\n\n"
        for operator in operators:
            docstring += md_operator(operator)
            docstring += "\n\n"

    if fields:
        docstring += "## Fields\n\n"
        docstring += (
            "| Name | Type | Description |\n"
            "| --- | --- | --- |\n"
        )

        for field in fields:
            docstring += md_field(field) + "\n"

        docstring += "\n"

    if methods:
        docstring += "## Methods\n\n"
        for method in methods:
            docstring += md_method(method, lib)
            docstring += "\n\n"

    libpath = write_path / f"{lib.name}.md"
    with libpath.open(mode="w", encoding="utf-8", newline="\n") as f:
        f.write(docstring.strip() + "\n")


def gen_markdown_docs(model: ApiModel, docs_path: Path):
    if not model.libraries:
        return

    for lib in model.libraries:
        parse_markdown(lib, docs_path)


if __name__ == "__main__":
	root = Path(__file__).parent.parent
	headers = root / "src"/ "core"/ "lua_scripting"/ "libs"
	if not headers.exists() or not headers.is_dir():
		print(f"Invaid headers path: '{headers}'")
		exit(1)

	api_root = root / "docs" / "Lua API"
	luals_root = api_root / "LuaLS"
	md_root = api_root / "Docs"
	if not luals_root.exists() or not luals_root.is_dir():
		try:
			os.mkdir(luals_root)
		except Exception as e:
			print(f"Failed to create language definitions folder: {e}")
			exit(1)

	if not md_root.exists() or not md_root.is_dir():
		try:
			os.mkdir(md_root)
		except Exception as e:
			print(f"Failed to create markdown docs folder: {e}")
			exit(1)

	clear_dir(luals_root)
	clear_dir(md_root)

	for p in headers.iterdir():
		if p.suffix != ".hpp":
			continue

		api = parse_source(p.read_text())
		gen_luals_defs(api, luals_root)
		gen_markdown_docs(api, md_root)
