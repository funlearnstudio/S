# SE 0.6 進階 API 參考

[English version](advanced-0.6-api.md) · [概念教學](advanced-0.6-zh-TW.md)

這份文件是 0.6 advanced runtime layer 的精簡 API Reference。

## collections

```text
collections.filter list predicate -> List
collections.map list transform -> List
collections.reduce list initial reducer -> value
collections.sort_by list key -> List
collections.sort_by_desc list key -> List
collections.sort_with list comparator -> List
collections.slice list start end -> List
collections.take list count -> List
collections.drop list count -> List
```

這些 helper 原則上回傳新 List，不暴露 host container mutation detail。

## function

```text
function.bind fn values... -> Function
function.call fn values... -> value
function.pipe value fn... -> value
```

巢狀 `make` 另外支援 lexical closure。

## async

```text
async.run fn values... -> Task
async.ready task -> Bool
async.await task -> value
```

Task failure 可進入 recoverable error model。Interpreter VM callback 會依 implementation 做 managed/serialized execution；這個 API 不代表任意 VM code 都能 unrestricted parallel execution。

## option

```text
option.some value -> Option
option.none -> Option
option.is_some option -> Bool
option.is_none option -> Bool
option.value option -> value
option.or option fallback -> value
```

對 `none` 直接取 value 會產生 Option-level error。

## result

```text
result.ok value -> Result
result.err text -> Result
result.is_ok result -> Bool
result.is_err result -> Bool
result.value result -> value
result.error result -> Text
result.or result fallback -> value
```

## match Helpers

```text
match.value value pattern handler ... fallback -> value
match.option option some_handler none_handler -> value
match.result result ok_handler error_handler -> value
```

語言本身另外有 `match / case / else` statement。

## Local Database

```text
db.open path -> Database
db.set db key value -> None
db.get db key -> Option
db.has db key -> Bool
db.remove db key -> Bool
db.keys db -> List
db.save db -> None
```

這是本機 persistent Text key/value store，不是 relational SQL database。

## HTTPS

```text
https.get url -> Text
https.post url body -> Text
https.post_json url json_text -> Text
```

0.6 HTTPS transport 使用系統 `curl` 處理 TLS / certificate。

## Generic Function Syntax

```se
make identity[T] value:T -> T
    give value
```

0.6 generic support 主要是 generic function；更深入 generic user type 工作放在 0.7 versioned document。

## Value Match Syntax

```se
match status
    case 1
        say "ready"
    case 2
        say "running"
    else
        say "unknown"
```

此 stage 使用 value/equality pattern，不宣稱已有完整 destructuring / guard。
