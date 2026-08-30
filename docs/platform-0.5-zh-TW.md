# SE 0.5 Application / Web Platform

[English version](platform-0.5.md)

這份版本化文件記錄 0.5 stage：SE 加入較完整的 application/Web runtime module，同時保持 core language syntax 精簡。

## JSON

```se
use json

value = json.parse "{\"name\":\"SE\"}"
say json.stringify value
say json.pretty value
```

JSON object 對應 Map、array 對應 List、boolean/number/text 對應 SE value、null 對應 None。Heterogeneous parse result 在 static type 上可能較寬鬆。

## Text / Collections

```se
use text
use collections

clean = text.trim "  hello  "
parts = text.split "a,b,c" ","
sorted = collections.sort [3, 1, 2]
```

0.5 建立常用 text/collection utility module；0.6 再加入更完整的 higher-order collection API。

## Test Assertions

```se
use test

test.ok true
test.equal 2 + 2 4
```

Assertion failure 會和 `se test` 整合。

## Process Bridge

```se
use process

code = process.run "echo SE"
output = process.output "echo SE"
```

Process helper 使用 OS command execution；應避免把不可信文字直接拼成 shell command。

## HTTP Client

```se
use http
body = try http.get "http://example.com/"
```

0.5 built-in transport 是 plain HTTP；HTTPS 在 0.6 line 另外加入。

## Web Server / Router

```se
use web

make hello body
    name = web.param "name"
    give "Hello " + name

web.get "/hello/:name" hello
try web.listen 8080
```

這個 stage 包含常用 HTTP method、route parameter、request accessor、response helper 與 in-memory route testing。

Server model 是 synchronous/blocking，適合教學、開發與小型服務，不應描述成 hardened high-concurrency production server。

## JavaScript / TypeScript Bridge

```se
use js
use ts

output = js.output "script.js"
code = ts.compile "app.ts"
```

這些 bridge 依賴對應的 Node / TypeScript external tooling。

## Layered Interoperability

SE 將不同邊界分層：

```text
C ABI / .snative / se bind   native library
process                      CLI tool
js / ts                      Node/TypeScript tooling
HTTP / JSON                  language-neutral service
Web/browser output           frontend integration
```

這樣不需要把每個 foreign language / platform 都變成新的 compiler syntax system。

目前 Web Component / Browser API 請看 [SE Web Language](web-language-0.8-zh-TW.md) 與 [Browser API](browser-api-0.8-zh-TW.md)。
