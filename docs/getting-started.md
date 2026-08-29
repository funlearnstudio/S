# Getting Started

## Requirements

- CMake 3.20 or newer
- GCC 10+, Clang 12+, or another C++20 compiler

## Build and install

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
cmake --install build --prefix "$HOME/.local"
```

Create `hello.s`:

```s
say "Hello World"
```

Run or compile it:

```bash
s run hello.s
s check hello.s
s build hello.s
./hello
```

Run `s` without arguments for the REPL. A blank line finishes an indented block.
