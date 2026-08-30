# SE Roadmap 路線圖

[English version](roadmap.md)

這份 Roadmap 描述方向，不代表所有內容都已進入正式 Release。Stable behavior 以主要教學／Reference 為準；帶版本號的 design document 則描述對應 development stage。

## 已建立的 Foundation

SE 已有 C++20 implementation，包含 indentation-aware Lexer、Pratt Parser、AST、Static Checker / Type Inference、Interpreter、C++ Native Backend、Runtime values、Modules、User Types、Collections、Recoverable Errors、Testing、File/Path/Time、JSON/HTTP、Native C ABI interop 與 Web Compiler 工作。

## Type System

持續演進方向可能包含：

- 更精確的 generic collection type propagation
- implementation/revision 支援時的 generic user type
- constraints / protocol / trait-like capability，但前提是不能破壞 SE 簡潔語法
- 對 broad / Unknown value 做更安全的 narrowing

## Pattern Matching

未來可以繼續增加：

- destructuring pattern
- guard
- 更自然的 Option / Result pattern
- exhaustiveness analysis

除非 implementation 已經完成，否則 stable 文件不應把這些描述成現成功能。

## Async / Concurrency

可能方向：

- 更完整 scheduler/runtime structure
- non-blocking I/O integration
- 安全 concurrency model
- 一般 SE source 盡量不直接管理 thread / lock

## Web / Networking

SE Web 已經是實際 compiler target，後續可繼續：

- 更完整 multi-file component/project integration
- 更多 browser platform API
- deployment adapter / tooling
- WebSocket 或 streaming/event capability
- 更成熟 production server architecture

Browser 與 Server capability boundary 應持續保持明確。

## Data / Database

未來資料層可以增加更完整 SQL/database adapter、transaction、prepared query 與 connection/resource management，同時維持簡單 module/API model。

## Tooling

可能包含：

- LSP
- completion / hover / diagnostics
- formatter
- debugger integration
- package manager / registry
- distribution 改善

## Compiler / Build System

可能方向：

- Native optimization
- caching / incremental build
- relocatable installation
- 在值得增加複雜度時加入其他 code-generation backend

LLVM 可以是選項，但不是 SE 身分或成功的必要條件。

## 長期原則

> **Simple at every level.**

SE 應該透過 Compiler、Runtime、Standard Library 與 Tooling 承擔複雜度，而不是讓一般程式被迫增加 punctuation、manual ownership 或 platform boilerplate。
