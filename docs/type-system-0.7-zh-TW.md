# SE 0.7 型別深度：Structured TypeRef

SE 0.7 的第一個型別系統基礎是 **Structured TypeRef**。目的不是增加更多標點，而是讓 Compiler 真正理解巢狀型別結構，而不是把型別註記只當成單一文字名稱。

## 基本型別註記

既有 SE 0.6 語法保持相容：

```se
make add a:Int b:Int -> Int
    give a + b
```

## List / Set

現在函式型別註記可以保留元素型別：

```se
make first[T] values:List[T] -> T
    give values[0]
```

也可以直接使用具體型別：

```se
make total values:List[Int] -> Int
    sum = 0
    for value in values
        sum = sum + value
    give sum
```

`Set[T]` 使用相同結構。

## Map

SE 目前的 Map key 仍固定為 Text，所以完整註記寫成：

```se
Map[Text, Int]
Map[Text, User]
```

例如：

```se
make lookup[T] values:Map[Text, T] key:Text -> T
    give values[key]
```

如果寫成 `Map[Int, Text]`，Checker 會直接拒絕，因為目前 SE Map runtime 不支援非 Text key。

## 巢狀型別

TypeRef 可以遞迴巢狀：

```se
List[List[Int]]
List[Map[Text, Int]]
Map[Text, List[Text]]
```

例如：

```se
make nested[T] values:List[Map[Text, T]] -> T
    give values[0]["value"]
```

Compiler 會保留 `T` 在巢狀 List / Map 裡的位置，call site 再遞迴推斷。

## 泛型推斷

```se
make same[T] values:List[T] value:T -> T
    give value

say same [1, 2] 3
```

這裡 `List[T]` 先讓 Checker 推斷 `T = Int`，所以第二個 `value:T` 也必須是 Int。

以下程式會在靜態檢查階段失敗：

```se
say same [1, 2] "bad"
```

因為 `T` 已經由第一個參數推斷為 Int。

## AST

Function 的參數與回傳型別現在使用結構化 TypeRef：

```text
TypeRef
├── name
└── args[]
```

因此：

```text
List[Map[Text, T]]
```

不再只是字串，而是：

```text
TypeRef("List")
└── TypeRef("Map")
    ├── TypeRef("Text")
    └── TypeRef("T")
```

這個結構會一路保留到 Parser AST、Static Checker，以及 native C++ AST serializer。

## 相容性

以下舊語法都仍然有效：

```se
make hello name
    say name

make add a:Int b:Int -> Int
    give a + b

make identity[T] value:T -> T
    give value
```

沒有型別註記時仍使用既有 inference。

## 目前邊界

這一階段完成的是 **型別參照與巢狀 generic annotation foundation**，不是完整 generic user type。

目前還沒有正式開放：

```se
type Box[T]
    ...
```

如果在普通 user type 名稱後加入 type arguments，Checker 會明確告知這屬於下一階段，而不是默默把型別降成 Unknown。

後續會在這個 TypeRef 基礎上加入：

- generic user types
- typed Option / Result
- 更精準的 collections 高階函式型別
- choice / sum types
- destructuring + exhaustive match

這些功能都共用同一套巢狀型別表示，避免每個進階功能各自發明一套型別語法。
