# SE 0.6 進階 API 參考

本文件是 `advanced-0.6-api.md` 的繁體中文版，整理 SE 0.6 新增的高階 API。完整概念教學請搭配 `advanced-0.6-zh-TW.md`。

## collections

```se
use collections
```

- `collections.filter list predicate`：保留 predicate 回傳 true 的元素。
- `collections.map list transform`：逐項轉換並回傳新 List。
- `collections.reduce list initial reducer`：依序累積結果。
- `collections.sort_by list "key"`：依 Map key 或 object field 升冪排序。
- `collections.sort_by_desc list "key"`：降冪排序。
- `collections.sort_with list comparator`：使用 `(a, b) -> Bool` 比較函式。
- `collections.slice list start end`：取得 `[start, end)`；支援負索引。
- `collections.take list count`：取前 count 項。
- `collections.drop list count`：略過前 count 項。

以上操作都會回傳新 List，不直接改動原 List。

## function

```se
use function
```

- `function.bind fn value...`：預先綁定部分參數並回傳新 callable。
- `function.call fn value...`：以函式值呼叫。
- `function.pipe value fn...`：把值依序送進一串單參數函式。

巢狀 `make` 會建立 lexical closure，能捕捉外層環境。

## async

```se
use async
```

- `async.run fn args... -> Task`
- `async.ready task -> Bool`
- `async.await task -> value`

`await` 可能失敗，因此應放在 `try` 中。Interpreter 為了 VM 安全會序列化進入 SE VM 的 callback；這不是無限制多執行緒執行模型。

## option

```se
use option
```

- `option.some value`
- `option.none`
- `option.is_some option`
- `option.is_none option`
- `option.value option`
- `option.or option fallback`

`option.value` 對 none 取值會產生 `OptionError`。

## result

```se
use result
```

- `result.ok value`
- `result.err text`
- `result.is_ok result`
- `result.is_err result`
- `result.value result`
- `result.error result`
- `result.or result fallback`

## match helper

```se
use match
```

- `match.value value pattern handler ... fallback`
- `match.option option some_handler none_handler`
- `match.result result ok_handler error_handler`

語言本身另有 `match / case / else` statement。

## db

```se
use db
```

- `db.open path -> Database`
- `db.set db key value`
- `db.get db key -> Option`
- `db.has db key -> Bool`
- `db.remove db key -> Bool`
- `db.keys db -> List`
- `db.save db`

目前是本機檔案型 **Text key/value store**，不是 SQL database。

## https

```se
use https
```

- `https.get url -> Text`
- `https.post url body -> Text`
- `https.post_json url json_text -> Text`

HTTPS 目前使用系統 `curl` 處理 TLS 與憑證驗證，因此 `curl` 必須存在於 PATH。

## 泛型函式

```se
make identity[T] value:T -> T
    give value
```

SE 0.6 的泛型範圍是 generic functions；尚未宣稱支援 generic user types、traits 或 constraints。

## 模式匹配

```se
match status
    case 1
        say "ready"
    case 2
        say "running"
    else
        say "unknown"
```

目前支援 value pattern；destructuring 與 guard 尚未加入。
