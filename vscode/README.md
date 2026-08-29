# SE Language for Visual Studio Code

Official VS Code support files for SE source code (`.se`).

## Features

- `.se` file recognition
- syntax highlighting for comments, strings, numbers, keywords, functions, and types
- indentation support for SE blocks
- snippets for `say`, `ask`, `if`, `repeat`, `for`, `while`, `make`, `type`, and `try`
- **SE: Run File** command
- **SE: Check File** command
- **SE: Build File** command
- editor title and context-menu actions
- configurable SE executable path

## Local development / installation

From the repository root:

```bash
cd vscode
npm install
npx vsce package
```

This creates a `.vsix` package. Install it in VS Code with:

```bash
code --install-extension se-language-0.3.0.vsix
```

Or use **Extensions → ... → Install from VSIX...** inside VS Code.

The extension expects the `se` command to be available in your PATH. If it is installed somewhere else, set **SE: Executable Path** in VS Code settings.

## Commands

Open any `.se` file and use the Command Palette:

```text
SE: Run File
SE: Check File
SE: Build File
```

Run is also available with `Cmd+F5` on macOS or `Ctrl+F5` on Windows/Linux. Check uses `Cmd+Shift+F5` or `Ctrl+Shift+F5`.
