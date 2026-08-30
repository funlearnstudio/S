# SE 語法速查

[English version](syntax-reference.md)

這是一份精簡語法表。要看解釋與完整範例，請閱讀 [SE 完整教學](tutorial-zh-TW.md) 與 [語言參考](language-reference-zh-TW.md)。

## 基本形式

| 用途 | 語法 |
| --- | --- |
| 輸出 | `say value` |
| 輸入 | `name = ask "Question"` |
| Assignment | `name = value` |
| Compound assignment | `+= -= *= /= %=` |
| 註解 | `# text` |
| List | `[1, 2, 3]` |
| Map | `["key": value]` |
| Set | `set [1, 2, 3]` |
| Range | `1..10` |
| 函式 | `make name input` |
| 回傳 | `give value` |
| 模組 | `use module` |
| Runtime error | `fail "message"` |

## 條件判斷

```se
if condition
    ...
else if other_condition
    ...
else
    ...
```

較新的 source revision 也可能接受 `elif` 作為 `else if` alias。

## 迴圈

```se
repeat count
    ...

for item in values
    ...

for key value in map_value
    ...

while condition
    ...
```

## 函式

```se
make add a b
    give a + b

answer = add 2 3
```

Typed / Generic form：

```se
make identity[T] value:T -> T
    give value
```

## Type

```se
type User
    name = ""

    make hello
        say name

user = User
    name = "SE"
```

## 錯誤處理

```se
try
    value = read "data.txt"
else err
    say err.message
```

傳遞錯誤：

```se
make load
    give try read "data.txt"
```

## Match

```se
match value
    case 1
        ...
    case 2
        ...
    else
        ...
```

## Web Component

```se
make Button text
    html
        button text

    css
        padding 12

    js
        when click
            say text

page "/"
    Button "Save"
```

## Operators

大致 precedence 由高到低：

```text
**
unary - / not
* / %
+ -
..
< <= > >=
== !=
and
or
```

`**` 是 right-associative。

## Text Escape

Text 使用雙引號，支援 newline、tab、escaped quote 與 escaped backslash 等常見 escape。

## 低標點呼叫

一般函式呼叫通常不需要括號：

```se
add 2 3
```

遇到 grouping 歧義時再使用括號明確分組。
