<!-- markdownlint-disable -->

# HTTP

Provides basic HTTP GET requests.

## Methods

## `Get`

```lua
function HTTP.Get(host, path) end
```

Performs a web request. NOTE: this is synchronous so it's recommended to execute it in its own [Task](./Task.md)

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `host` | `string` | URL host. Example: `github.com` |
| `path` | `string` | URL path. Example: `/octocat/CoolRepo` |

### Returns

| Type | Description |
| --- | --- |
| `HttpResponse` | response |

## `Get`

```lua
function HTTP.Get(host, path, headers) end
```

Performs a web request. NOTE: this is synchronous so it's recommended to execute it in its own [Task](./Task.md)

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `host` | `string` | URL host. Example: `github.com` |
| `path` | `string` | URL path. Example: `/octocat/CoolRepo` |
| `headers` | `{ [integer]: string }` | HTTP headers. Must be a contiguous string array else the function will throw. Example: `{ "Content-Type: application/json", "Connection: keep-alive" }` |

### Returns

| Type | Description |
| --- | --- |
| `HttpResponse` | response |

## `Get`

```lua
function HTTP.Get(host, path, headers, outFile) end
```

Performs a web request. NOTE: this is synchronous so it's recommended to execute it in its own [Task](./Task.md)

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `host` | `string` | URL host. Example: `github.com` |
| `path` | `string` | URL path. Example: `/octocat/CoolRepo` |
| `headers` | `{ [integer]: string }` | HTTP headers. Must be a contiguous string array else the function will throw. Example: `{ "Content-Type: application/json", "Connection: keep-alive" }` |
| `outFile` | `Path` | File destination to write to. If provided, the response bytes will be written to the file instead. |

### Returns

| Type | Description |
| --- | --- |
| `HttpResponse` | response |
