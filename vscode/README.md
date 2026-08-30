# SE Language for Visual Studio Code

Official VS Code support files for SE source code (`.se`).

## Features

- `.se` file recognition as **SE**
- TextMate syntax highlighting for comments, strings, escapes, integers, floating-point values and duration literals
- highlighting for SE control flow: `if`, `else`, `for`, `while`, `repeat`, `match`, `case`, `try`
- highlighting for declarations: `make`, `type`, `use`, function names, type names and generic parameters
- highlighting for core types such as `Int`, `Num`, `Text`, `Bool`, `Bytes`, `List`, `Map`, `Set`, `Option`, `Result` and `Task`
- highlighting for conversion builtins such as `int`, `string`, `double`, `float`, `char` and related aliases
- highlighting for standard modules including `math`, `random`, `json`, `collections`, `async`, `db`, `https`, `web` and others
- operators, member access and type annotations
- indentation support for SE blocks
- snippets for common SE constructs
- **SE: Run File**, **SE: Check File**, and **SE: Build File** commands
- configurable SE executable path

## Local installation

From the repository root:

```bash
cd vscode
npm install
npx vsce package
```

This creates:

```text
se-language-0.6.0.vsix
```

Install it with:

```bash
code --install-extension se-language-0.6.0.vsix
```

Or use **Extensions → ... → Install from VSIX...** inside VS Code.

The extension expects the `se` command to be available in your `PATH`. If SE is installed somewhere else, set **SE: Executable Path** in VS Code settings.

## Commands

Open any `.se` file and use the Command Palette:

```text
SE: Run File
SE: Check File
SE: Build File
```

Run is also available with `Cmd+F5` on macOS or `Ctrl+F5` on Windows/Linux. Check uses `Cmd+Shift+F5` or `Ctrl+Shift+F5`.

## Grammar

The grammar is stored at:

```text
vscode/syntaxes/se.tmLanguage.json
```

Its TextMate scope is:

```text
source.se
```

This grammar is intentionally written as a normal TextMate-compatible grammar so it can also serve as the upstream grammar source when SE becomes eligible for GitHub Linguist language recognition.
