---@meta

-- Provides an abstraction around byte patching.
-- 
-- It is exclusively returned by [Pointer](lua://Pointer) and can not be explicitly constructed.
---@class BytePatch
BytePatch = {}

-- Applies the patch to the target pointer's memory.
---@return boolean success Success or failure.
function BytePatch:Apply() end

-- Restores the target pointer's original bytes.
---@return boolean success Success or failure.
function BytePatch:Restore() end

-- Whether this patch is currently applied.
---@return boolean 
function BytePatch:IsApplied() end
