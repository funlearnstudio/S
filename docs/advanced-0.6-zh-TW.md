# SE 0.6 進階功能教學

SE 0.6 的目標是讓高階程式設計仍維持 **Simple at every level**。這一版把函式值、高階 collections、泛型函式、模式匹配、Task、Option / Result、檔案型資料庫與 HTTPS 加入既有語言。

> 本文件描述實際實作。SE 0.6 的泛型目前是「泛型函式」，還不是完整的 generic type / trait 系統；HTTPS 使用系統 `curl` 作 TLS transport；資料庫是內建的輕量檔案型 Text key/value database，不宣稱取代 SQLite/PostgreSQL。

## 1. filter / map / reduce

```se
use collections

make even n
    give n % 2 == 0

make double n
    give n * 2

make add a b
    give a + b

nums = [1, 2, 3, 4, 5]
evens = collections.filter nums even
doubled = collections.map nums double
total = collections.reduce nums 0 add
```

- `filter list predicate`：保留 predicate 回傳 `true` 的值。
- `map list function`：把每個值轉換後建立新 List。
- `reduce list initial function`：以 accumulator 逐項合併。

這些函式不修改原 List。

## 2. sort by key / callback sort

Map 或 object 可以依欄位排序：

```se
users = [["name": "B", "score": 20], ["name": "A", "score": 10]]
ordered = collections.sort_by users "score"
desc = collections.sort_by_desc users "score"
```

也可自行提供比較函式：

```se
make before a b
    give a < b

ordered = collections.sort_with [3, 1, 2] before
```

排序結果是新 List。

## 3. slice / take / drop

```se
nums = [10, 20, 30, 40, 50]
part = collections.slice nums 1 4
first = collections.take nums 2
rest = collections.drop nums 2
```

`slice` 採 `[start, end)`，end 不包含在結果裡，也支援負 index。

## 4. Function values、closure 與 lambda 的 SE 替代

SE 不要求加入大量 lambda punctuation。一般做法是先用 `make` 建立清楚的小函式，再把函式當成 value 傳遞。

```se
make double n
    give n * 2

values = collections.map [1, 2, 3] double
```

需要 capture / partial application 時使用 `function.bind`：

```se
use function

make add a b
    give a + b

add10 = function.bind add 10
answer = function.call add10 5
say answer
```

`add10` 是會記住 `10` 的新 callable，因此提供真正的 captured function / closure 行為，而不需要寫 `lambda x: ...`。

`function.pipe` 可把值依序傳入多個單參數函式：

```se
result = function.pipe value step1 step2 step3
```

## 5. 泛型函式

SE 0.6 支援明確的 generic parameter：

```se
make identity[T] value:T -> T
    give value

number = identity 42
text = identity "SE"
```

多個泛型型別：

```se
make choose[A, B] first:A second:B -> A
    give first
```

Checker 會在 call site 綁定泛型參數並檢查同一個泛型名稱的使用是否相容。

目前邊界：這一版重點是 generic functions。像 `type Box[T]`、trait constraints、`List[T]` annotation 等完整 generic type system 仍是之後的獨立工作。

## 6. Pattern matching

```se
value = 2

match value
    case 1
        say "one"
    case 2
        say "two"
    else
        say "other"
```

目前 pattern matching 是 value pattern：case expression 與 subject 相等時執行第一個匹配 branch。Checker 也會檢查 case 與 subject 的型別相容性。

複雜的 destructuring patterns / guards 尚未假裝成已完成能力。

## 7. Option

`Option` 用於「可能有值，也可能沒有值」的情況：

```se
use option

name = option.some "Steve"

if option.is_some name
    say option.value name
```

沒有值：

```se
missing = option.none
value = option.or missing "default"
```

API：`some`、`none`、`is_some`、`is_none`、`value`、`or`。

對 `none` 呼叫 `option.value` 會產生可處理錯誤。

## 8. Result

`Result` 明確表示 success / failure：

```se
use result

answer = result.ok 42
problem = result.err "invalid input"

if result.is_ok answer
    say result.value answer

if result.is_err problem
    say result.error problem
```

也可以提供 fallback：

```se
value = result.or problem 0
```

API：`ok`、`err`、`is_ok`、`is_err`、`value`、`error`、`or`。

## 9. Match helpers for Option / Result

除了語言層的 `match` statement，`use match` 還提供 function-oriented helpers，方便把 handlers 傳進去。

```se
use match
use option

make got value
    say value

make missing
    say "none"

value = option.some "SE"
match.option value got missing
```

`match.result` 同理，可分別處理 ok value 與 error text。

## 10. async / await 的 SE 版本

```se
use async

make slow_work value
    give value * 2

job = async.run slow_work 21

try
    answer = async.await job
    say answer
else err
    say err.message
```

`async.run` 立即回傳 managed `Task`，`async.ready task` 可檢查完成狀態，`async.await task` 取得結果並傳遞 task failure。

目前 SE interpreter 為了安全會序列化進入 VM 的 callback，所以這一版提供的是安全的 Task / await 模型，不宣稱 SE bytecode 已經具備無限制平行執行能力。

## 11. Database API

```se
use db
use option

try
    store = db.open "data.se-db"
    db.set store "name" "SE"

    name = db.get store "name"
    if option.is_some name
        say option.value name
else err
    say err.message
```

其他操作：

```se
exists = db.has store "name"
removed = db.remove store "name"
keys = db.keys store
try db.save store
```

資料庫是 SE runtime 提供的持久化 Text key/value store。資料寫在本機檔案，適合設定、小型工具、教學與簡單應用。大型 relational database、SQL、transaction isolation、indexes 等不屬於這個 API 的宣稱範圍。

## 12. HTTPS

SE 0.5 的 socket HTTP client 只處理 plain HTTP。SE 0.6 增加真正 TLS HTTPS transport：

```se
use https

try
    body = https.get "https://example.com"
    say body
else err
    say err.message
```

POST：

```se
try
    body = https.post "https://example.com/api" "hello"
else err
    say err.message
```

JSON：

```se
try
    body = https.post_json "https://example.com/api" "{\"name\":\"SE\"}"
else err
    say err.message
```

TLS transport 目前使用系統 `curl`，因此 `curl` 必須在 PATH。這樣能使用成熟 TLS / certificate validation，而不是自己實作危險的加密協定。

## 13. 組合起來

SE 0.6 可以把這些能力串在一起：

```se
use collections
use function
use async
use option
use result

make valid n
    give n > 0

make double n
    give n * 2

nums = [-2, 1, 2, 3]
clean = collections.filter nums valid
values = collections.map clean double
first_two = collections.take values 2

say first_two
```

重點不是讓 SE 增加一大堆符號，而是把 advanced programming 做成可以從原本 `make`、List、module、`try` 自然延伸的能力。
