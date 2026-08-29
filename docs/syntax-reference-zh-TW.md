# SE 語法速查

本文件是 `syntax-reference.md` 的繁體中文版。

## 輸出 / 輸入

```se
say "Hello"
name = ask "Name"
```

## 變數

```se
x = 10
name = "SE"
```

## 條件

```se
if x > 5
    say "large"
else
    say "small"
```

## 迴圈

```se
repeat 3
    say "hi"

for n in 1..5
    say n

while x > 0
    x = x - 1
```

## 函式

```se
make add a b
    give a + b

answer = add 2 3
```

## 泛型函式

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
```

## List / Map

```se
nums = [1, 2, 3]
user = ["name": "SE", "score": 100]
```

## Module

```se
use math
say math.sqrt 9
```

## 錯誤

```se
try
    value = read "data.txt"
else err
    say err.message
```

## Pattern matching

```se
match status
    case 1
        say "one"
    else
        say "other"
```

## 低標點原則

SE 正常程式盡量省略 `;`、`{}` 與不必要的 `()`。函式呼叫通常寫成：

```se
add 2 3
```

當巢狀低標點呼叫可能產生歧義時，可以使用括號明確分組。
