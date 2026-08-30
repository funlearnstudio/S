# SE 0.6.0

SE 0.6.0 is the first SE release designed for quick public installation with prebuilt packages. Normal users can download SE and immediately use `se run`, `se check`, `se test`, the REPL, modules and the interpreter without installing CMake, Git or a C++ compiler.

## Quick install

macOS / Linux:

```sh
curl -fsSL https://raw.githubusercontent.com/funlearnstudio/SE/main/install.sh | sh
```

Windows PowerShell:

```powershell
irm https://raw.githubusercontent.com/funlearnstudio/SE/main/install.ps1 | iex
```

GitHub Releases also provides downloadable archives for macOS Apple Silicon, macOS Intel, Linux x64 and Windows x64.

## Language and runtime

- Low-punctuation SE syntax with indentation-based blocks.
- Static checking and type inference.
- Functions, lexical closures and simplified function helpers.
- User-defined types, fields and methods.
- Generic functions.
- Value-based `match` / `case`.
- List, Map and Set collections.
- `filter`, `map`, `reduce`, slicing and sorting helpers.
- Option and Result helpers.
- Managed Task-style async / await.
- File, path, time, math, random and OS support.
- JSON, HTTP, web routing, process execution and lightweight persistent DB APIs.
- HTTPS client support through system `curl`.
- C ABI native interoperability and binding generation.

## Core conversions

SE 0.6.0 includes beginner-friendly conversion names:

```se
int "123"
integer "123"
num "3.14"
double "3.14"
float "3.14"
text 123
string 123
bool "true"
boolean "false"
char "A"
char 65
```

The canonical SE runtime types remain `Int`, `Num`, `Text` and `Bool`; familiar names such as `string`, `double` and `float` are conversion aliases rather than duplicate runtime types. `char` currently produces one-character Unicode Text.

## Distribution

The release pipeline builds and verifies precompiled SE packages on supported GitHub-hosted platforms and publishes SHA-256 checksums with the release assets.

A prebuilt SE installation does **not** require CMake or a C++ compiler for normal interpreted workflows such as:

```sh
se run file.se
se check file.se
se check-all .
se test .
```

`se build` is different: it currently emits C++20 and invokes a system C++ compiler to produce a native executable, so native compilation still requires a C++20 compiler. This limitation is intentionally documented rather than hidden.

## Version

SE 0.6.0
