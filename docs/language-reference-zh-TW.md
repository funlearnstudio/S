# SE 語言參考

本文件是 `language-reference.md` 的繁體中文版，快速列出目前語法與核心語意。

## 原始碼

新檔案使用 `.se`。舊 `.s` 暫時保留相容性。

## 註解

```se
# comment
```

## Literal

```se
123
3.14
true
false
"text"
[1, 2, 3]
["name": "SE"]
```

Duration 支援 `ms`、`s`、`min`。

## 運算

算術：`+ - * / % **`

比較：`== != > >= < <=`

邏輯：`and or not`

Range：`1..10`

## 控制流程

```se
if condition
    ...
else
    ...

repeat count
    ...

for item in values
    ...

while condition
    ...
```

## 函式

```se
make name a b
    give value
```

呼叫通常不需要括號：

```se
result = add 2 3
```

必要時可用括號解除低標點呼叫歧義。

## 型別

```se
type User
    name = ""

    make hello
        say name
```

## 模組

```se
use module
```

以下劃線開頭 `_name` 表示 module private convention。

## 錯誤

```se
try
    ...
else err
    ...

fail "message"
```

## 泛型函式

```se
make identity[T] value:T -> T
    give value
```

## Pattern matching

```se
match value
    case 1
        ...
    else
        ...
```

目前是 value pattern。

## Member / index

```se
value.member
list[index]
map["key"]
value.help
```

`.help` 可用來查看值可用的成員與基本說明。
