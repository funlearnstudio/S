# SE 深度能力路線圖：0.7 → 1.0

SE 的目標不是只讓 Hello World 簡單，而是讓型別系統、錯誤處理、並行、網路、資料庫與大型程式仍維持 **Simple at every level**。

這份路線圖把「增加能力」和「增加語法複雜度」分開處理。新增功能必須先證明它真的增加語言深度，而且不能只是把 C++ / Rust / Nim / V 的語法搬進 SE。

## 設計守則

每一個進階功能都必須符合以下規則：

1. 普通程式不需要看到進階機制。
2. Compiler 優先承擔型別推導、資源管理與樣板程式。
3. 同一件常見工作盡量只有一種主要寫法。
4. 低標點仍是預設；只有在資訊真的無法省略時才加入符號。
5. 靜態檢查要比語法更嚴格，不能用「簡單」當成放棄安全性的理由。
6. 新功能必須同時有 Parser、AST、Checker、Interpreter、native build、diagnostics、tests、docs 的完整支援才算完成。

---

# SE 0.7 — Type Depth

0.7 的核心不是再加更多 module，而是把 SE 的型別系統變成可以支撐大型程式的基礎。

## 1. 結構化 TypeRef

目前 function annotation 在 AST 內主要以字串保存。0.7 應改成真正的 TypeRef AST，例如：

```text
Int
List[Int]
Map[Text, User]
Result[User, Error]
Box[T]
```

目標：Parser 不再把型別當普通文字，Checker 也不再靠字串比對解析複合型別。

驗收：

```se
make total nums:List[Int] -> Int
    ...

make names users:List[User] -> List[Text]
    ...
```

必須能完整靜態檢查。

## 2. Generic user types

從目前 generic function：

```se
make identity[T] value:T -> T
    give value
```

擴展到：

```se
type Box[T]
    value:T
```

以及：

```se
type Pair[A, B]
    first:A
    second:B
```

目標包括 generic substitution、member type preservation、nested generic types、錯誤訊息。

不在第一階段加入複雜 trait constraint 語法。

## 3. Choice / Sum Types

SE 需要原生「一個值可能是幾種合法狀態之一」的能力，而不是全部依靠 Unknown 或 runtime tag。

建議語法：

```se
choice Shape
    Circle radius:Num
    Rectangle width:Num height:Num
    Point
```

建立值：

```se
shape = Circle 10
```

這會成為 Option / Result、AST、protocol message、state machine 的型別基礎。

## 4. 深度 match

0.6 的 match 是 value equality matching。0.7 應升級為 typed pattern matching：

```se
match shape
    case Circle radius
        say radius
    case Rectangle width height
        say width * height
    case Point
        say 0
```

並加入 exhaustiveness checking：如果 `Shape` 有三個 variant，只處理兩個而沒有 `else`，checker 必須報錯。

之後才考慮 guard：

```se
case Circle radius if radius > 100
```

## 5. Option[T] / Result[T, E] 進入型別系統

0.6 已有 option / result module。0.7 應讓它們成為真正 parameterized types：

```se
make find_user id:Int -> Option[User]

make load path:Text -> Result[Text, Error]
```

目標：`option.value` / `result.value` 不再丟失實際元素型別。

## 6. Function types

高階函式目前可執行，但需要更完整的靜態型別表達。

建議 annotation：

```text
Function[Int -> Bool]
Function[Int, Int -> Int]
```

讓 collections.filter / map / reduce 可以保留型別：

```se
make double n:Int -> Int
    give n * 2

values = collections.map nums double
# values 應推導為 List[Int]，不是 List[value]
```

## 0.7 Release Gate

0.7 不應在以下條件完成前發布 stable：

- nested generic annotation 可解析
- generic substitution 可穿過 List / Set / Map / user type
- generic user type 可建立與存取 member
- choice type 可建立
- match 可 destructure choice variant
- exhaustive match 有 checker diagnostics
- Option / Result 保留 element/error types
- collections.map/filter/sort 不再無故退化成 Unknown
- interpreter / native build parity
- Linux / macOS / Windows CI 全綠
- 完整 zh-TW + English reference

---

# SE 0.8 — Safe Concurrency

0.6 的 async Task 是安全的 managed task，但 callback 進 interpreter VM 仍會序列化。0.8 才處理真正 concurrency。

