# SE 型別與方法

[English version](types.md)

SE 採用 **能推斷就推斷、需要時再標註** 的靜態型別方向。沒有寫出 annotation，不代表沒有 type checking。

## 核心 Value Family

常見 Checker / Runtime type：

```text
None
Bool
Int
Num
Text
Bytes
List
Map
Set
Function
Object
Error
Duration
Path / File
Module
NativeHandle
```

## Type Inference

```se
age = 15
name = "SE"
active = true
```

Checker 會從 literal 與 expression 推斷明確型別。

## Typed Function

```se
make add a:Int b:Int -> Int
    give a + b
```

當 API boundary 或 inference 不夠明確時，可以加入 annotation。

## Generic Function

```se
make identity[T] value:T -> T
    give value
```

同一個 generic parameter 在一次 call 中必須維持一致的型別關係。

## User-defined Type

```se
type Player
    name = ""
    hp = 100

    make hit damage
        hp = hp - damage

    make alive
        give hp > 0

player = Player
    name = "Steve"

player.hit 20
say player.hp
```

Field type 通常從 default value 推斷，也可以在支援的位置加入明確 annotation。不相容 assignment 會成為 checker error。

## Method Field Lookup

Method 內未被 local variable shadow 的 field 名稱可以解析成 current object field：

```se
make hit damage
    hp = hp - damage
```

因此一般 SE code 不必每次都強制寫 `self.`。

## Object Lifetime

每次建立 object 都會取得獨立 field storage。一般 SE 程式不管理 raw pointer 或 object memory；生命週期由 runtime 管理。

## Collection Typing

List / Map / Set 在 Checker 能推斷時會保留型別資訊。部分 heterogeneous data 或 platform API 可能只能得到較寬鬆的型別。

## Unknown

`Unknown` 表示靜態階段無法確定精確型別，例如 heterogeneous JSON 或部分 dynamic platform API。它不是「關掉整個 type checker」；需要 member/index 等操作時仍可能要求更明確資訊。

## Option / Result

SE 提供 managed Option / Result helper，讓「可能沒有值」或「成功／失敗」可以成為明確資料：

```se
use option
maybe = option.some "SE"

use result
answer = result.ok 42
```

## Generic User Type 的版本邊界

Generic function 已屬既有語言能力；Generic user type 與更深的 type-system work 另外放在帶版本號的設計／Reference 文件中，避免把未來工作和 stable release 混在一起。
