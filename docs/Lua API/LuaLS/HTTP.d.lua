---@meta

-- Provides basic HTTP GET requests.
---@class HTTP
HTTP = {}

-- Performs a web request. NOTE: this is synchronous so it's recommended to execute it in its own [Task](lua://Task)
---@param host string URL host. Example: `github.com`
---@param path string URL path. Example: `/octocat/CoolRepo`
---@return HttpResponse response
function HTTP.Get(host, path) end

-- Performs a web request. NOTE: this is synchronous so it's recommended to execute it in its own [Task](lua://Task)
---@param host string URL host. Example: `github.com`
---@param path string URL path. Example: `/octocat/CoolRepo`
---@param headers { [integer]: string } HTTP headers. Must be a contiguous string array else the function will throw. Example: `{ "Content-Type: application/json", "Connection: keep-alive" }`
---@return HttpResponse response
function HTTP.Get(host, path, headers) end

-- Performs a web request. NOTE: this is synchronous so it's recommended to execute it in its own [Task](lua://Task)
---@param host string URL host. Example: `github.com`
---@param path string URL path. Example: `/octocat/CoolRepo`
---@param headers { [integer]: string } HTTP headers. Must be a contiguous string array else the function will throw. Example: `{ "Content-Type: application/json", "Connection: keep-alive" }`
---@param outFile Path File destination to write to. If provided, the response bytes will be written to the file instead.
---@return HttpResponse response
function HTTP.Get(host, path, headers, outFile) end
