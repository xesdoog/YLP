<!-- markdownlint-disable -->

# ImGuiCond

## Fields

| Name | Type | Description |
| --- | --- | --- |
| `None` | `integer` | No condition (always set the variable), same as _Always |
| `Always` | `integer` | No condition (always set the variable), same as _None |
| `Once` | `integer` | Set the variable once per runtime session (only the first call will succeed) |
| `FirstUseEver` | `integer` | Set the variable if the object/window has no persistently saved data (no entry in .ini file) |
| `Appearing` | `integer` | Set the variable if the object/window is appearing after being hidden/inactive (or the first time) |
