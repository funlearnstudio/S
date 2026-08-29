# Roadmap

## 0.1 — core language

Implemented: indentation lexer, AST parser, strict inferred primitives, safe lists, control flow, functions, interpreter, REPL, diagnostics, static checking, and native C++ builds.

## 0.2 — everyday programs

Design and implement `type`, objects, `use`, automatically closed files, `open` / `read` / `write`, `wait`, and optional media `play`. Syntax will be accepted only after the simplicity tests in the language specification.

## 0.3 — connected programs

Add tasks, structured concurrency with `all`, network access, inferred generics, packages, C ABI FFI, and an LSP.

## Later

Evaluate LLVM only when measurements show the C++ backend is a bottleneck. Add a debugger and stable package registry after the language and module rules settle.
