# SE 完整語言教學（繁體中文）

這份文件以目前 **SE 0.5.0-dev** 的實作為準，從最基本的語法一路介紹到型別、模組、錯誤處理、測試、JSON、Process、HTTP、Web Router、JSON API、JavaScript / TypeScript bridge、native interop 與 CLI。

SE 的核心理念是：

> Simple at every level.

SE 使用 `.se` 作為主要原始碼副檔名，CLI 是 `se`。Block 使用縮排，不需要 `{}`；函式呼叫盡量不用括號；型別通常由 compiler 推斷。

---

## 1. 第一個 SE 程式

建立 `hello.se`：

```se
say "Hello SE"
```

執行：

```bash
se run hello.se
```

只做語法與型別檢查：

```bash
se check hello.se
```

編譯為 native executable：

```bash
se build hello.se
./hello
```

常見開發流程就是：

```bash
se check app.se
se run app.se
se build app.se
```

---

## 2. 註解

SE 使用 `#`：

```se
# 這是一行註解
say "Hello"
```

---

## 3. 變數與型別推斷

直接用 `=` 建立變數：

```se
name = "Steve"
age = 15
score = 98.5
ready = true
```

SE 會推斷：

```text
name  → Text
age   → Int
score → Num
ready → Bool
```

常見 runtime value family 包括：

```text
None
Int
Num
Bool
Text
Bytes
Duration
List
Map
Set
Function
Object
Error
Path
File
Native Handle
```

SE 是「少寫型別」，不是「沒有型別系統」。不相容的操作會被 checker 阻止。

---

## 4. 輸出與輸入

輸出：

```se
say "Hello"
say 123
```

輸入：

```se
name = ask "What is your name?"
say "Hello " + name
```

---

## 5. 數學

```se
a = 10
b = 3

say a + b
say a - b
say a * b
say a / b
say a % b
say a ** 2
```

`**` 是次方。

也可以使用 `math` module：

```se
use math

say math.pi
say math.sqrt 9
say math.pow 2 8
```

---

## 6. 比較與邏輯

```se
age = 15

say age == 15
say age != 18
say age > 10
say age >= 15
say age < 20
say age <= 15
```

邏輯：

```se
ready = true
admin = false

say ready and not admin
say ready or admin
```

---

## 7. if / else

SE 不使用 `{}`，而是使用縮排：

```se
age = 15

if age >= 15
    say "allowed"
else
    say "not allowed"
```

縮排建議固定使用 4 個空白。

---

## 8. repeat

固定重複：

```se
repeat 3
    say "Hello"
```

---

## 9. while

```se
n = 0

while n < 5
    say n
    n = n + 1
```

---

## 10. for 與 range

List：

```se
nums = [10, 20, 30]

for n in nums
    say n
```

Range：

```se
for n in 1..10
    say n
```

Map 可以一次取得 key 與 value：

```se
user = ["name": "Steve", "role": "student"]

for key value in user
    say key
    say value
```

---

## 11. 函式：make / give

建立函式：

```se
make add a b
    give a + b
```

呼叫：

```se
answer = add 5 3
say answer
```

SE 的低標點呼叫不需要：

```text
add(5, 3)
```

而是：

```se
add 5 3
```

多參數會依空白分開：

```se
test.equal value 4
```

代表 `test.equal` 收到 `value` 與 `4` 兩個參數。

---

## 12. List

```se
nums = [1, 2, 3]
nums.add 4
nums.remove 2

say nums.len
say nums[0]
```

SE 會檢查 index 範圍，不把 C/C++ undefined behavior 暴露給一般程式。

List 通常維持單一 element type。

---

## 13. Map

```se
user = ["name": "Steve", "city": "Taipei"]

say user["name"]
user["name"] = "SE user"
```

目前 Map key 主要使用 `Text`。

---

## 14. Set

```se
values = set [1, 2, 2, 3]
values.add 4
values.remove 1

if 3 in values
    say "found"
```

Set 只保留 unique values。

---

## 15. Text

Text 自帶常用 member：

```se
text = "Hello SE"

say text.len
say text.upper
say text.lower
```

更完整的文字工具放在 `text` module：

