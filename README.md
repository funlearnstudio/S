# SE

> Simple at every level.

SE is a low-punctuation, safety-first programming language implemented in C++20. The goal is to keep code simple from beginner programs through types, modules, testing, JSON, web development, native interoperability and native compilation.

SE 是一門以 C++20 實作、低標點、重視安全性的程式語言。目標是從初學程式一路到型別、模組、測試、JSON、Web、原生互通與 Native Build，都保持簡單。

Current development line / 目前開發版本：

```text
Language: SE
CLI:      se
Source:   .se
Version:  SE 0.6.0-dev
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

### English

Requirements:

- CMake 3.20+
- a C++20 compiler
- Git
- `curl` if you want to use the SE HTTPS module

Clone and build:

```bash
git clone https://github.com/funlearnstudio/SE.git
cd SE
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

To keep `se` available after restarting your terminal, add this to your shell profile such as `~/.zshrc` or `~/.bashrc`:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

Then reload the shell configuration:

```bash
source ~/.zshrc
```

On Windows, build with a C++20-capable Visual Studio/MSVC environment:

```powershell
git clone https://github.com/funlearnstudio/SE.git
cd SE
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
cmake --install build --config Release --prefix "$env:USERPROFILE\.local"
```

Add the installed `bin` directory to `PATH`, then verify:

```powershell
se --version
se doctor
```

### 中文

需求：

- CMake 3.20 以上
- 支援 C++20 的編譯器
- Git
- 如果要使用 SE 的 HTTPS 模組，需要系統有 `curl`

下載並編譯：

```bash
git clone https://github.com/funlearnstudio/SE.git
cd SE
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

macOS / Linux 安裝：

```bash
cmake --install build --prefix "$HOME/.local"
export PATH="$HOME/.local/bin:$PATH"
se --version
```

如果希望重新開啟 Terminal 後仍然可以直接使用 `se`，把下面這行加入 `~/.zshrc` 或 `~/.bashrc`：

```bash
export PATH="$HOME/.local/bin:$PATH"
```

例如使用 zsh：

```bash
source ~/.zshrc
```

Windows 可以在支援 C++20 的 Visual Studio / MSVC 環境中執行：

```powershell
git clone https://github.com/funlearnstudio/SE.git
cd SE
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
cmake --install build --config Release --prefix "$env:USERPROFILE\.local"
```

接著把安裝位置的 `bin` 資料夾加入 `PATH`，再檢查：

```powershell
se --version
se doctor
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

Build a native executable:

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
