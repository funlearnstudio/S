# SE Roadmap 路線圖

本文件是 `roadmap.md` 的繁體中文版。Roadmap 表示方向，不代表所有項目都已完成。

## 已建立的基礎

目前 SE 已具備 Lexer、INDENT/DEDENT、Pratt Parser、AST、Static Checker、Type Inference、Interpreter、C++20 native backend、Runtime、modules、types、collections、錯誤處理、測試、Web/HTTP、C ABI 與 SE 0.6 高階功能。

## 近期方向

### 型別系統

- generic user types，例如 `type Box[T]`
- generic collections 的更精確型別傳播
- constraints / protocol / trait 類能力，但語法必須維持 SE 的簡潔原則
- 改善 `Unknown` 的安全 narrowing

### Pattern matching

- destructuring
- guards
- Option / Result 更自然的 match pattern
- exhaustiveness checking

### Async / concurrency

- 更完整的 scheduler
- 非 blocking I/O
- 安全的 concurrency model
- 保持一般使用者不必直接管理 thread / lock 的方向

### Web / network

- 更成熟的 HTTPS / TLS 整合
- production-oriented server architecture
- structured request / response types
- WebSocket 等即時能力

### Database

- SQLite 或標準 SQL adapter
- transaction
- prepared query
- connection/resource safety

### Tooling

- LSP
- autocomplete / hover / diagnostics
- formatter
- debugger
- package manager / registry
- 更完整 Windows distribution

### Compiler

- 更佳 native optimization
- relocatable installation
- optional LLVM backend
- incremental build / caching

## 長期目標

SE 的長期目標不是單純增加功能，而是維持：

> **Simple at every level.**

初學、Web、原生程式、系統互通、資料處理與進階抽象應盡量共用同一套簡單、低標點、可推斷的語言模型。
