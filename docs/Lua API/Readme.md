# Builtin LuaJIT

> [!Warning]
> The scripting feature is **disabled** by default. It is highly recommended to only enable it if you trust the source of the code you intend to execute.

## About

YLP offers power-users the ability to extend its functionality however they like through a LuaJIT environment.

During the lifetime of this project, a few requests have been made to add support for other mods and/or add new functionality outside the project's scope. The thing is, no matter the framework one ends up creating *(especially as someone who's not a programmer)*, it will never satisfy everyone's needs; and so the decision was made, despite the potential risks, to embed a scripting language that's both tiny and powerful.

Since most of YLP's users are technical enough and almost all of them come from the GTA modding scene, it made perfect sense to choose Lua *(also dev bias)*.

As of now, the API is in its baby stage so expect bugs and possibly undefined behavior.

## Basic Code Example

To demonstrate what you can currently do with the Lua API, here's an example GTA V Legacy `ScriptGlobal` implementation:

```Lua
local SG_PTR ---@type Pointer?

------------------------------------
------------------------------------
------------------------------------

---@class ScriptGlobal
---@field private m_ptr Pointer
---@overload fun(address: integer): ScriptGlobal
local ScriptGlobal = setmetatable({}, {
    __call = function(t, address)
        return t:new(address)
    end
}); ScriptGlobal.__index = ScriptGlobal

---@param ptr Pointer
---@return ScriptGlobal
local function fromptr(ptr)
    ---@diagnostic disable-next-line
    return setmetatable({ m_ptr = ptr }, ScriptGlobal)
end

---@param index integer
---@return ScriptGlobal
function ScriptGlobal:new(index)
    assert(SG_PTR and not SG_PTR:IsNull(), "Globals table pointer is null!")
    assert(type(index) == "number", "Address must be an integer.")
    return setmetatable({
        m_ptr = SG_PTR:Add(((index >> 0x12) & 0x3F) * 8):Dereference():Add((index & 0x3FFFF) * 8)
    }, self)
end

---@nodiscard
---@return boolean
function ScriptGlobal:IsValid()
    return self.m_ptr:GetAddress() >= 0x1000
end

---@param offset number
function ScriptGlobal:At(offset)
    return fromptr(self.m_ptr:Add(offset * 8))
end

---@return integer
function ScriptGlobal:GetAddress()
    return self.m_ptr:GetAddress()
end

-- For the sake of testing, we're only going to define a `ReadFloat` method.
--
-- You can add all read/write methods from the [Pointer](lua://Pointer) class.
---@return number
function ScriptGlobal:ReadFloat()
    return self.m_ptr:ReadFloat()
end

----------------------------------------
----------------------------------------
----------------------------------------

local function test()
    local fKickVotesNeededRatio = ScriptGlobal(262145):At(6)
    if (not fKickVotesNeededRatio:IsValid()) then
        log.warning("Please reload the script after loading into a game mode.")
        return
    end

    printf("fKickVotesNeededRatio: %.2ff", fKickVotesNeededRatio:ReadFloat())
end

YLP.RegisterProcessWatcher("GTA5.exe", function(process --[[This parameter is passed by YLP when the process is found]])
    local ptr = process:FindPattern("48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF 48 89 1D", "Script Globals")
    if (ptr:IsNull()) then
        return
    end

    SG_PTR = ptr:Add(0x3):Rip()
    test()
end)

```

>[!Note]
>The example code above is GTA V-specific but the underlying API is not. [Process](./Docs/Process.md) and [Pointer](./Docs/Pointer.md) operate on arbitrary processes so Lua scripts can be used to extend YLP for other applications and modding frameworks as well.

For advanced users, the `JIT` and `FFI` libs are open. `os`, `io`, and `debug` are not.

## Language Server

To have type hints in your code editor, follow these steps:

1. Install Lua Language Server *([VS Code Extension](https://marketplace.visualstudio.com/items?itemName=sumneko.lua), [GitHub repository](https://github.com/LuaLS/lua-language-server))*.
2. Download the [/LuaLS](./LuaLS/) folder and place it in `%AppData%\YLP\Plugins\shared`.
3. For Visual Studio Code users, create a `.code-workspace` file in your plugin folder *(next to your `main.lua` file)* and paste this in it:

    ```JSON
    {
        "folders": [
            {
                "path": ".",
            },
        ],
        "settings": {
            "Lua.runtime.version": "LuaJIT",
            "Lua.runtime.enableLuaJITExtensions": true,
            "Lua.workspace.checkThirdParty": false,
            "Lua.workspace.library": [
                "../shared"
            ],
            "Lua.runtime.builtin": {
                "debug": "disable",
                "os": "disable",
                "io": "disable",
            },
        }
    }
    ```

## API Docs

Documentation for YLP's bindings are in the [/Docs](./Docs/) folder.

## LuaJIT Version Specification

The embedded version is v2.1 with 3.0 extensions backport. These extensions include:

- **Bit Operators:**

    ```Lua
        local x = a & b
        local y = a | b
        local z = a ~ b
        local w = ~a
        local foo = a << 4
        local bar = a >> 2
        local baz = a ~>> 0
    ```

- **C-style Operators:**

    ```Lua
    if a != b then ... end
    if !value then ... end
    if a && b then ... end
    if a || b then ... end
    ```

- **Ternary:**

    ```Lua
    local x = condition ? first : second
    ```

- **`nil` Coalescing:**

    ```Lua
    local x = value ?? fallback
    local y = maybeTable?.value
    local z = Class:MaybeMethod.?(args)
    ```

- **Compound Assignment:**

    ```Lua
    x += 1
    x &= mask
    x ..= "foo"
    ```

- **Other Additions:**

    ```Lua
    continue
    const x = 123
    local fn = (x, y) => x + y
    local n = 1_000_000
    ```

You can learn more about 3.0 extensions on [LuaJIT#1475](https://github.com/LuaJIT/LuaJIT/issues/1475).
