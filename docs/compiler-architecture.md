# SE Compiler Architecture

[繁體中文版](compiler-architecture-zh-TW.md)

SE keeps syntax, semantic checking, execution and code generation in separate layers so each part can evolve without duplicating language semantics.

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

The lexer tokenizes `.se` source and owns indentation processing. Blocks become explicit `INDENT` / `DEDENT` tokens, so the parser does not need to guess whitespace structure later.

The lexer also handles identifiers, keywords, strings, numbers, duration literals, operators, comments and bracket-aware newlines.

## Pratt parser

Expressions use Pratt parsing for operator precedence and associativity. Statement parsing handles assignment, control flow, functions, types, modules, errors, match/case, Web sections and other language constructs.

## AST

The parser creates a shared abstract syntax tree. Checker, interpreter and code-generation paths all consume this AST instead of independently re-parsing source.

Important AST families cover literals/expressions, calls, assignments, control flow, functions, user types, modules, errors, match, Web sections/pages and related constructs.

## Static checker

The checker tracks lexical scopes and verifies information that is knowable before execution, including unknown names, incompatible operations, call shapes, member/type relationships and generic type relationships supported by the current language.

Type inference reduces annotation noise while preserving static validation.

## Interpreter

`se run file.se` evaluates the checked program through the interpreter and runtime. Runtime values include scalars, Text/Bytes, collections, functions/closures, objects, errors, files/paths/durations and managed native resources.

## C++20 backend

`se build file.se` generates C++20 and invokes a system compiler to produce a native executable. The generated path reuses checked SE semantics rather than defining a separate language.

This backend keeps generated code inspectable and leaves room for additional future backends without requiring LLVM as the only path.

## Component Web compiler

`se web build app.se dist` recognizes SE Web component/page source and generates standard browser assets such as HTML, CSS and JavaScript/TypeScript output.

The Web compiler includes browser-specific lowering and validation while reusing SE AST/expression/control-flow concepts where supported.

## Module loading

Module resolution builds the dependency graph before execution/code generation. Source modules, standard/runtime modules and native metadata share the `use` model. Circular dependencies are rejected.

## Native boundary

Native C/C++ interoperability is isolated behind C ABI metadata and managed runtime values. This keeps arbitrary C++ ABI details out of ordinary parser/type-system rules.

## Testing architecture

Regression coverage is expected across lexer/parser/checker, interpreter, native backend parity, runtime modules, native ABI/Bytes/resource cleanup, Web compilation and CLI workflows.

The architectural goal is simple: **one language model, multiple execution targets, minimal duplicated semantics.**
