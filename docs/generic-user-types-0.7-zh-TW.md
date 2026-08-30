# SE 0.7 泛型使用者型別

SE 0.7 的型別深度工作加入泛型使用者型別。目標不是增加很多特殊符號，而是讓同一個簡單的 `type` 模型可以安全地描述可重用資料結構。

## 基本語法

```se
type Box[T]
    value:T

box = Box[Int]
    value = 42

say box.value
```

`T` 是 `Box` 的型別參數。`Box[Int]` 會把 `T` 特化成 `Int`，因此 `value` 也會是 `Int`。

下面的程式會在 `se check` 階段被拒絕：

```se
type Box[T]
    value:T

box = Box[Int]
    value = "wrong"
```

## 多個型別參數

```se
type Pair[A, B]
    first:A
    second:B

pair = Pair[Text, Int]
    first = "SE"
    second = 7
```

型別參數數量必須完全符合宣告。`Pair[Int]` 或 `Pair[Int, Text, Bool]` 都是靜態錯誤。

## 欄位型別與預設值

舊有 SE 寫法仍然有效：

```se
type User
    name = ""
    age = 0
```

0.7 也允許明確欄位型別：

```se
type User
    name:Text = ""
    age:Int = 0
```

沒有預設值的 typed field 是必要欄位：

```se
type User
    name:Text
    age:Int

user = User
    name = "Milo"
    age = 15
```

如果必要欄位沒有在 indented initialization 中提供，checker 會報錯。

## 方法可以使用型別參數

```se
type Box[T]
    value:T

    make get -> T
        give value

    make replace next:T
        value = next
```

對 `Box[Int]` 而言，checker 會把 `get` 視為回傳 `Int`，並要求 `replace` 的參數是 `Int`。對 `Box[Text]` 則會自動特化成 `Text`。

## 泛型函式與泛型型別可以一起使用

```se
make unwrap[T] box:Box[T] -> T
    give box.value

box = Box[Int]
    value = 42

say unwrap box
```

呼叫 `unwrap box` 時，checker 會從 `Box[Int]` 推斷 `T = Int`，所以回傳型別也是 `Int`。

## 巢狀泛型型別

```se
type Box[T]
    value:T

type Wrapper[T]
    item:Box[T]

box = Box[Int]
    value = 42

wrapper = Wrapper[Int]
    item = box
```

型別替換會遞迴進入使用者型別、List、Set 與 Map，而不是在外層退化成 Unknown。

## 與 SE 0.6 的相容性

非泛型 `type`、預設值欄位、方法與物件初始化語法保持相容。泛型使用者型別採用 PascalCase 型別名稱，例如 `Box[Int]`。這也讓 parser 能把型別套用與普通 collection indexing 清楚區分；一般值仍建議使用小寫名稱。

## Runtime 模型

目前泛型使用者型別採 **runtime erasure**：`Box[Int]` 與 `Box[Text]` 共用同一個 runtime `TypeData` 定義，型別參數的安全性由 static checker 保證。這避免為泛型引入額外 runtime 複雜度，同時保留 SE 的簡單執行模型。

因此 0.7 的承諾是：

> 表面仍然只是 `type Box[T]`，但 compiler 會保留並檢查它的完整特化型別。

## 目前邊界

這個階段提供 invariant 的泛型使用者型別與特化，不包含 traits / interfaces、variance、generic constraints、higher-kinded types 或 specialization overloading。這些能力只有在能維持 `Simple at every level` 的前提下才會考慮加入。
