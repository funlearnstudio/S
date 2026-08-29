# SE

> Simple at every level.

SE is a low-punctuation, safety-first programming language implemented in C++20. The goal is to keep code simple from beginner programs through types, modules, testing, JSON, web development, native interoperability and native compilation.

SE 0.5 development uses:

```text
Language: SE
CLI:      se
Source:   .se
```

```se
type Player
    name = ""
    hp = 100

    make hit damage
        hp = hp - damage

player = Player
    name = "Steve"

player.hit 20
say player.name
say player.hp
```

## Build SE

Requirements: CMake 3.20+ and a C++20 compiler.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Install on macOS/Linux:

```bash
cmake --install build --prefix "$HOME/.local"
export PATH="$HOME/.local/bin:$PATH"
se --version
```

Current development builds report the SE 0.5 development version.

## CLI

```bash
se
se --version
se help
se doctor
se check hello.se
se check-all .
se run hello.se
se test .
se build hello.se
se new app myapp
se new web mysite
se bind module.sbind generated
```

`se check` loads and statically checks one program and its module graph. `se check-all` checks a source tree. `se run` executes through the interpreter. `se test` runs `*_test.se` files. `se build` emits C++20 and invokes the system C++ compiler to create a native executable.

## Current language features

- indentation-based blocks with `INDENT` / `DEDENT`
- Pratt expression parser and AST
- static checking and type inference
- `say`, `ask`, variables, arithmetic, comparison and logic
- `if`, `else`, `repeat`, `for`, `while`
- `make`, low-punctuation calls and `give`
- List, Map and Set with checked operations
- `.len`, `.upper`, `.lower`
- user-defined `type`, fields and methods
- modules with `use`, private `_name` convention and circular-import detection
- recoverable `try`, `try expr`, `fail` and Error values
- file I/O, path utilities, time/duration, math, random and OS helpers
- binary-safe `Bytes`
- JSON parsing/stringification and text/collection utilities
- test assertions and project-level `se test`
- process execution/output bridge
- HTTP client
- synchronous HTTP server and web router
- request/response helpers and JSON APIs
- JavaScript bridge through Node.js
- TypeScript bridge through ts-node / tsc
- `se new app` and `se new web` project scaffolding
- C ABI native modules for scalar/Text/Bytes values and managed native handles
- binding generation through `se bind`
- interpreter and native C++ backend
- REPL, check, run, test and native build workflows

## Documentation

Start here:

- [Getting Started](docs/getting-started.md)
- [SE 完整語言教學（繁體中文，0.5）](docs/complete-language-guide-zh-TW.md)
- [SE 教學（繁體中文，較短版）](docs/tutorial-zh-TW.md)
- [SE 技術文件（繁體中文）](docs/technical-reference-zh-TW.md)
- [Language Reference](docs/language-reference.md)
- [Syntax Reference](docs/syntax-reference.md)
- [Types and Methods](docs/types.md)
- [Modules](docs/modules.md)
- [Collections](docs/collections.md)
- [Errors](docs/errors.md)
- [Files](docs/files.md)
- [Paths](docs/paths.md)
- [Time](docs/time.md)
- [Native Interoperability](docs/native-interop.md)
- [Compiler Architecture](docs/compiler-architecture.md)
- [Runtime Design](docs/runtime-design.md)
- [Roadmap](docs/roadmap.md)

## Architecture

```text
SE source
    ↓
Lexer → tokens + INDENT/DEDENT
    ↓
Pratt Parser
    ↓
AST
    ↓
Static Checker
    ├── Interpreter
    └── C++20 Backend → system C++ compiler → native executable
```

The compiler/runtime intentionally keeps raw pointers, manual memory management, C++ templates and arbitrary C++ ABI details out of ordinary SE programs.

## Current platform boundaries

The built-in HTTP client currently supports plain `http://`, not TLS/HTTPS. The built-in HTTP server is synchronous/blocking and is intended for development, learning and small services rather than being presented as a hardened high-concurrency production server. JavaScript and TypeScript bridges depend on their external runtimes/tools being installed.

## Migration note

SE is the successor name to the earlier S codebase. Internal C++ namespaces/header paths may still use `s` for implementation compatibility, and legacy `.s` source compatibility may temporarily remain. New user-facing code and documentation should use `SE`, `se` and `.se`.

## License

MIT
