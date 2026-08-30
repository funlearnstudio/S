# SE Web Language（0.8）

[English version](web-language-0.8.md)

SE Web 保留 HTML、CSS 與 browser behavior 的核心概念，同時用 SE 的縮排、`make` 與低標點語法表達常見 Web 結構。

> One language, not necessarily one file.

## Component

Web component 直接沿用 `make`：

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

使用方式和一般 SE call 一樣：

```se
Button "Save"
Button "Cancel"
```

Compiler 會把 `html`、`css`、`js` section 關聯到同一個 component，並產生內部 instance identity 供 event binding 與 scoped CSS 使用。

Zero-argument component 也可以直接放進 page：

```se
make Navbar
    html
        nav "SE"

page "/"
    Navbar
```

## HTML

```se
html
    main
        h1 "Hello"
        p "Welcome"
        button "Start"
```

HTML tag 保留一般 Web concept，不需要把所有 tag 都變成 Lexer keyword。

## CSS

```se
css
    padding 12
    border_radius 8
    background "white"

    button:hover
        opacity 0.8
```

SE-style underscore property 會映射到 CSS dash，例如 `border_radius` → `border-radius`。數值依 compiler 的 CSS value rule 轉換；Component CSS 會自動 scope。

## Browser Behavior

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

目前 browser event lowering 已涵蓋常見 assignment、condition、loop、error handling、value match、collection expression、function/member call，以及 Browser API 支援的 async operation。

## Pages

同一份 source 可以宣告多個 page：

```se
page "/"
    Home

page "/settings"
    Settings

page "/about"
    About
```

Generated router 使用 History API 切換已宣告 route。`browser.go`、`browser.replace`、back/forward 與 HTTP/form API 請看 [Browser API](browser-api-0.8-zh-TW.md)。

## Build

```bash
se web build app.se dist
```

輸出：

```text
dist/
  index.html
  style.css
  app.js
  app.ts
```

這些都是一般 Web 檔案。SE Web 刻意讓 generated HTML/CSS/JS 可直接查看，因為 generated output 本身也可以是學習 Web platform 的橋梁。

## Native Escape Hatch

遇到尚未映射的 Web platform feature，可以局部使用 native HTML/CSS/JS：

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

Native Web code 仍遵循原生 HTML/CSS/JavaScript 的安全與正確性規則。

## Browser / Server 邊界

Browser component 與 server capability 分層：

```text
SE browser component
    ↓ HTTPS / API
SE backend
    ↓
database / external service
```

Filesystem、Native FFI、server listener 與 server-only credential 不會自動轉成 browser JavaScript API。

## Legacy `ui.*` DSL

較早的 `ui.*` Web authoring path 仍可保留相容性。新 component source 使用 `make` + `html/css/js/page`，舊 Web source 不需要立刻全部改寫。

## Current Boundaries

SE Web 仍是持續演進的 compiler target。`app.ts` 是 TypeScript-compatible companion output，不代表每個 SE type annotation 都會被高精度保留。History API route 的 direct refresh 仍需要 hosting rewrite/fallback；少數進階 Web platform feature 仍可能需要 `native`。

Request、JSON、表單、取消 request 與 navigation 請接著看 [Browser API](browser-api-0.8-zh-TW.md)。
