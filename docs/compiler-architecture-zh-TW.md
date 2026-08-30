# SE Compiler 架構

[English version](compiler-architecture.md)

SE 把 syntax、semantic checking、execution 與 code generation 分層，讓各部分可以獨立演進，同時避免重複定義語言語意。

```text
SE source
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

## Lexer

Lexer 會 tokenize `.se` source，並直接負責 indentation。Block 會變成明確的 `INDENT` / `DEDENT` token，因此 Parser 不需要之後再猜 whitespace structure。

Lexer 也處理 identifier、keyword、string、number、duration literal、operator、comment 與 bracket-aware newline。

## Pratt Parser

Expression 使用 Pratt parsing 處理 precedence 與 associativity。Statement parser 負責 assignment、control flow、function、type、module、error、match/case、Web section 等語法。

## AST

Parser 建立共享 AST。Checker、Interpreter 與 code-generation path 都以同一份 AST 為輸入，不會各自重新解析 source。

AST 主要包含 literal/expression、call、assignment、control flow、function、user type、module、error、match、Web section/page 等節點。

## Static Checker

Checker 管理 lexical scope，並檢查執行前可知的資訊，例如未知名稱、不相容 operation、call shape、member/type relationship 與目前支援的 generic type relationship。

Type inference 用來減少 annotation，但不移除 static validation。

## Interpreter

`se run file.se` 透過 Interpreter + Runtime 執行已檢查程式。Runtime value 包含 scalar、Text/Bytes、collection、function/closure、object、error、file/path/duration 與 managed native resource。

## C++20 Backend

`se build file.se` 會產生 C++20，再呼叫系統 compiler 建立 native executable。Native path 應沿用同一套已檢查 SE semantics，而不是變成另一門語言。

這也讓 generated code 相對容易檢查，並保留未來增加其他 backend 的空間。

## Component Web Compiler

`se web build app.se dist` 會辨識 SE Web component/page source，並產生標準 browser asset，例如 HTML、CSS、JavaScript/TypeScript。

Web compiler 有 browser-specific lowering/validation，但會盡量重用 SE AST、expression 與 control-flow concept。

## Module Loading

Module loader 在 execution/code generation 前建立 dependency graph。Source module、standard/runtime module 與 native metadata 都共用 `use` model；circular dependency 會被拒絕。

## Native Boundary

C/C++ interop 透過 C ABI metadata 與 managed runtime value 隔離，使任意 C++ ABI detail 不需要進入一般 Parser / Type System。

## Testing Architecture

Regression coverage 應包含 Lexer/Parser/Checker、Interpreter、Native backend parity、Runtime modules、Native ABI/Bytes/resource cleanup、Web compilation 與 CLI workflow。

整體目標是：**一套語言模型、多個 execution target、盡量少的重複語意。**
