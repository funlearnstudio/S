# SE 深度能力 Roadmap：0.7 → 1.0

[English version](depth-roadmap-0.7-1.0.md)

這是一份設計 Roadmap，不是 stable Release feature list。目標是描述 SE 如何增加語言深度，同時避免讓一般 source 變成 punctuation-heavy systems language。

## 設計規則

進階能力應遵守：

1. 一般程式沒有使用進階機制時，不需要先理解它。
2. 能由 Compiler / Runtime 承擔的 type inference、resource handling、boilerplate 應優先留在底層。
3. 常見工作盡量維持一種主要模型，避免互相重疊的 syntax。
4. Low punctuation 仍是預設。
5. 語法簡單不能等於 static checking 變弱。
6. Language feature 必須讓 Parser / AST / Checker / execution 或 codegen / diagnostics / tests / docs 一致，才算真正完成。

## 0.7 — Type Depth

主要方向：

- structured / nested TypeRef
- generic collection annotation 與 substitution
- generic user type
- choice / sum type
- destructuring 與 exhaustive match
- typed Option / Result
- 更精確的 function / higher-order collection typing

例如：

```se
make total nums:List[Int] -> Int
    ...

type Box[T]
    value:T
```

擴充時仍應保持舊 untyped function 與既有 generic-function source compatibility。

## 0.8 — Safe Concurrency

Managed Task model 可以繼續發展 scheduler、cancellation / timeout 與安全 parallel work，但不應把 manual thread / mutex lifecycle 變成一般使用者預設模型。

Channel-like communication、`parallel.map` 類 high-level API 只有在 Runtime 能提供清楚 safety semantics 時才適合加入。

## 0.9 — Production Platform Depth

Platform 深化方向包含：

- 統一 HTTP / HTTPS behavior
- richer request/response、streaming
- relational database adapter 與安全 parameter binding
- package / manifest / lockfile tooling
- 更深入 test framework
- production-oriented server/runtime

## 1.0 — Language Contract

1.0 應代表語言契約穩定，而不是只有 module 數量很多。需要明確 compatibility rule 的範圍包含：

- lexical / indentation grammar
- core expression / statement
- type inference / generic semantics
- error / propagation
- module visibility
- object / choice / collection semantics
- async / Task behavior
- FFI ABI versioning
- backward compatibility policy

## Native Build 決策

1.0 前，`se build` 的部署模型要明確：繼續使用 C++ backend 並提供清楚 toolchain story、增加其他 native backend，或正式支援多 backend。不能只把 dependency 隱藏起來。

## 明確 Non-goals

不要只因為其他語言有某功能就直接加入 SE。應避免無限制 textual preprocessor/macro、package 任意改寫 Parser grammar、大量 operator overload system、重疊 iteration syntax，或沒有充分理由的 user-facing ownership punctuation。

## 建議實作順序

```text
Structured TypeRef
→ generic collection typing
→ generic user types
→ choice + deeper match
→ typed Option/Result integration
→ concurrency depth
→ production platform depth
```

能分階段時，不應同時大改 Type System 與 VM scheduler。

## 最終衡量標準

> **Simple at every level** 的意思不是拿掉 advanced capability，而是讓 advanced program 仍可讀，並由 Compiler / Runtime 承擔底層複雜度。
