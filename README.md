# SE

> Simple at every level.

SE is the next name of the S language. Starting with the 0.3 line, source files use **`.se`** and the command-line tool is **`se`**. The compiler remains C++20, low-punctuation, safety-first, and compatible with existing S 0.2 language semantics. Legacy `.s` files are accepted during the transition, but new projects should use `.se`.

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

## Build from source

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Then run:

```bash
./build/se run examples/type.se
./build/se check examples/modules/main.se
./build/se build examples/collections.se
```

On Windows the executable is normally `build\\Release\\se.exe`.

## Install

GitHub Releases publish ready-to-install binaries for Linux x86_64, macOS Intel, macOS Apple Silicon, and Windows x86_64.

macOS / Linux:

```bash
curl -fsSL https://raw.githubusercontent.com/funlearnstudio/S/main/scripts/install.sh | sh
```

Windows PowerShell:

```powershell
iwr https://raw.githubusercontent.com/funlearnstudio/S/main/scripts/install.ps1 -UseBasicParsing | iex
```

After installation:

```bash
se --version
se run hello.se
se check hello.se
se build hello.se
```

See `docs/installation.md` for PATH and uninstall details.

## VS Code

The repository contains a VS Code extension in `vscode/`. Tagged releases also attach `se-vscode.vsix`. It provides `.se` syntax highlighting, indentation, snippets, and commands for `SE: Run`, `SE: Check`, and `SE: Build`.

## Native interoperability

SE keeps the safe C ABI bridge from S 0.2, including Int/Num/Bool/Text/Bytes, fallible calls, and managed opaque handles. `SEBytesView` is the preferred public spelling; `SBytesView` remains an ABI-compatible alias.

## License

MIT
