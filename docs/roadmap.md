# SE Roadmap

[繁體中文版](roadmap-zh-TW.md)

This roadmap describes direction, not guaranteed release contents. Stable release behavior is documented in the main guides; versioned design documents describe specific development stages.

## Established foundation

SE already has a C++20 implementation with an indentation-aware lexer, Pratt parser, AST, static checker/type inference, interpreter, native C++ backend, runtime values, modules, user types, collections, recoverable errors, testing, file/path/time utilities, JSON/HTTP capabilities, native C ABI interoperability, and Web compiler work.

## Type system

Planned or evolving areas include:

- more precise generic collection propagation
- generic user types where the implementation/revision supports them
- constraints/protocol/trait-like capabilities only if they can preserve SE's simple syntax
- safer narrowing of broad/unknown values

## Pattern matching

Potential future depth:

- destructuring patterns
- guards
- more natural Option/Result matching
- exhaustiveness analysis

Current stable matching should not be described as having these features unless the implementation actually includes them.

## Async and concurrency

Directions include:

- stronger scheduling/runtime structure
- non-blocking I/O integrations
- a safe concurrency model
- keeping direct thread/lock management out of ordinary SE source where possible

## Web and networking

SE Web is already an active compiler target. Further work can include:

- deeper multi-file component/project integration
- more browser platform APIs
- deployment adapters/tooling
- WebSocket or streaming/event capabilities
- stronger production server architecture

Browser and server capability boundaries should remain explicit.

## Data and database

Future database depth may include richer SQL/database adapters, transactions, prepared queries and resource/connection management while keeping the simple module/API model.

## Tooling

Potential tooling work:

- LSP
- completion / hover / diagnostics
- formatter
- debugger integrations
- package manager / registry
- distribution improvements

## Compiler and build system

Potential areas:

- native optimization improvements
- caching / incremental build
- relocatable installation improvements
- optional additional code-generation backends when they justify their complexity

An LLVM backend is an option, not a requirement for SE's identity.

## Long-term rule

> **Simple at every level.**

SE should grow by moving complexity into the compiler, runtime, standard library and tooling rather than forcing ordinary programs to absorb more punctuation, manual ownership or platform boilerplate.