```se
use text

value = text.trim "  hello  "
say value

say text.contains "hello world" "world"
say text.starts "hello" "he"
say text.ends "hello" "lo"
say text.replace "a-b-c" "-" "/"

parts = text.split "a,b,c" ","
say text.join parts "-"
say text.repeat "SE" 3
```

---

## 16. collections module

```se
use collections

nums = [3, 1, 3, 2]

sorted = collections.sort nums
reversed = collections.reverse nums
unique = collections.unique nums

say collections.first sorted
say collections.last sorted
say collections.contains nums 3
```

Map helper：

```se
user = ["name": "Steve", "city": "Taipei"]
keys = collections.keys user
values = collections.values user
```

`sort` 支援同一可排序型別的 Int、Num 或 Text。

---

## 17. 自訂 type

```se
type Player
    name = ""
    hp = 100
```

建立 object：

```se
player = Player
```

Indented initialization：

```se
player = Player
    name = "Steve"
    hp = 80
```

讀取 field：

```se
say player.name
say player.hp
```

---

## 18. Method

```se
type Player
    name = ""
    hp = 100

    make hit damage
        hp = hp - damage

    make alive
        give hp > 0

player = Player
    name = "Steve"

player.hit 20
say player.hp
say player.alive
```

Method 裡可以直接使用 field，不必寫 `self.hp`。

---

## 19. Modules：use

`player.se`：

```se
type Player
    hp = 100

make make_player
    give Player
```

另一個檔案：

```se
use player

p = Player
say p.hp
```

SE 優先搜尋 `.se`，並暫時保留舊 `.s` 相容性。

Top-level 名稱若以 `_` 開頭，表示 module-private convention：

```se
_secret = 123
```

Circular import 會被拒絕。

---

## 20. 錯誤處理：try / else

```se
try
    text = read "data.txt"
    say text
else err
    say err.kind
    say err.message
```

建立錯誤：

```se
make divide a b
    if b == 0
        fail "Cannot divide by zero"
    give a / b
```

將可恢復錯誤向外傳：

```se
make load
    give try read "data.txt"
```

`try expression` 表示：失敗時把 error 傳給上一層，而不是在這裡處理。

---

## 21. Files

```se
try
    write "hello.txt" "Hello"
    append "hello.txt" " SE"
    content = read "hello.txt"
    say content
else err
    say err
```

File 操作的底層資源由 runtime 管理。

---

## 22. path module

```se
use path

file = path.join "data" "user.txt"

say path.name file
say path.ext file
say path.exists file
```

Path 使用跨平台 filesystem 行為，避免自己硬寫 `/` 或 `\\`。

---

## 23. time 與 Duration

```se
use time

say time.now

wait 500ms
wait 2s
wait 1min
```

SE 直接支援：

```text
ms
s
min
```

Duration literal。

---

## 24. random module

```se
use random

say random.int 1 10
say random.num
```

`random.int` 產生整數範圍值，`random.num` 取得隨機 Num。

---

## 25. os module

```se
use os

say os.platform
say os.cwd
```

可用來取得目前平台與 working directory。

---

## 26. Bytes

```se
data = bytes "hello"
say data.len
```

`Bytes` 與 `Text` 分離，用於 binary-safe data 與 native interoperability。

---

## 27. JSON

```se
use json

raw = "{\"name\":\"SE\",\"count\":2}"
data = json.parse raw

say json.stringify data
say json.pretty data
```

`json.parse` 會把 JSON 轉成 SE values：

```text
JSON object → Map
JSON array  → List
string      → Text
integer     → Int
number      → Num
boolean     → Bool
null        → None
```

反方向使用：

```se
data = ["name": "SE", "version": 5]
encoded = json.stringify data
say encoded
```

---

## 28. 測試：test module

建立例如：

```text
math_test.se
```

內容：

```se
use test

value = 2 + 2

test.equal value 4
test.not_equal value 5
test.ok value == 4
```

故意讓測試失敗：

```se
test.fail "This test should stop here"
```

執行整個目錄下的 `*_test.se`：

```bash
se test .
```

SE 會輸出 PASS / FAIL 與總結。

---

## 29. Process

