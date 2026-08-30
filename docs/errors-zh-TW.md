# SE 錯誤處理

[English version](errors.md)

SE 會把 compile-time/checker error 與 recoverable runtime error 分開。

## try / else

```se
try
    text = read "data.txt"
    say text
else err
    say err.message
```

如果 `try` block 中出現可恢復的 runtime failure，流程會進入 `else err`。

## Error Value

Error 可以提供：

```text
message
kind
source
line
```

例如：

```se
say err.message
say err.kind
```

## fail

建立 application/runtime error：

```se
make check score
    if score < 0
        fail "Invalid score"
    give score
```

`fail` 會中止目前流程，除非外層 recoverable-error boundary 接住它。

## try Expression

在支援的 expression context 中，`try expr` 可以把 recoverable failure 往外傳遞：

```se
make load
    give try read "data.txt"
```

只有被定義為 fallible 的 operation 才需要這種處理。

## Compile-time Error

Syntax error、未知名稱、不相容型別與其他 checker failure 都屬於 compiler error。它們不是一般 runtime value，因此不能靠程式內的 `try` 捕捉。

## Option / Result

當「沒有值」或「成功／失敗」本身應該是普通資料時，可以使用 Option / Result helper，而不是全部依靠 control-flow error。

```se
use result

r = result.err "invalid"
if result.is_err r
    say result.error r
```

Runtime error 適合真正的 fallible operation；Option / Result 適合把狀態明確建模成值。
