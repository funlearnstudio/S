# SE

> Simple at every level.

SE is a low-punctuation, safety-first programming language implemented in C++20. The goal is to keep code simple from beginner programs through types, modules, native interoperability and native compilation.

SE 0.3 uses:

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

Expected output:

```text
SE 0.3.0
```

## CLI

```bash
se
se --version
se check hello.se
se run hello.se
se build hello.se
se bind module.sbind generated
```

`se check` loads and statically checks the program and its module graph. `se run` executes through the interpreter. `se build` emits C++20 and invokes the system C++ compiler to create a native executable.

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
- file I/O, path utilities and time/duration values
- binary-safe `Bytes`
- C ABI native modules for scalar/Text/Bytes values and managed native handles
- binding generation through `se bind`
- interpreter and native C++ backend
- REPL, `check`, `run` and native `build`

## Documentation

Start here:

- [Getting Started](docs/getting-started.md)
- [SE 教學（繁體中文）](docs/tutorial-zh-TW.md)
- [SE 0.3 技術文件（繁體中文）](docs/technical-reference-zh-TW.md)
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

## Migration note

SE is the successor name to the earlier S codebase. Internal C++ namespaces/header paths may still use `s` for implementation compatibility, and legacy `.s` source compatibility may temporarily remain. New user-facing code and documentation should use `SE`, `se` and `.se`.

## License

MIT
