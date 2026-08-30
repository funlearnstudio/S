# SE 0.7 Generic User Types

[English version](generic-user-types-0.7.md)

這份版本化文件描述建立在 Structured TypeRef 之上的 generic user-type stage；它和 stable 0.6 的 generic-function Reference 分開。

## 基本語法

```se
type Box[T]
    value:T

box = Box[Int]
    value = 42
```

`Box[Int]` 會把 `T` 特化成 `Int`，因此 `value` 會以 `Int` 做 static check。

## 多個 Type Parameter

```se
type Pair[A, B]
    first:A
    second:B

pair = Pair[Text, Int]
    first = "SE"
    second = 7
```

Type argument 數量會在 static checking 階段驗證。

## Typed / Required Field

既有 default field 仍然有效：

```se
type User
    name = ""
```

也可以寫明確型別與 default：

```se
type User
    name:Text = ""
```

沒有 default 的 typed field 在 object initialization 時是必要欄位：

```se
type User
    name:Text
    age:Int

user = User
    name = "Milo"
    age = 15
```

## Method 與 Type Parameter

```se
type Box[T]
    value:T

    make get -> T
        give value

    make replace next:T
        value = next
```

對 `Box[Int]` 而言，`get` 回傳 `Int`，`replace` 也只接受 `Int`。

## Generic Function + Generic User Type

```se
make unwrap[T] box:Box[T] -> T
    give box.value
```

傳入 `Box[Int]` 時，`T` 會綁定成 `Int`，包含回傳型別。

## Nested Specialization

Generic substitution 可以繼續穿過 nested user/collection type：

```se
type Wrapper[T]
    item:Box[T]
```

## Runtime Model

此 stage 採 runtime erasure：不同 specialization 共用 runtime type representation，而完整 specialization information 由 Static Checker 保留與驗證。

## Compatibility

非泛型 user type、default field、method 與一般 object initialization 保持有效。Generic type application 使用 `Box[Int]` 等形式，讓 type specialization 和一般 value indexing 維持清楚區別。

## 目前邊界

此階段涵蓋 invariant generic user type 與 specialization。Traits/interfaces、variance、generic constraints、higher-kinded type、specialization overloading 不會因這份文件而自動視為已支援；只有後續 implementation 明確加入時才算完成。
