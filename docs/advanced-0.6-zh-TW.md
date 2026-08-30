# SE 0.6 進階功能

[English version](advanced-0.6.md)

SE 0.6 在維持低標點語法的前提下，加入 higher-order collection、lexical closure、generic function、value-based match、managed Task、Option / Result、本機持久化資料庫與 HTTPS。

> 這是版本化文件。0.6 的 generic 核心是 generic function；完整 generic user type / trait system 屬於後續階段。HTTPS 目前使用系統 `curl`，內建 database 則是輕量 Text key/value store，不等於 relational database。

## Higher-order Collections

```se
use collections

filtered = collections.filter nums predicate
mapped = collections.map nums transform
total = collections.reduce nums 0 add
part = collections.slice nums 1 4
first = collections.take nums 2
rest = collections.drop nums 2
```

排序 helper 包含 `sort_by`、`sort_by_desc` 與 callback-based `sort_with`。

## Closure 與 Function Value

```se
make make_adder base
    make inner value
        give base + value
    give inner

add10 = make_adder 10
say add10 5
```

巢狀 `make` 會捕捉 lexical environment。`function` module 也可以提供 bind / partial application 與 pipeline helper。

## Generic Function

```se
make identity[T] value:T -> T
    give value

number = identity 42
text = identity "SE"
```

Checker 會在 call site 綁定 generic parameter，並要求同一 type parameter 的使用維持一致關係。

## Value-based Match

```se
match value
    case 1
        say "one"
    case 2
        say "two"
    else
        say "other"
```

此階段是 equality/value matching。Destructuring、guard、完整 exhaustiveness 屬於更後面的 type/match 深化工作，除非你使用的 revision 已明確實作。

## Option

```se
use option

name = option.some "Steve"
if option.is_some name
    say option.value name
```

Option helper 負責 some/none、狀態檢查、取值與 fallback。

## Result

```se
use result

answer = result.ok 42
problem = result.err "invalid input"
```

當 success/failure 本身應該是普通資料時，Result 可以比 control-flow error 更清楚。

## Managed Async Task

```se
use async

job = async.run slow_work 21
answer = async.await job
say answer
```

Runtime 提供 managed Task/await；這不代表任意 Interpreter VM code 都能 unrestricted parallel execution。

## Lightweight Database

```se
use db

store = db.open "data.se-db"
db.set store "name" "SE"
name = db.get store "name"
```

這是本機 persistent Text key/value store，適合小型工具、設定與教學；SQL、transaction、index 等 relational database 能力不屬於這個 API。

## HTTPS

```se
use https

body = try https.get "https://example.com"
say body
```

目前 TLS transport 使用系統 `curl`，直接利用成熟的 TLS / certificate validation，而不是在 SE Runtime 重新實作 TLS。

## 設計原則

這些能力的目標不是增加更多符號，而是把 `make`、Collections、Modules、`try` 等既有概念自然延伸到更進階程式。