```se
use process
```

執行 shell command，取得 exit code：

```se
code = process.run "echo Hello"
say code
```

取得 stdout：

```se
output = process.output "echo Hello SE"
say output
```

`process` 會真的啟動外部程式，所以使用時應只執行你信任與理解的 command。

---

## 30. HTTP client

```se
use http
```

GET：

```se
body = http.get "http://example.com/"
say body
```

POST text：

```se
body = http.post "http://localhost:8080/data" "hello"
```

POST JSON text：

```se
payload = "{\"name\":\"SE\"}"
body = http.post_json "http://localhost:8080/api" payload
```

General request：

```se
response = http.request "GET" "http://localhost:8080/" ""
say response.status
say response.body
say response.type
```

目前 built-in HTTP client 支援 **plain `http://`**。目前還沒有 TLS/HTTPS。

---

## 31. Web Router

```se
use web
```

最簡單 route：

```se
make home body
    give "Hello from SE"

web.get "/" home
```

支援：

```se
web.get path handler
web.post path handler
web.put path handler
web.patch path handler
web.delete path handler
```

Route parameter：

```se
make hello body
    name = web.param "name"
    give "Hello " + name

web.get "/hello/:name" hello
```

---

## 32. Request 資訊

在 route handler 裡可以讀：

```se
method = web.method
path_value = web.path
query = web.query
body_value = web.body
agent = web.header "user-agent"
name = web.param "name"
```

可取得：

```text
HTTP method
path
query string
request body
request header
route parameter
```

---

## 33. Response

純文字：

```se
make home body
    give web.text "Hello"
```

JSON：

```se
use web
use json

make api body
    data = ["ok": true, "message": "Hello SE"]
    give web.json json.stringify data
```

自訂 status / body / content type：

```se
make missing body
    give web.response 404 "Not found" "text/plain"
```

`web.response` 的參數順序是：

```text
status body content_type
```

---

## 34. 啟動 HTTP Server

```se
use web

make home body
    give "Hello SE"

web.get "/" home

say "Listening on http://localhost:8080"
try web.listen 8080
```

`web.listen` 目前是 synchronous / blocking server，適合開發、小型服務與教學，不應把它當成已經完成 hardened high-concurrency production server。

---

## 35. 不開 port 測試 Web Router

SE 提供 in-memory dispatch：

```se
use web
use test

make home body
    give "Hello"

web.get "/" home

result = web.handle "GET" "/" ""
status = web.handle_status "GET" "/" ""

test.equal status 200
test.equal result "Hello"
```

Route 數量：

```se
say web.route_count
```

這很適合搭配 `se test`。

---

## 36. JSON API 完整範例

```se
use web
use json

make hello body
    name = web.param "name"
    data = ["message": "Hello " + name, "received": body]
    payload = json.stringify data
    give web.json payload

web.get "/api/hello/:name" hello

say "API running on http://localhost:8080"
try web.listen 8080
```

瀏覽器呼叫：

```js
const response = await fetch('http://localhost:8080/api/hello/browser')
const data = await response.json()
console.log(data)
```

---

## 37. JavaScript bridge

SE 的 `js` module 透過 Node.js 執行 JavaScript。

```se
use js
```

執行檔案並取得 exit code：

```se
code = js.run "script.js"
say code
```

取得 stdout：

```se
output = js.output "script.js"
say output
```

直接 eval：

```se
result = js.eval "console.log(2 + 2)"
say result
```

需要系統已安裝 `node`。

---

## 38. TypeScript bridge

```se
use ts
```

執行 TypeScript：

```se
code = ts.run "app.ts"
```

取得 stdout：

```se
output = ts.output "app.ts"
say output
```

用 `tsc` 編譯：

```se
code = ts.compile "app.ts"
say code
```

`ts.run` / `ts.output` 需要 `ts-node`，`ts.compile` 需要 `tsc`。

---

## 39. 建立新專案

一般 app：

```bash
se new app myapp
cd myapp
se check-all .
se test .
se run src/main.se
```

Web app：

```bash
se new web mysite
cd mysite
se check-all backend
se test backend
se run backend/main.se
```

Web template 會建立：

