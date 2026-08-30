# SE

> Simple at every level.

SE is a low-punctuation, safety-first programming language implemented in C++20. The goal is to keep code simple from beginner programs through types, modules, testing, JSON, web development, native interoperability and native compilation.

SE 是一門以 C++20 實作、低標點、重視安全性的程式語言。目標是從初學程式一路到型別、模組、測試、JSON、Web、原生互通與 Native Build，都保持簡單。

Current release / 目前版本：

```text
Language: SE
CLI:      se
Source:   .se
Version:  SE 0.6.0
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

## Installation / 安裝

Full standalone guide / 完整獨立安裝說明：**[docs/installation.md](docs/installation.md)**

### Quick install — no CMake or C++ compiler needed

macOS / Linux:

```bash
curl -fsSL https://raw.githubusercontent.com/funlearnstudio/SE/main/install.sh | sh
```

Windows PowerShell:

```powershell
irm https://raw.githubusercontent.com/funlearnstudio/SE/main/install.ps1 | iex
```

Then verify / 安裝完成後檢查：

```bash
se --version
se doctor
```

The installer downloads the correct prebuilt SE package from GitHub Releases. You do **not** need Git, CMake, or a C++ compiler to use the REPL, `se run`, `se check`, `se check-all`, or `se test`.

安裝器會直接從 GitHub Releases 下載已經編譯完成的 SE。一般使用 REPL、`se run`、`se check`、`se check-all`、`se test` 時，**不需要另外安裝 Git、CMake 或 C++ 編譯器**。

`se build` is the one exception: SE's current native backend emits C++20 and invokes a system C++ compiler, so producing a standalone native executable still requires a C++20 compiler. CMake is not required for normal installed usage.

唯一例外是 `se build`：目前 SE 的 Native Backend 仍會產生 C++20 並呼叫系統 C++ 編譯器，因此如果要把 SE 程式編譯成獨立 Native Executable，仍需要 C++20 編譯器。一般執行 SE 不需要。

### Direct downloads / 直接下載

GitHub Releases publishes these prebuilt archives:

- `se-0.6.0-macos-arm64.tar.gz` — Apple Silicon Mac
- `se-0.6.0-macos-x64.tar.gz` — Intel Mac
- `se-0.6.0-linux-x64.tar.gz` — Linux x64
- `se-0.6.0-windows-x64.zip` — Windows x64
- `SHA256SUMS.txt` — release checksums

### Build SE from source / 從原始碼編譯 SE

Only contributors who want to build SE itself need CMake, Git and a C++20 compiler:

```bash
git clone https://github.com/funlearnstudio/SE.git
cd SE
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build --prefix "$HOME/.local"
```

### First program / 第一個程式

Create `hello.se`:

```se
say "Hello SE"
```

Run it:

```bash
se run hello.se
```

Check it without running:

```bash
se check hello.se
```

If a C++20 compiler is installed, build a native executable:

```bash
se build hello.se
```

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
- collection `filter`, `map`, `reduce`, `slice`, `take`, `drop` and sorting helpers
- `.len`, `.upper`, `.lower`
- user-defined `type`, fields and methods
- modules with `use`, private `_name` convention and circular-import detection
- recoverable `try`, `try expr`, `fail` and Error values
- Option and Result helpers
- lexical closures and function helpers
- generic functions
- value-based `match` / `case`
- managed Task-style async / await
- file I/O, path utilities, time/duration, math, random and OS helpers
- lightweight persistent Text key/value database API
- binary-safe `Bytes`
- JSON parsing/stringification and text/collection utilities
- test assertions and project-level `se test`
- process execution/output bridge
- HTTP client
- HTTPS client through system `curl`
- synchronous HTTP server and web router
- request/response helpers and JSON APIs
- JavaScript bridge through Node.js
- TypeScript bridge through ts-node / tsc
- `se new app` and `se new web` project scaffolding
- C ABI native modules for scalar/Text/Bytes values and managed native handles
- binding generation through `se bind`
- interpreter and native C++ backend
- REPL, check, run, test and native build workflows
- core conversion aliases: `int`, `integer`, `num`, `double`, `float`, `text`, `string`, `bool`, `boolean`, `char`

## Documentation

Start here:

- [Installation / 安裝](docs/installation.md)
- [繁體中文文件總覽](docs/README-zh-TW.md)
- [Getting Started](docs/getting-started.md)
- [SE 完整語言教學（繁體中文）](docs/complete-language-guide-zh-TW.md)
- [SE 教學（繁體中文，較短版）](docs/tutorial-zh-TW.md)
- [SE 技術文件（繁體中文）](docs/technical-reference-zh-TW.md)
- [SE 0.6 進階功能（繁體中文）](docs/advanced-0.6-zh-TW.md)
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

SE 0.6 currently provides generic functions rather than a complete generic user-type/trait system. Pattern matching is value/equality based rather than full destructuring with guards. The built-in database is a lightweight persistent Text key/value store rather than SQL. HTTPS delegates TLS transport to the system `curl` command. The built-in HTTP server is synchronous/blocking and is intended for development, learning and small services rather than being presented as a hardened high-concurrency production server. Managed async tasks serialize callbacks entering the interpreter VM and are not advertised as unrestricted parallel SE execution. JavaScript and TypeScript bridges depend on their external runtimes/tools being installed.

## Migration note

SE is the successor name to the earlier S codebase. Internal C++ namespaces/header paths may still use `s` for implementation compatibility, and legacy `.s` source compatibility may temporarily remain. New user-facing code and documentation should use `SE`, `se` and `.se`.

## License

MIT
