---@meta

-- A process abstraction providing basic process interactions.
---@class Process
---@overload fun(processName: string): Process
Process = {}

---@return boolean 
function Process:IsRunning() end

---@param moduleName string Name of the module. Example: `"kernelbase.dll"`
---@return boolean 
function Process:IsModuleLoaded(moduleName) end

---@return integer 
function Process:GetModuleSize() end

---@return integer -- The module's base address
function Process:GetModuleBase() end

-- Updates the process object's internal state. This is useful because the `Process` class does not poll for status on its own.
function Process:Update() end

---@param pattern string IDA-style byte signature. Example: `"8B 88 C0 ?? ?? ?? 89 8F"`
---@param name string? Optional name of the pattern to scan. Purely for logging purposes.
---@param chunkSize integer? Optional memory chunk size. Defaults to 4096
---@return Pointer ptr A pointer at the found address or a null pointer if the scan fails. This is guaranteed to always return a Pointer object.
function Process:FindPattern(pattern, name, chunkSize) end
