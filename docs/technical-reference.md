# SE Technical Reference

[繁體中文版](technical-reference-zh-TW.md)

This document describes the implementation model of SE: compiler stages, runtime boundaries, native interoperability, Web compilation and testing. It separates implementation facts from roadmap ideas.

## 1. Compiler pipeline

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

`se check` stops after semantic checking. `se run` uses the interpreter. `se build` generates C++20 and invokes a system compiler. `se web build` uses the Web compiler path.

## 2. Lexer and indentation

Blocks are represented by `INDENT` and `DEDENT` tokens. The parser therefore receives explicit block boundaries instead of guessing indentation later.

```se
if ready
    say "yes"
say "done"
```

## 3. Parser and AST

Expressions use a Pratt parser for precedence and associativity. Statements cover assignment, control flow, functions, types, modules, errors and other language constructs.

Checker, interpreter and code-generation paths share the AST so syntax is not independently re-parsed by each backend.

## 4. Static checking and inference

```se
age = 15
name = "SE"
ready = true
```

SE avoids redundant type declarations where the type is clear, but inferred values are still checked for incompatible operations, invalid calls, unknown names and collection/type errors.

> Less type syntax does not mean no type system.

## 5. Runtime values and safety

Important value families include:

```text
None Int Num Bool Text Bytes
List Map Set
Function Object Error
Duration Path File
managed native handles
```

Ordinary SE programs do not directly manage raw pointers, `new/delete`, `malloc/free`, C++ templates or arbitrary C++ object ABI details.

Runtime operations should turn invalid indexing, arity errors, invalid members, I/O failures and native failures into SE-level errors instead of C++ undefined behavior.

## 6. Functions, closures and types

`make` represents functions and methods. Lexical environments support closures. User-defined types contain fields and methods, and method field lookup can resolve the current object's fields without forcing a `self.` prefix everywhere.

Generic functions are supported. Versioned generic user-type work is documented separately because it may evolve independently from the stable release.

## 7. Modules

`use module` loads source or standard/native modules through the module loader. New source prefers `.se`; legacy `.s` may remain for migration compatibility. Circular dependency chains are rejected. Top-level `_name` follows the private convention.

## 8. Errors

SE distinguishes compiler/checker errors from recoverable runtime errors. Runtime failures can be handled with `try ... else err`, created with `fail`, and propagated with `try expr` where supported.

## 9. Collections and data

List operations are bounds checked. Map and Set provide managed collection semantics. Text and binary-safe Bytes are separate value types. JSON maps JSON data into SE collection/scalar values.

## 10. Tasks and async

Managed Task-style async/await exists in the runtime. Interpreter callbacks entering the VM are serialized; the model is not advertised as unrestricted parallel execution of arbitrary SE code.

SE Web event handlers can lower supported `async.await` expressions to browser JavaScript `await`.

## 11. HTTP and Web server

SE provides HTTP client functionality and HTTPS through the system `curl` transport. The built-in HTTP server/router is synchronous/blocking and is intended for learning, development and small services rather than being described as a hardened high-concurrency production server.

## 12. SE Web compiler

Component Web source reuses `make` with optional `html`, `css`, and `js` sections plus `page` declarations. The compiler emits standard Web files and injects the browser runtime used for requests, routing and DOM helpers.

Browser code and server code have different capability boundaries. Filesystem, native FFI, database secrets and server listeners do not automatically become browser APIs.

## 13. Native C ABI bridge

Recommended boundary:

```text
C/C++ library
    ↓
C ABI wrapper
    ↓
.snative metadata
    ↓
SE
```

The bridge supports common scalar values, Text, Bytes and managed opaque handles. Native resources can define cleanup functions so resource lifetime remains managed instead of exposing raw addresses to ordinary SE code.

## 14. Binding generator

```bash
se bind module.sbind generated
```

The binding path generates metadata/header/wrapper output for supported signatures and should reject unsupported signatures rather than silently generating incomplete bindings.

## 15. Build and warnings

SE itself uses CMake and C++20. GCC/Clang builds use strict warnings such as `-Wall -Wextra -Wpedantic -Werror`; MSVC uses corresponding high-warning/error settings.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## 16. Testing strategy

Regression coverage should include lexer/parser/checker behavior, interpreter execution, native build parity, collections, types, modules, errors, native ABI/Bytes ownership, resource cleanup, binding generation, Web compilation and CLI smoke tests.

Interpreter/native parity is important because the same SE source should not silently change meaning when moving from `se run` to `se build`.

## 17. Naming compatibility

SE is the successor to the earlier S codebase. Internal C++ namespaces, header paths, target names or legacy `.s` examples may still contain `s` for implementation compatibility. User-facing naming is **SE**, CLI `se`, source `.se`.

## 18. Design rule

> Simple at every level.

Complexity should preferentially live in the compiler, runtime, standard library and native bridge rather than forcing ordinary programs to repeatedly manage punctuation, ownership, ABI details or boilerplate.
