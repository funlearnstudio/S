# 錯誤處理

本文件是 `errors.md` 的繁體中文版。

SE 讓可恢復錯誤保持簡單，不要求大量例外處理樣板。

## try / else

```se
try
    text = read "data.txt"
    say text
else err
    say err.message
```

如果 `try` 區塊中的 fallible 操作失敗，就進入 `else err`。

## try expression

可以對單一可能失敗的運算使用 `try expr`。

```se
try value = option.value maybe
```

## fail

```se
if age < 0
    fail "age cannot be negative"
```

`fail` 會建立執行期錯誤並停止目前流程，除非外層 `try` 接住。

## Error value

錯誤可包含 message、kind、來源與行號。一般程式最常使用：

```se
say err.message
```

## Option / Result

SE 0.6 另外提供 Option 與 Result，讓「沒有值」與「成功/失敗」可以成為明確資料，而不必全部依靠例外。

```se
use result

r = result.err "invalid"
if result.is_err r
    say result.error r
```
