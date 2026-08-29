# SE 0.5 platform

SE 0.5 expands SE into application and web development while keeping beginner syntax small.

## JSON

```se
use json

value = json.parse "{\"name\":\"SE\"}"
say json.stringify value
say json.pretty value
```

`json.parse` converts JSON to SE values. Objects become Maps, arrays become Lists, and JSON null becomes None. `json.stringify` produces compact JSON and `json.pretty` uses readable indentation.

## Text

```se
use text

clean = text.trim "  hello  "
parts = text.split "a,b,c" ","
joined = text.join parts "-"
```

Functions: `trim`, `contains`, `starts`, `ends`, `replace`, `split`, `join`, `repeat`.

## Collections

```se
use collections

nums = [3, 1, 3, 2]
sorted = collections.sort nums
unique = collections.unique nums
```

Functions: `reverse`, `contains`, `first`, `last`, `unique`, `sort`, `keys`, `values`.

## Test assertions

```se
use test

test.ok true
test.equal 2 + 2 4
test.not_equal "SE" "Python"
```

Assertion failures use the `AssertionError` runtime error kind and are understood by `se test`.

## Processes

```se
use process

code = process.run "echo SE"
output = process.output "echo SE"
```

Process commands use the operating system shell. Programs should not construct shell commands from untrusted input.

## HTTP client

```se
use http

body = try http.get "http://example.com/"
say body
```

Available operations: `get`, `post`, `post_json`, and `request`.

The built-in transport currently implements plain HTTP/1.1 over TCP and accepts `http://` URLs. TLS/HTTPS is intentionally not claimed as implemented yet.

## Web server and router

```se
use web
use json

make hello body
    name = web.param "name"
    data = ["message": "Hello " + name]
    payload = json.stringify data
    give web.json payload

web.get "/api/hello/:name" hello
say "Listening on http://localhost:8080"
try web.listen 8080
```

Routes support GET, POST, PUT, PATCH, and DELETE. `:name` path segments are route parameters.

The current request can be inspected with:

- `web.method`
- `web.path`
- `web.query`
- `web.body`
- `web.header name`
- `web.param name`

Responses can be created with `web.text`, `web.json`, or `web.response status body contentType`. Returning a Map or List from a route automatically creates a JSON response.

For tests, routes can be executed without opening a socket:

```se
body = web.handle "GET" "/api/hello/SE" ""
status = web.handle_status "GET" "/api/hello/SE" ""
```

The server is synchronous and blocking in SE 0.5. It is suitable as a development server and for small services, not yet presented as a hardened high-concurrency production server.

## JavaScript and TypeScript bridges

SE can launch local JS/TS tooling:

```se
use js
use ts

output = js.output "script.js"
code = ts.compile "app.ts"
```

`js` uses `node`. `ts` uses `ts-node` and `tsc`, so those tools must exist on PATH.

`se new web NAME` also generates `frontend/se-api.js` and `frontend/se-api.ts`. These use browser `fetch` to call the SE backend and form the browser-side SE ↔ JavaScript/TypeScript API bridge.

## Web project

```sh
se new web my-site
cd my-site
se check-all backend
se test backend
se run backend/main.se
```

The generated project includes an SE router/API backend, backend route tests, HTML/CSS, JavaScript, TypeScript, and JS/TS API bridge files.

## Interoperability layers

SE now has several complementary boundaries:

- C ABI / `.snative` / `se bind` for native libraries.
- `process` for command-line tools.
- `js` and `ts` for Node/TypeScript tooling.
- HTTP/JSON for language-neutral service boundaries.
- generated browser JS/TS clients for frontend projects.

These are intentionally layered instead of making every foreign language a special compiler feature.