## 1. Runtime scheduler

建立明確 Task scheduler，不讓任意 thread 同時直接操作 VM state。

## 2. Channels

建議維持低語法：

```se
channel = channel[Int]
channel.send 10
value = channel.receive
```

後續可以讓 compiler 推導型別，常見情況甚至不必寫 `[Int]`。

## 3. Parallel work

只有能證明沒有共享可變狀態問題時，才提供真正平行執行。

候選 API：

```se
results = parallel.map values work
```

比直接暴露 mutex / atomic / thread lifecycle 更符合 SE。

## 4. Cancellation / timeout

Task 必須能結構化取消：

```se
job.cancel
answer = async.await job timeout 5s
```

具體語法需在實作前做 parser ambiguity review。

---

# SE 0.9 — Production Platform

0.9 的目標是把「能展示」提高到「能拿來做真正工具與服務」。

## 1. HTTP / HTTPS 統一

- 同一套 request / response API
- HTTPS 不再要求使用者理解 transport 差異
- connection timeout
- headers
- status code
- redirects
- binary body
- streaming

若仍依賴 curl，必須清楚標示；長期應改成可隨 SE binary 發布的成熟 TLS implementation，而不是自己寫 TLS。

## 2. SQL database layer

保留現有 file-backed KV DB，另加真正 relational database abstraction：

```se
db = sql.open "app.db"
rows = db.query "select name from users"
```

第一個 backend 建議 SQLite；PostgreSQL / MySQL 之後再加 driver。

必須支援 prepared statement / parameter binding，避免 API 鼓勵 SQL injection。

## 3. Package manager

預定 CLI：

```text
se add package
se remove package
se install
se update
```

需要 manifest、lock file、semantic versioning、checksum、registry / Git dependency policy。

## 4. Test framework 深化

```se
test "adds numbers"
    expect add 2 3 == 5
```

語法只是候選；真正設計前需確保不破壞現有 `se test`。

---

# SE 1.0 — Language Contract

1.0 不是「功能很多」就發布，而是語言契約穩定。

## 必須穩定的部分

- lexical grammar
- indentation rules
- core expressions/statements
- type inference rules
- generic semantics
- error propagation semantics
- module visibility
- object / choice model
- collection safety
- async task semantics
- FFI ABI versioning policy
- backward compatibility policy

## Native build

目前一般 `se run/check/test` 可以直接使用預編譯 SE，但 `se build` 仍需要系統 C++20 compiler。

1.0 前要做一次正式決策：

1. 保留 C++ backend，但隨 SE 發布完整 toolchain；或
2. 增加直接 native backend，例如 LLVM / Cranelift 類型方案；或
3. C++ backend 保留為 portable backend，另有預設 native backend。

不能只是把 compiler requirement 隱藏起來；必須讓 `se build` 的部署模型真正清楚。

---

# 明確不做的事

為了避免 SE 變成「語法少一點的 C++」，以下能力不應因為其他語言有就直接加入：

- 任意 textual macro
- preprocessor language
- operator overloading 大全
- 多套互相重疊的 iteration syntax
- 需要大量 lifetime punctuation 的 user-facing model
- 讓 package 自行改寫 parser grammar 的 macro system

如果未來需要 metaprogramming，優先研究 typed compile-time API，而不是文字替換 macro。

---

# 實作順序

建議嚴格依序做：

**Phase A — TypeRef foundation**

TypeRef AST → nested annotations → checker substitution → diagnostics → tests。

**Phase B — Generic containers**

List[T] / Set[T] / Map[K,V] → map/filter/reduce type preservation。

**Phase C — Generic user types**

Box[T] / Pair[A,B] → instance specialization → member/method substitution。

**Phase D — Choice + Match**

variant declarations → constructors → destructuring → exhaustive checking。

**Phase E — Option / Result integration**

把 0.6 module-compatible API 接到 typed representation，保持舊程式相容。

完成 0.7 後再開始 concurrency；不要同時大改 VM scheduler 和 type system。

---

# 最終衡量標準

SE 不應只問「比 Python 少幾個符號嗎？」。

真正的標準是：

> 當程式需要 generics、sum types、typed errors、concurrency、networking、database、FFI 時，使用者仍能用一套一致且容易讀的語法完成，而 compiler/runtime 承擔底層複雜度。

這才是 **Simple at every level** 的完整版本。
