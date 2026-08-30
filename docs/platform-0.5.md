# SE 0.5 Application and Web Platform

[繁體中文版](platform-0.5-zh-TW.md)

This versioned document records the 0.5 stage, when SE added broader application/Web runtime modules while keeping the core language syntax small.

## JSON

```se
use json

value = json.parse "{\"name\":\"SE\"}"
say json.stringify value
say json.pretty value
```

JSON objects map to SE Maps, arrays to Lists, booleans/numbers/text to their SE values, and null to None. Heterogeneous parsed data may have broad static type information.

## Text and collections

```se
use text
use collections

clean = text.trim "  hello  "
parts = text.split "a,b,c" ","
sorted = collections.sort [3, 1, 2]
```

0.5 established common text/collection utility modules; 0.6 later deepened higher-order collection APIs.

## Test assertions

```se
use test

test.ok true
test.equal 2 + 2 4
```

Assertion failures integrate with `se test`.

## Process bridge

```se
use process

code = process.run "echo SE"
output = process.output "echo SE"
```

Process helpers use operating-system command execution. Applications should not build shell commands from untrusted text.

## HTTP client

```se
use http
body = try http.get "http://example.com/"
```

The 0.5 built-in transport is plain HTTP. HTTPS was introduced separately in the 0.6 line.

## Web server and router

```se
use web

make hello body
    name = web.param "name"
    give "Hello " + name

web.get "/hello/:name" hello
try web.listen 8080
```

This stage includes common HTTP methods, route parameters, request accessors, response helpers and in-memory route testing.

The server model is synchronous/blocking and should be described as a learning/development/small-service server rather than a hardened high-concurrency production server.

## JavaScript and TypeScript bridges

```se
use js
use ts

output = js.output "script.js"
code = ts.compile "app.ts"
```

These bridges rely on the corresponding external Node/TypeScript tooling.

## Layered interoperability

SE keeps several boundaries separate:

```text
C ABI / .snative / se bind   native libraries
process                      CLI tools
js / ts                      Node/TypeScript tooling
HTTP / JSON                  language-neutral services
Web/browser output           frontend integration
```

This prevents every foreign language or platform from becoming a new compiler syntax system.

For current Web component/browser APIs, use [SE Web Language](web-language-0.8.md) and [Browser API](browser-api-0.8.md).
