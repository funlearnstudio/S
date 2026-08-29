# S

> Simple at every level.

S is a low-punctuation, safety-first programming language implemented in C++20. Version 0.1 contains a real indentation-aware lexer, parser and AST, static checker, interpreter, REPL, and a C++ native-code backend.

```s
name = ask "What is your name"
say "Hello " + name

repeat 3
    say "Welcome to S"
```

## Build S

You need CMake 3.20+ and a C++20 compiler.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

The CLI is at `build/s`:

```bash
./build/s run examples/hello.s
./build/s check examples/function.s
./build/s build examples/hello.s
./examples/hello
./build/s
```

`s build` translates the checked AST into standalone C++20 and invokes the system C++ compiler. The resulting program does not need the S interpreter or S source file.

## S 0.1

- `Int`, `Num`, `Text`, `Bool`, `List`, ranges and strict operations
- `say`, `ask`, variables, safe indexing and text/list members
- `if` / `else`, `repeat`, `for in`, `while`
- `make`, low-punctuation calls and `give`
- readable source diagnostics, REPL, `run`, `check`, and native `build`

See [Getting Started](docs/getting-started.md), the [Language Guide](docs/language-guide.md), and the [Roadmap](docs/roadmap.md).

## Status

S 0.1 is a usable bootstrap implementation, not the end of the language. The 0.2 object/module/file design and the 0.3 concurrency/package work are intentionally staged so syntax can be tested before it is made permanent.

## License

MIT
