# Runtime 設計

本文件是 `runtime-design.md` 的繁體中文版。

SE Runtime 負責把簡單的語言表面轉成安全、可管理的執行期行為。

## Value

所有 SE 執行期資料都以 Runtime Value 表示，例如：

- None
- Bool
- Int / Num
- Text
- Bytes
- List / Map / Set
- Object
- Module
- Function / Callable
- File / Path / Duration / Time
- NativeHandle

## Managed data

集合、物件與其他較大型資料由 Runtime 管理生命週期。一般 SE 程式不直接操作 raw pointer。

## Collections safety

List 索引會檢查範圍。錯誤索引應產生可讀錯誤，而不是造成未定義記憶體存取。

## Callable

函式在 Runtime 中可以成為 Value。SE 0.6 的巢狀 `make` 會保存 lexical environment，因此可建立 closure；`function.bind` 則可產生 partial application callable。

## Error model

Runtime failure 會包含 kind、message、source/line 等資訊，並可透過 SE 的 `try / else err` 處理。

## Resource lifetime

File 與 native handle 應由 Runtime 管理釋放。C ABI 的 opaque resource 可以附帶 deleter，讓離開生命週期時自動 cleanup。

## Platform modules

JSON、Web、process、DB、HTTPS 等平台能力透過 built-in module 暴露給 Interpreter，而不是把大量特殊語法硬塞進 Parser。

## 設計方向

Runtime 的核心原則是：

> 使用者看到簡單 API，但執行期仍要有明確型別、資源邊界、錯誤與安全檢查。

SE 可以讓 Runtime 內部更複雜，但不應把記憶體管理與大量樣板推回一般 SE 程式。
