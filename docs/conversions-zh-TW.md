# SE 核心型別轉換

SE 0.6 開發線加入一組可直接使用、不需要 `use` 的核心轉換函式。

## 設計原則

SE 仍然以自己的核心型別名稱為準：

- `Text`：文字
- `Int`：整數
- `Num`：浮點數
- `Bool`：布林值

為了讓 C、C++、Java、JavaScript 等語言的使用者更容易上手，也提供熟悉的別名轉換函式。這些名稱是**轉換函式**，不是額外建立一套重複的 runtime 型別。

## 文字

```se
say text 123
say string 123
```

`text` 與 `string` 都回傳 `Text`。

## 整數

```se
age = int "15"
count = integer "42"
```

`int` 與 `integer` 都回傳 `Int`。

也可以處理本來就是 Int 的值，以及沒有小數部分的 Num：

```se
say int 12
say int 12.0
```

不能完整轉成整數時會產生清楚的 conversion error，而不是默默回傳 0。

## 浮點數

```se
price = num "19.95"
a = double "3.14"
b = float 2
```

`num`、`double`、`float` 都回傳 SE 的 `Num`。

SE 不另外建立 `Double` 與 `Float` runtime 型別；這些名稱是方便不同語言使用者的轉換別名。

## Bool

```se
ready = bool "true"
enabled = boolean "FALSE"
```

`bool` 與 `boolean` 都回傳 `Bool`。文字輸入接受大小寫不同的 `true` / `false`。

## char

SE 目前沒有獨立的 `Char` runtime 型別。`char` 會回傳「剛好一個 Unicode 字元」的 `Text`。

```se
letter = char "A"
accent = char "é"
han = char "中"
```

也可以從 Unicode code point 建立：

```se
letter = char 65
say letter
```

輸出：

```text
A
```

如果文字包含零個或多個字元，`char` 會拒絕它。

## 與 ask 一起使用

這是加入轉換功能的重要用途：

```se
number = int ask "Enter a number (2-1000): "

while number < 2 or number > 1000
    number = int ask "Please enter a number between 2 and 1000: "
```

因此 `ask` 可以維持最簡單的「取得 Text」規則，需要數字時再明確轉換。

## 完整別名表

| 寫法 | 結果型別 | 備註 |
|---|---|---|
| `text value` | `Text` | SE 原生名稱 |
| `string value` | `Text` | 熟悉別名 |
| `int value` | `Int` | SE 常用簡寫 |
| `integer value` | `Int` | 完整名稱別名 |
| `num value` | `Num` | SE 原生浮點概念 |
| `double value` | `Num` | C/C++/Java 風格別名 |
| `float value` | `Num` | 常見別名 |
| `bool value` | `Bool` | SE 常用簡寫 |
| `boolean value` | `Bool` | 完整名稱別名 |
| `char value` | `Text` | 一個 Unicode 字元 |
