# Compiler 架構

本文件是 `compiler-architecture.md` 的繁體中文版，說明 SE 從原始碼到執行結果的主要流程。

```text
SE source
   ↓
Lexer
   ↓
Token stream
   ↓
Parser
   ↓
AST
   ↓
Static Checker / Type Inference
   ↓
Interpreter 或 C++ Backend
   ↓
Runtime / Native executable
```

## Lexer

Lexer 將 `.se` 原始碼切成 Token。SE 的縮排是語法的一部分，因此 Lexer 會產生 `INDENT` / `DEDENT`，並處理換行、字串、數字、運算子與關鍵字。

## Parser

Parser 使用 Pratt parsing 處理運算式優先順序，並建立 AST。區塊結構主要由 INDENT / DEDENT 決定，而不是 `{}`。

## AST

AST 是語言的中間結構，包含變數、函式、控制流程、type、module、call、collection、match 等節點。Interpreter、Checker 與 C++ Backend 都以 AST 為主要輸入。

## Static Checker

Checker 在執行前檢查：

- 變數與符號是否存在
- 呼叫參數是否合理
- 型別是否相容
- member 是否存在
- 函式回傳與泛型函式型別關係
- 部分 fallible API 是否需要錯誤處理

SE 會盡可能推斷型別，讓使用者少寫型別標註。

## Interpreter

`se run file.se` 透過 Interpreter 執行 AST。Runtime 提供 Value、List/Map/Set、Object、File、Bytes、Module、NativeHandle 等執行期資料。

## C++ Backend

`se build file.se` 會把 SE AST 產生為 C++20，再呼叫系統 C++ compiler 建立 native executable。這讓 SE 能保留簡單語法，同時擁有原生編譯路徑。

## Native / C ABI

SE 另外有 `.snative`、C ABI 與 binding generator，可讓 Runtime 呼叫 C/C++ 世界的函式。Bytes 與 managed native handle 都有明確 ownership 邊界。

## 設計原則

SE 的重要原則是：**把複雜度放進 Compiler 與 Runtime，而不是放進使用者語法。** 因此 parser、checker 和 runtime 可以比較複雜，但 SE 程式本身仍應保持低標點與一致性。
