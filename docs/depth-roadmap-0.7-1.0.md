# SE Depth Roadmap: 0.7 → 1.0

[繁體中文版](depth-roadmap-0.7-1.0-zh-TW.md)

This is a design roadmap, not a stable-release feature list. Its purpose is to describe how SE can become deeper without turning ordinary source into a punctuation-heavy systems language.

## Design rules

Advanced work should follow these rules:

1. Ordinary programs should not need to see advanced mechanisms unless they use them.
2. The compiler/runtime should absorb type inference, resource handling and boilerplate where practical.
3. Common tasks should have one primary model rather than many overlapping syntaxes.
4. Low punctuation remains the default.
5. Simpler syntax must not mean weaker static checking.
6. A language feature is not complete until parser/AST/checker/execution or codegen, diagnostics, tests and documentation agree.

## 0.7 — Type depth

Core directions:

- structured/nested TypeRef representations
- generic collection annotations and substitution
- generic user types
- choice/sum types
- destructuring and exhaustive match
- typed Option/Result
- stronger function/higher-order collection typing

Example direction:

```se
make total nums:List[Int] -> Int
    ...

type Box[T]
    value:T
```

These features should preserve old unannotated and generic-function source compatibility.

## 0.8 — Safe concurrency

The managed Task model can evolve toward a clearer scheduler, cancellation/timeout and safe parallel work without making manual thread/mutex lifecycle the default user model.

Possible high-level APIs include channel-like communication and `parallel.map`-style work when the runtime can provide clear safety semantics.

## 0.9 — Production platform depth

Platform directions include:

- unified HTTP/HTTPS behavior
- richer request/response and streaming support
- relational database adapters with safe parameter binding
- package/manifest/lockfile tooling
- deeper test framework features
- production-oriented server/runtime work

## 1.0 — Language contract

A 1.0 milestone should mean the language contract is stable, not merely that many modules exist. Areas that need clear compatibility rules include:

- lexical and indentation grammar
- core expressions/statements
- type inference/generic semantics
- errors and propagation
- module visibility
- object/choice/collection semantics
- async/task behavior
- FFI ABI versioning
- backward-compatibility policy

## Native build decision

Before 1.0, the deployment story for `se build` should be explicit: keep a C++ backend with an appropriate toolchain story, add another native backend, or support multiple backends. The requirement should be documented rather than hidden.

## Explicit non-goals

SE should not add features only because another language has them. Avoid uncontrolled textual preprocessors/macros, arbitrary parser-rewriting packages, excessive operator-overload systems, overlapping iteration syntaxes or user-facing ownership punctuation without a strong reason.

## Implementation order

A reasonable dependency order is:

```text
Structured TypeRef
→ generic collection typing
→ generic user types
→ choice + deeper match
→ typed Option/Result integration
→ concurrency depth
→ production platform depth
```

Do not destabilize the type system and VM scheduler simultaneously when the work can be staged.

## Final measure

> **Simple at every level** means advanced programs remain readable because the compiler/runtime carry the low-level complexity, not because advanced capabilities are omitted.
