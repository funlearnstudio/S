# SE 型別系統

本文件是 `types.md` 的繁體中文版。

SE 採用「盡量推斷、需要時才標註」的靜態型別方向。

## 基本型別

常見 Runtime / Checker 型別包括：

- `None`
- `Bool`
- `Int`
- `Num`
- `Text`
- `Bytes`
- `List`
- `Map`
- `Set`
- `Function`
- user-defined Object
- Module
- File / Path
- Duration / Time
- NativeHandle

## 型別推斷

```se
age = 15
name = "SE"
active = true
```

Checker 可以從值推斷 Int、Text、Bool，因此一般程式不需要重複寫型別。

## 明確函式型別

SE 0.6 泛型函式可以使用參數與回傳型別標註：

```se
make identity[T] value:T -> T
    give value
```

同一個 `T` 在呼叫時必須保持一致的型別關係。

## Collection element

List 等集合會盡可能保存元素型別資訊，但部分高階 generic collection API 目前仍可能回傳較寬鬆的 Unknown，這是型別系統後續要繼續加強的地方。

## Unknown

`Unknown` 表示 Checker 在靜態階段無法確定精確型別，例如 heterogeneous JSON parse 結果或某些 generic platform API。Unknown 不是「關掉型別安全」；需要 member/index 等操作時仍可能要求更明確的型別資訊。

## User type

```se
type User
    name = ""
    score = 0
```

建立 object 後，Checker 知道其 fields / methods。

## Option / Result

SE 0.6 用 managed runtime types 表示可能缺值或成功/失敗：

```se
use option
maybe = option.some "SE"

use result
answer = result.ok 42
```

## 長期方向

目前 generics 主要是 generic functions。完整 generic user types、constraints、精確 collection type propagation 與 narrowing 仍屬後續型別系統工作。
