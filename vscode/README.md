# SE Language for Visual Studio Code

[繁體中文版](README-zh-TW.md)

Official Visual Studio Code support for SE source files (`.se`).

## Core features

- SE `.se` file recognition and TextMate syntax highlighting
- indentation support for SE blocks
- snippets for common language constructs
- prefix completion for keywords and modules
- local completion for variables, functions and user-defined types
- member completion for common runtime modules and inferred values
- hover information
- Go to Definition and Find References within the current file
- Outline / Breadcrumb symbols
- low-punctuation function signature help
- commands for running, checking and building the current SE file
- configurable path to the `se` executable

## Syntax highlighting

The grammar covers comments, strings/escapes, numeric and duration literals, operators, declarations, control flow, core types, standard modules and type annotations.

Grammar source:

```text
vscode/syntaxes/se.tmLanguage.json
```

TextMate scope:

```text
source.se
```

The same grammar is intended to remain suitable as the upstream grammar source for a future GitHub Linguist submission.

## IntelliSense

Prefix completion narrows the language-aware candidate set as you type. For example, `collections.` can offer collection helpers after importing the module.

```se
use collections

collections.
```

User-defined members can also be inferred from declarations in the current file:

```se
type Player
    name = ""
    hp = 100

    make hit damage
        hp -= damage

player = Player
player.
```

The extension can suggest `name`, `hp`, and `hit` from the local `Player` definition.

The current editor analysis is intentionally lightweight and runs inside the extension. A future full LSP can reuse compiler/parser/checker semantics for deeper cross-file analysis without changing the user-facing editor model.

## Commands

From the Command Palette:

```text
SE: Run File
SE: Check File
SE: Build File
```

The extension expects `se` to be available in `PATH`. If it is installed elsewhere, configure **SE: Executable Path** in VS Code settings.

## Package locally

From the repository root:

```bash
cd vscode
npm install
npx vsce package
```

Install the generated `.vsix` with VS Code's **Install from VSIX...** command or the `code --install-extension` CLI.

The package filename/version follows the extension's current package metadata; do not hard-code an old version when following these instructions.

## Related documentation

- [SE Documentation](../docs/README.md)
- [GitHub Syntax Highlighting](../docs/github-syntax-highlighting.md)
