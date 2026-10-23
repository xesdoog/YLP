---@meta

-- Provides functions to output text to console and log file.
---@class log
log = {}

-- Logs an information message.
---@param ... any Any number of arguments of any type.
function log.info(...) end

-- Logs a warning message.
---@param ... any Any number of arguments of any type.
function log.warning(...) end

-- Logs a debug message.
---@param ... any Any number of arguments of any type.
function log.debug(...) end

-- Logs an error message.
---@param ... any Any number of arguments of any type.
function log.error(...) end

-- Logs a formatted information message. Arguments are the same as `string.format`
---@param msg string Message.
---@param ... any Optional format arguments.
function log.finfo(msg, ...) end

-- Logs a formatted warning message. Arguments are the same as `string.format`
---@param msg string Message.
---@param ... any Optional format arguments.
function log.fwarning(msg, ...) end

-- Logs a formatted debug message. Arguments are the same as `string.format`
---@param msg string Message.
---@param ... any Optional format arguments.
function log.fdebug(msg, ...) end

-- Logs a formatted error message. Arguments are the same as `string.format`
---@param msg string Message.
---@param ... any Optional format arguments.
function log.ferror(msg, ...) end
