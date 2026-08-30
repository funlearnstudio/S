# SE 技術參考

[English version](technical-reference.md)

這份文件整理 SE 的實作模型：compiler stages、runtime 邊界、Native Interoperability、Web compilation 與測試策略。實際 implementation 與未來 Roadmap 分開說明，避免版本內容混在一起。

## 1. Compiler Pipeline

```text
.se source
    ↓
Lexer
    ↓
Tokens + INDENT / DEDENT
    ↓
Pratt Parser
    ↓
AST
    ↓
Static Checker / Type Inference
    ├── Interpreter
    ├── C++20 Backend → system compiler → native executable
    └── Component Web Compiler → HTML/CSS/JS/TS
```

`se check` 做到 semantic checking；`se run` 交給 Interpreter；`se build` 產生 C++20 並呼叫系統 compiler；`se web build` 走 Web compiler path。

## 2. Lexer 與縮排

SE block 不使用 `{}`，Lexer 會產生 `INDENT` / `DEDENT`：

```se
if ready
    say "yes"
say "done"
```

因此 Parser 收到的是明確 block boundary，而不是之後再猜 whitespace。

## 3. Parser 與 AST

Expression 使用 Pratt Parser 處理 precedence 與 associativity。Statement layer 負責 assignment、control flow、function、type、module、error 等語法。

Checker、Interpreter 與各 code generation path 共用 AST，避免每個 backend 各自重新解析 source。

## 4. Static Checking 與 Type Inference

```se
age = 15
name = "SE"
ready = true
```

SE 在型別明確時避免要求重複 annotation，但推斷後仍檢查不相容 operation、錯誤 call、未知名稱與 collection/type 問題。

> 少寫型別語法，不等於沒有型別系統。

## 5. Runtime Values 與 Safety

重要 value family：

```text
None Int Num Bool Text Bytes
List Map Set
Function Object Error
Duration Path File
managed native handles
```

一般 SE 程式不直接管理 raw pointer、`new/delete`、`malloc/free`、C++ template 或任意 C++ object ABI。

Index 越界、arity 錯誤、無效 member、I/O failure、native failure 等情況應轉成 SE-level error，而不是把 C++ undefined behavior 暴露出去。

## 6. Function、Closure、Type

`make` 同時用於 function 與 method。Lexical environment 支援 closure。User-defined type 包含 field 與 method；method field lookup 可以解析 current object field，不需要到處強制寫 `self.`。

SE 已有 generic function。Generic user type 的版本化功能另外放在專門文件，避免和 stable release 狀態混淆。

## 7. Modules

`use module` 由 module loader 載入 source、standard 或 native module。新 source 優先使用 `.se`；舊 `.s` 只保留 migration compatibility。Circular dependency chain 會被拒絕。Top-level `_name` 採 private convention。

## 8. Errors

Compiler/checker error 與 recoverable runtime error 是不同層級。Runtime failure 可以用 `try ... else err` 處理、用 `fail` 建立，並在支援的位置用 `try expr` 傳遞。

## 9. Collections 與資料

List operation 會做 bounds check。Map / Set 使用 managed collection semantics。Text 與 binary-safe Bytes 是不同 value type。JSON 會映射成 SE collection/scalar value。

## 10. Task 與 Async

Runtime 提供 managed Task-style async/await。進入 Interpreter VM 的 callback 會序列化，因此不能把它描述成任意 SE code 的 unrestricted parallel execution。

SE Web event handler 對支援的 `async.await` expression 可以降低成 browser JavaScript `await`。

## 11. HTTP 與 Web Server

SE 提供 HTTP client；HTTPS 目前透過系統 `curl` transport。Built-in HTTP server/router 是 synchronous/blocking，定位在學習、開發與小型 service，不應描述成 hardened high-concurrency production server。

## 12. SE Web Compiler

Component Web source 沿用 `make`，搭配可選的 `html`、`css`、`js` section 與 `page` declaration。Compiler 輸出標準 Web files，並注入 Browser Runtime 供 request、routing 與 DOM helper 使用。

Browser 與 Server 能力邊界不同。Filesystem、Native FFI、database secret 與 server listener 不會自動變成 browser API。

## 13. Native C ABI Bridge

建議邊界：

```text
C/C++ library
    ↓
C ABI wrapper
    ↓
.snative metadata
    ↓
SE
```

Bridge 支援常見 scalar、Text、Bytes 與 managed opaque handle。Native resource 可以指定 cleanup function，讓 resource lifetime 維持 managed，而不是把 raw address 暴露給一般 SE code。

## 14. Binding Generator

```bash
se bind module.sbind generated
```

Binding path 會對支援的 signature 產生 metadata/header/wrapper；不支援的 signature 應明確拒絕，而不是靜默產生不完整 binding。

## 15. Build 與 Warning Policy

SE 本身使用 CMake + C++20。GCC/Clang build 採 `-Wall -Wextra -Wpedantic -Werror` 等嚴格 warning；MSVC 使用對應的高 warning/error 設定。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## 16. Testing Strategy

Regression coverage 應包含 Lexer/Parser/Checker、Interpreter、Native build parity、Collections、Types、Modules、Errors、Native ABI/Bytes ownership、resource cleanup、binding generation、Web compilation 與 CLI smoke test。

Interpreter/native parity 很重要，因為同一份 SE source 不應只因為從 `se run` 換成 `se build` 就偷偷改變語意。

## 17. Naming Compatibility

SE 是早期 S codebase 的後繼名稱。內部 C++ namespace、header path、target name 或 legacy `.s` example 仍可能出現 `s` 以維持 implementation compatibility。使用者-facing 名稱是 **SE**、CLI `se`、source `.se`。

## 18. 設計原則

> Simple at every level.

複雜度應優先放在 compiler、runtime、standard library 與 native bridge，而不是要求一般程式反覆處理 punctuation、ownership、ABI detail 與 boilerplate。
