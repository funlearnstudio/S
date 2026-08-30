# SE Web Language（0.8 foundation）

SE Web 的方向不是再發明一門 Web framework 語言，而是讓 HTML、CSS、JavaScript 的重要概念繼續存在，同時用已經熟悉的 SE 語法與縮排規則表達它們。

核心原則：

> One language, not necessarily one file.

> 語言統一、功能多元、概念重用。

## 建議寫法：`make` 就是元件

Web 元件沿用 SE 原本的 `make`，不增加另一套 component function system：

```se
make Button text
    html
        button text

    css
        padding 12
        border_radius 8

    js
        when click
            say text
```

使用時就是一般 SE 呼叫：

```se
Button "Save"
Button "Cancel"
Button "Next"
```

Compiler 會把同一個 `make Button` 內的 `html`、`css`、`js` 關聯成同一個 component definition。每次使用都建立獨立 instance，產生內部 `data-se-instance` identity，不需要手動維護 HTML id、CSS selector 與 `getElementById()`。

## 完整小型網站

```se
make Button text
    html
        button text

    css
        padding 12
        border_radius 8

        button:hover
            opacity 0.8

    js
        when click
            say text

make Card text
    text text

    style
        padding 20
        border_radius 12

page "/"
    Card "Hello"
    Button "Save"
    Button "Cancel"
    Button "Next"
```

建置：

```sh
se web build app.se dist
```

輸出仍然是普通 Web 標準檔案：

```text
dist/
  index.html
  style.css
  app.js
  app.ts
```

瀏覽器不需要特殊 SE runtime 格式；`index.html`、`style.css`、`app.js` 可以直接由一般靜態主機提供。`app.ts` 目前是 TypeScript-compatible companion output。

## HTML：保留 HTML 概念，減少標點

```se
html
    div
        h1 "Hello"
        p "Welcome"
        button "Start"
```

Web AST 會保留 element tree，Compiler 再輸出真正 HTML。HTML tag 使用一般 identifier，因此不需要為 `div`、`button`、`section` 等每一個 tag 都增加 Lexer keyword 或 parser 特例，未來也能持續擴充。

元件參數可以直接進入 HTML：

```se
make Title text
    html
        h1 text
```

## CSS：SE 命名映射到真正 CSS

```se
css
    padding 12
    border_radius 8
    background "white"
```

會得到概念上：

```css
padding: 12px;
border-radius: 8px;
background: white;
```

數字預設轉成 `px`；像 `opacity` 等 unitless property 例外。底線 property 會映射成 CSS dash，例如 `border_radius` → `border-radius`。

Nested selector：

```se
css
    button
        padding 12

    button:hover
        opacity 0.8
```

Component CSS 會自動 scope 到 `.se-ComponentName`，避免不同 component 的常用 selector 互相污染。使用者不必自己同步三套名稱。

## JavaScript：行為繼續寫 SE

```se
js
    when click
        say text
```

`click` 這個 event 概念沒有被藏掉；只是 Compiler 自動產生 DOM event binding。

目前 component event block 可 lowering 的 SE statement 包含：

- `say`
- assignment 與 `+=` / `-=` / `*=` / `/=` / `%=`
- `if` / `else if` / `else`
- `repeat`
- `while`
- 一般 expression / function call
- `give`
- `fail`

例如：

```se
js
    when click
        count += 1
        if count >= 10
            say "10+"
        else if count >= 5
            say "5+"
        else
            say count
```

## `html` / `css` / `js` 都是可選的

只有 HTML：

```se
make Title text
    html
        h1 text
```

HTML + CSS：

```se
make Card text
    html
        div text

    css
        padding 20
```

HTML + JS：

```se
make Button text
    html
        button text

    js
        when click
            say text
```

SE 不要求每個 component 一定要有三個 section。

## 短寫形式仍然存在

簡單元件不用被迫展開成完整 HTML：

```se
make Card text
    text text

    style
        padding 20
```

在 Web component context 中，`text value` 會降低到 component content；`style` 與 `css` 使用同一組 Web CSS model。一般非 Web `make` 的既有 SE 呼叫語意不因此改變。

