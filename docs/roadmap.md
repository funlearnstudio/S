# Roadmap

## 0.1 — core language

Implemented: indentation lexer, Pratt parser/AST, inferred primitive types, safe List indexing, control flow, functions, interpreter, REPL, diagnostics, static checking, and S -> C++20 -> native builds.

## 0.2 — core expansion

Implemented in the 0.2 line:

- user-defined `type`, inferred fields, methods, implicit current-object field access, and simple object initialization
- multi-file modules with `use`, project/std/package resolution, private `_name` convention, graph checking, and circular-import errors
- typed List, Map, and Set operations and iteration
- recoverable runtime Error values with `try`, `try expr`, and `fail`
- file, path, time, and Duration support using C++20 resource/path/time facilities
- C ABI native modules with scalar bindings, opaque managed handles, fallible native calls, and C++ interoperability through small C wrappers
- interpreter/native parity tests on macOS and Linux CI

## 0.3 — connected programs

Candidates after the 0.2 semantics settle: tasks and structured concurrency, networking, package metadata/registry, richer inferred generic collection APIs, a binding generator for more C ABI shapes, and an LSP.

## Later

Evaluate direct lower-level/LLVM code generation only when measurement justifies the added compiler complexity. Keep arbitrary C++ ABI details, raw pointers, templates, and manual ownership outside normal S source.