```text
mysite/
├── backend/
│   ├── main.se
│   └── tests/
│       └── backend_test.se
└── frontend/
    ├── index.html
    ├── style.css
    ├── app.js
    ├── app.ts
    ├── se-api.js
    └── se-api.ts
```

`se-api.js` / `se-api.ts` 是 browser → SE backend 的 fetch bridge。

---

## 40. 專案級檢查

單檔：

```bash
se check app.se
```

整個目錄：

```bash
se check-all .
```

測試：

```bash
se test .
```

環境檢查：

```bash
se doctor
```

Help：

```bash
se help
```

---

## 41. REPL

```bash
se
```

例如：

```text
> x = 5
> say x * 2
10
```

Block 在 REPL 裡通常用空白行完成。

---

## 42. Native C ABI module

SE 可以透過 `.snative` 使用 C ABI library。

例如：

```text
library mathx
add math_add Int Int -> Int
```

SE：

```se
use mathx
say add 20 22
```

主要 bridge value 包括：

```text
Int
Num
Bool
Text
Bytes
None
managed opaque handles
```

Native handle 可以設定 cleanup function，由 runtime 自動管理生命週期。

---

## 43. Binding generator

`.sbind`：

```text
module mathx
library mathx
header mathx.hpp

add math_add Int Int -> Int
```

產生 binding：

```bash
se bind mathx.sbind generated
```

目前會產生類似：

```text
mathx.snative
mathx_bindings.h
mathx_bindings.cpp
```

---

## 44. `.help`

SE 的不少 value/module 可以使用 `.help` 查看可用 member，例如：

```se
math.help
```

當 checker 提示某個 value 沒有指定 member 時，也可能建議使用：

```se
value.help
```

---

## 45. 一個完整小程式

```se
use json
use text
use collections
use test

make greet name
    clean = text.trim name
    give "Hello " + clean

names = [" Steve ", "SE", "Steve"]
unique = collections.unique names

message = greet names[0]
say message

data = ["message": message, "count": unique.len]
say json.pretty data

test.equal message "Hello Steve"
test.ok unique.len > 0
```

執行：

```bash
se check program.se
se run program.se
```

---

## 46. 一個完整 Web API 小專案

`server.se`：

```se
use web
use json

make health body
    data = ["ok": true, "service": "SE"]
    give web.json json.stringify data

make hello body
    name = web.param "name"
    data = ["hello": name]
    give web.json json.stringify data

web.get "/health" health
web.get "/hello/:name" hello

say "http://localhost:8080"
try web.listen 8080
```

執行：

```bash
se run server.se
```

---

## 47. SE 的寫法習慣

推薦保持：

```text
4-space indentation
少括號
短而清楚的 function
用 module 分檔
先 se check，再 se run
正式驗證時跑 se test
需要 native executable 時再 se build
```

SE 刻意避免把一般使用者推進 raw pointer、manual memory management、任意 C++ ABI 與大量 punctuation。

---

## 48. 目前的重要邊界

SE 0.5.0-dev 已有完整的基礎語言、native backend、standard/platform modules、測試、JSON 與 Web 開發鏈，但仍有清楚的工程邊界：

- built-in HTTP client 目前只有 `http://`，尚未提供 TLS/HTTPS。
- built-in Web server 目前是同步 blocking server。
- JavaScript bridge 依賴 Node.js。
- TypeScript bridge 依賴 `ts-node` / `tsc`。
- native interoperability 的推薦邊界是 C ABI，不直接暴露任意 C++ ABI。
- `.s` 目前可能仍作為 migration compatibility；新程式應使用 `.se`。

---

## 49. 建議學習順序

如果第一次完整學 SE，可以照這條路徑：

```text
say / variables / ask
→ math / comparison
→ if / loops
→ make / give
→ List / Map / Set
→ Text / collections
→ type / methods
→ modules
→ try / fail
→ file / path / time / random / os
→ JSON / test
→ process
→ HTTP client
→ Web router / request / response / JSON API
→ JS / TS bridge
→ native C ABI / bindgen
```

學到最後仍然使用同一套核心概念：縮排、值、函式、module 與低標點呼叫。這就是 SE 的核心設計方向。