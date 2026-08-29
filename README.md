# S

> Simple at every level.

S is a low-punctuation, safety-first programming language implemented in C++20. S 0.2 keeps the S 0.1 syntax and expands it with user-defined types, modules, typed collections, recoverable errors, standard file/path/time facilities, and a safe C ABI bridge.

```s
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

## Build S

You need CMake 3.20+ and a C++20 compiler.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

The CLI is at `build/s`:

```bash
./build/s run examples/type.s
./build/s check examples/modules/main.s
./build/s build examples/collections.s
./examples/collections
./build/s
```

`s build` resolves and checks the full S module graph, emits C++20 for the resolved program, and invokes the system C++ compiler. The resulting executable embeds the S 0.2 runtime code it needs and does not re-read or parse the original S source at startup.

## S 0.2

- S 0.1: `say`, `ask`, variables, math/comparison, indentation blocks, control flow, `make`/`give`, safe List indexing, REPL, `run`, `check`, and native `build`
- user-defined `type`, inferred fields, simple initialization, methods, and implicit current-object field access
- `use` modules with project/std/package resolution and circular-import detection
- typed List, Map, and Set operations and iteration
- recoverable `try` / `try expr` / `fail` error model
- file I/O with automatic resource cleanup, cross-platform paths, `time.now`, and `ms` / `s` / `min` Duration values
- C ABI native modules for Int/Num/Bool/Text and opaque managed native handles; C++ libraries connect through small C wrappers
- interpreter/native parity tests on macOS and Linux CI

See the [S 0.2 language reference](docs/language-reference.md), [types](docs/types.md), [modules](docs/modules.md), [collections](docs/collections.md), [errors](docs/errors.md), [native interoperability](docs/native-interop.md), and the [roadmap](docs/roadmap.md).

## Design boundary

S intentionally does not expose raw pointers, C++ templates, arbitrary C++ class ABI details, manual memory management, or complex import syntax to ordinary S programs. Those details stay in the compiler, runtime, standard library, or a native wrapper layer.

## License

MIT
