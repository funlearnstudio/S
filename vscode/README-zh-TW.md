# SE Language for Visual Studio Code

[English version](README.md)

這是 SE `.se` source 的官方 Visual Studio Code 支援。

## 核心功能

- `.se` file recognition 與 TextMate syntax highlighting
- SE block 縮排支援
- 常用語法 snippets
- keyword / module prefix completion
- current file 內的 variable、function、user-defined type completion
- 常用 Runtime module 與推斷 value 的 member completion
- hover information
- current file 內的 Go to Definition / Find References
- Outline / Breadcrumb symbols
- low-punctuation function signature help
- Run / Check / Build current file command
- 可設定 `se` executable path

## Syntax Highlighting

Grammar 涵蓋 comment、string/escape、number、duration literal、operator、declaration、control flow、core type、standard module 與 type annotation。

Grammar：

```text
vscode/syntaxes/se.tmLanguage.json
```

TextMate scope：

```text
source.se
```

這份 grammar 也應維持可作為未來 GitHub Linguist upstream submission 的 grammar source。

## IntelliSense

輸入 prefix 時，VS Code 會縮小 SE-aware candidate set。例如 import `collections` 後：

```se
use collections

collections.
```

可以提供對應 collection helper。

Current file 裡的 user-defined type member 也可以從 declaration 推斷：

```se
type Player
    name = ""
    hp = 100

    make hit damage
        hp -= damage

player = Player
player.
```

這時 extension 可以根據本機 `Player` definition 建議 `name`、`hp`、`hit`。

目前 editor analysis 刻意保持 lightweight，直接在 extension 內執行。未來完整 LSP 可以重用 compiler/parser/checker 做更深入 cross-file semantic analysis，但不需要改變使用者-facing editor model。

## Commands

Command Palette：

```text
SE: Run File
SE: Check File
SE: Build File
```

Extension 預設期待 `se` 已在 `PATH`。若安裝在其他位置，可設定 **SE: Executable Path**。

## 本機打包

從 repository root：

```bash
cd vscode
npm install
npx vsce package
```

接著使用 VS Code 的 **Install from VSIX...**，或 `code --install-extension` 安裝產生的 `.vsix`。

套件檔名與版本應以目前 package metadata 為準，不要在教學裡硬編一個之後會過期的版本號。

## 相關文件

- [SE 文件總覽](../docs/README-zh-TW.md)
- [GitHub Syntax Highlighting](../docs/github-syntax-highlighting-zh-TW.md)
