# SE 語言導覽

本文件是 `language-guide.md` 的繁體中文版，提供比快速開始更完整、但比技術參考更輕量的語言介紹。

## 基本輸出與輸入

```se
say "Hello"
name = ask "Name"
say "Hello " + name
```

## 變數與推斷

```se
age = 15
name = "SE"
active = true
```

一般情況不需要先宣告型別，Checker 會推斷。

## 控制流程

```se
if age >= 15
    say "yes"
else
    say "no"

repeat 3
    say "hi"

for n in 1..5
    say n
```

## 函式

```se
make add a b
    give a + b

answer = add 2 3
```

## 型別與方法

```se
type Dog
    name = ""

    make bark
        say name + " says woof"

dog = Dog
    name = "Milo"

dog.bark
```

## 模組

```se
use math
say math.sqrt 25
```

## 錯誤

```se
try
    text = read "data.txt"
else err
    say err.message
```

## SE 0.6 高階能力

SE 現在也支援 higher-order collections、lexical closure、泛型函式、value pattern matching、Option/Result、Task-style async、database 與 HTTPS。

```se
make identity[T] value:T -> T
    give value
```

```se
match status
    case 1
        say "ready"
    else
        say "other"
```

完整內容請看 `complete-language-guide-zh-TW.md` 與 `advanced-0.6-zh-TW.md`。
