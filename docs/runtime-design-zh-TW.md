# SE Runtime 設計

[English version](runtime-design.md)

SE Runtime 的工作，是把簡單的語言表面轉成有檢查、有生命週期管理的執行期行為。

## Runtime Value Family

重要 value 包含：

```text
None
Bool
Int / Num
Text
Bytes
List / Map / Set
Function / Callable
Object
Module
Error
File / Path
Duration / Time
NativeHandle
Task
```

## Managed Ownership

Collection、Object、Function、File、Native Handle 等由 Runtime 管理生命週期。一般 SE code 不直接操作 raw pointer、`new/delete`、`malloc/free` 或 C++ ownership primitive。

## Checked Operations

Runtime operation 會在執行危險的 host-language operation 前先檢查輸入，例如：

- List index bounds
- 無效 member access
- function arity 錯誤
- condition/value type 不合法
- division 與無效 arithmetic case
- file/native failure

Runtime 能偵測的 failure 應轉成 SE error，而不是 C++ undefined behavior。

## Function 與 Closure

Function 是 Runtime value。巢狀 `make` 可以保存 lexical environment，因此能形成 closure；Runtime 也可提供 bind / partial application 等 function helper。

## Object

User-defined object 擁有獨立 field storage。Method 可以解析 current-object field，但不會把 raw object layout 暴露給一般 SE source。

## Error Value

Recoverable failure 會保存 message、kind 與可用時的 source location。`try / else err` 處理這些 runtime failure；compiler/checker error 則保持獨立。

## Resource Lifetime

File 與 native resource 使用 managed lifetime。Opaque native handle 可以附帶 cleanup function，讓 managed value 被釋放時自動 cleanup。

## Task 與 Concurrency Boundary

Task-style async/await 由 Runtime 管理。需要進入 Interpreter VM 的 callback 會依 implementation 做序列化，因此不應把目前模型描述成任意 SE code 的 unrestricted parallel execution。

## Platform Modules

JSON、process、HTTP/HTTPS、DB adapter 等平台功能主要透過 Runtime module 提供，而不是每一種 subsystem 都增加新的特殊語法。

## Browser Runtime

SE Web 會產生 browser-side Runtime，用於 HTTP、navigation、request cancellation 與 DOM helper。Browser 是不同 execution environment：Server filesystem、native、database secret 等能力不會自動變成 browser API。

## 設計原則

> 當複雜度放進 Runtime 能讓一般 SE code 更安全、更簡單時，就應該把複雜度留在 Runtime。

Runtime 內部可以複雜，但一般程式不應被迫自己管理 memory、ABI detail 或 host-language boilerplate。
