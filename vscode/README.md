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
- prefix IntelliSense: typing `h`, `ma`, `rep`, etc. immediately narrows SE keyword/module suggestions
- local symbol completion for functions, user types and variables defined in the current `.se` file
- member completion after `.` for supported SE modules such as `http.`, `https.`, `json.`, `db.`, `collections.`, `option.`, `result.`, `async.` and `function.`
- inferred member completion for common values such as Text/List and for user-defined types
- user-type field and method completion, e.g. `player.` can suggest fields and methods from `type Player`
- hover information for SE builtins, functions, variables and user-defined types
- **Go to Definition** for local functions, variables and types
- **Find References** within the current SE file
- Outline / Breadcrumb document symbols for types, fields, methods, functions and variables
- low-punctuation function signature help while entering arguments
- **SE: Run File**, **SE: Check File**, and **SE: Build File** commands
- configurable SE executable path

## IntelliSense examples

Typing a prefix:

```text
h
```

can suggest SE words such as `html`, `http`, and `https`. VS Code performs the prefix filtering while the SE extension supplies the language-aware candidate set.

Module members:

```se
use collections

collections.
```

can suggest helpers such as `filter`, `map`, `reduce`, `slice`, `take`, `drop`, and sorting operations.

User-defined type members:

```se
type Player
    name = ""
    hp = 100

    make hit damage
        hp -= damage

player = Player
player.
```

can suggest `name`, `hp`, and `hit` based on the type declared in the file.

The current IntelliSense implementation is intentionally lightweight and runs directly inside the extension. It understands common SE declarations and simple local type inference without requiring a separate language-server process. A future full LSP can reuse the compiler/parser/type-checker for cross-file semantic analysis while preserving this editor experience.

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
