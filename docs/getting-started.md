# Getting Started with SE 0.3

SE uses `.se` source files and the `se` command-line tool.

## Requirements

- CMake 3.20 or newer
- A C++20 compiler
- macOS/Linux: Clang or GCC
- Windows: MSVC or another supported C++20 toolchain

## Build from source

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Install on macOS/Linux

```bash
cmake --install build --prefix "$HOME/.local"
export PATH="$HOME/.local/bin:$PATH"
se --version
```

Expected version output:

```text
SE 0.3.0
```

To keep the PATH change in zsh:

```bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

## First program

Create `hello.se`:

```se
say "Hello SE"
```

Run it with the interpreter:

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
./hello
```

## REPL

Run SE without arguments:

```bash
se
```

Example:

```text
SE 0.3.0
> say "Hello"
Hello
```

A blank line finishes an indented block in the REPL.

## Main CLI commands

```text
se
se --version
se check file.se
se run file.se
se build file.se
se bind module.sbind [output-directory]
```

## Learn SE

- [Traditional Chinese tutorial](tutorial-zh-TW.md)
- [Traditional Chinese technical reference](technical-reference-zh-TW.md)
- [Language reference](language-reference.md)
- [Syntax reference](syntax-reference.md)
- [Native interoperability](native-interop.md)

SE 0.3 uses `.se` as the primary extension. Legacy `.s` compatibility may remain during the migration period, but new code should use `.se`.