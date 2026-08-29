# Compiler Architecture

```text
source → lexer → tokens + INDENT/DEDENT → parser → AST → checker
                                                       ├─ interpreter
                                                       └─ C++ backend → native executable
```

The lexer owns indentation rules rather than hiding them in the parser. The parser creates typed AST classes and uses precedence parsing for expressions. The checker tracks lexical scopes, rejects unknown names and checks all types that are knowable before execution.

The interpreter and backend consume the same AST. `s build` emits standalone C++20 with a compact generated runtime. This makes native builds easy to inspect and keeps LLVM optional for a later backend.

The directory boundaries are intentional: syntax, semantics, execution, and code generation can evolve independently.
