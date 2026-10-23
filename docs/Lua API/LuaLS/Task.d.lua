---@meta

-- Provides functions to run tasks in YLP's thread.
---@class Task
Task = {}

---@param callback function The callback to execute
function Task.Run(callback) end

-- Execute a callback after a delay
---@param delay integer Delay in milliseconds
---@param callback function The function to execute
function Task.RunDelayed(delay, callback) end

---@param ms integer Sleep time in milliseconds
function Task.Sleep(ms) end

---@param ms integer? Optional yield time in milliseconds
function Task.Yield(ms) end