## 原生 Web escape hatch

SE 不試圖立刻重新包裝 Web 平台的每個冷門能力。遇到尚未映射的功能，可以局部下沉：

```se
html
    native "<dialog open>Advanced HTML</dialog>"
```

```se
css
    native "accent-color: auto;"
```

```se
js
    native "console.info('native browser code')"
```

常用功能維持 SE 形式，進階需求仍然能接回原生 Web。

## 舊 `ui.*` browser DSL 保持支援

0.8 之前已經存在的 browser authoring path 不會因 component syntax 被移除。例如：

```se
ui.page "My SE Site"
ui.style "body" ["margin": "2rem"]
root = ui.el "main"
button = ui.el "button" "Save"
ui.add root button
ui.mount root
```

`se web build` 會自動判斷：

- 新 `make/html/css/js/page` component source → component Web compiler
- 舊 `ui.*` source → 原本 Web compiler

這讓新模型可以加入而不破壞既有來源。

## Multiline List / Map

Web styling 常需要比較長的 Map，因此 parser 也接受 bracket 內換行，例如：

```se
ui.style "body" [
    "font-family": "system-ui, sans-serif",
    "margin": "2rem"
]
```

這仍然是 SE 的一般 Map，不是 Web 專用資料結構。

## `+=` 與 `else if`

0.8 foundation 同時加入：

```se
score += 10
score -= 2
score *= 3
ratio /= 2
index %= 4
```

以及：

```se
if score >= 90
    say "A"
else if score >= 80
    say "B"
else
    say "C"
```

Compound assignment 在 parser lowering 成既有 assignment + binary operation，因此 Interpreter、native path 與 Web compiler 共用原本語意，而不是維護五套重複 runtime operation。

## Generated output 是教學的一部分

SE Web 不把 HTML/CSS/JS 永久藏起來。執行：

```sh
se web build app.se dist
```

之後可以直接查看：

- `dist/index.html`：SE `html` / component 對應的真正 HTML
- `dist/style.css`：SE `css` / `style` 對應的真正 CSS
- `dist/app.js`：`when click` 等事件對應的真正 JavaScript
- `dist/app.ts`：目前的 TypeScript-compatible output

因此 SE Web 可以作為原生 Web 的學習橋梁。

## Database 邊界

Browser component 與資料庫故意分層。MongoDB URI、密碼、token 不應進入 frontend `.se`，而應放在 SE backend：

```text
SE browser component
    ↓ HTTPS / API
SE backend
    ↓ db.connect / db.exec
MongoDB / Google Apps Script
```

既有本地 `db.open/set/get/...` 保持相容；0.8 foundation 另有統一 `db.connect` / `db.exec` adapter，可連 MongoDB 與 Google Apps Script Web App。

## 目前限制

這是 component Web model 的 foundation，不假裝已覆蓋完整 Web 平台：

- `page` 已建立 Web AST 與 root page build，但目前 `se web build` 一次主要輸出 `/`；多 route → 多 HTML file/router lowering 尚未完成。
- Component CSS 目前主要是 definition-level scoped CSS；直接使用 component parameter 產生每個 instance 不同 CSS 尚未完成。
- Component JS event lowering 已涵蓋常用 SE statement，但完整 `for`、`match`、async/browser module 等仍會逐步補齊。
- HTML attribute 的 component shorthand 還在擴充；舊 `ui.attr` / `ui.el` path 仍可處理需要更直接 DOM control 的情況。
- 多檔大型 component project 的 import/lowering 還沒有完整串到 component compiler；目前單檔小型網站是完整主路徑。
- `app.ts` 尚未高精度保留所有 SE type annotation。
- Server-only `web.listen`、檔案系統與 native FFI 不會直接轉成 browser JavaScript。
- 原生 escape hatch 由開發者自行承擔原生 HTML/CSS/JS 的安全與正確性。

這些限制不改變既有 Interpreter、Type Checker、native backend 或舊 Web DSL 的語法。Web source 的專用 semantic validation 也會在 `se web build` 階段檢查 component arity、build-time CSS value 等 Web-specific 規則。
