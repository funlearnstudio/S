# SE 0.5 Web 平台

本文件是 `platform-0.5.md` 的繁體中文版。

SE 0.5 把 JSON、文字/集合工具、測試、process、HTTP、Web router 與 JavaScript/TypeScript bridge 加入語言。

## JSON

```se
use json

text = json.stringify ["name": "SE", "ok": true]
pretty = json.pretty [1, 2, 3]
value = json.parse "{\"x\":1}"
```

`json.parse` 可產生不同結構，因此 Checker 目前以較寬鬆的 Unknown 表示其結果。

## text

```se
use text

clean = text.trim "  SE  "
parts = text.split "a,b,c" ","
joined = text.join parts "-"
```

另有 contains、starts、ends、replace、repeat。

## collections

0.5 提供 reverse、contains、first、last、unique、sort、keys、values；0.6 再加入 filter/map/reduce、sort_by 與 slice。

## test

```se
use test

test.ok true
test.equal 2 + 2 4
test.not_equal 1 2
```

Assertion failure 會讓 `se test` 判定測試失敗。

## process

```se
use process

code = process.run "echo hello"
out = process.output "echo hello"
```

不要把不可信輸入直接拼進 shell command。

## HTTP client

```se
use http

body = http.get "http://example.com"
```

0.5 的內建 socket client 是 plain `http://`；HTTPS 在 0.6 透過 `use https` 補上。

## Web router

```se
use web

make hello body
    name = web.param "name"
    give "Hello " + name

web.get "/hello/:name" hello
try web.listen 8080
```

支援 GET、POST、PUT、PATCH、DELETE、route param、query/header/body、text/json/response helper，以及 in-memory `web.handle` 測試。

目前 server 是同步、blocking、單連線處理模型，適合教學、開發與小型服務，不宣稱是高併發 production server。

## JS / TS bridge

```se
use js
js.run "tool.js"

use ts
ts.compile "app.ts"
```

需要 Node.js；TypeScript 功能另外需要 `ts-node` / `tsc`。

## Web scaffold

```bash
se new web mysite
```

會建立 SE backend 與 HTML/CSS/JS/TS frontend，以及 `se-api.js` / `se-api.ts` browser fetch bridge。
