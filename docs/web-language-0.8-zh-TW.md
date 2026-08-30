# SE Web Language（0.8 foundation）

SE 的網站方向是讓同一份 `.se` 原始碼可以描述瀏覽器頁面結構、CSS 與互動邏輯，再轉換成一般瀏覽器和靜態主機可以直接使用的網站檔案。

## 一個檔案建立前端

```se
ui.page "My SE Site"

ui.style "body" [
    "font-family": "system-ui, sans-serif",
    "margin": "2rem"
]

count = 0
root = ui.el "main"
title = ui.el "h1" "Hello from SE"
button = ui.el "button" "Count"
output = ui.el "p" "0"

ui.add root title
ui.add root button
ui.add root output

make clicked event
    count += 1
    if count >= 10
        ui.text output "10+"
    else if count >= 5
        ui.text output "5+"
    else
        ui.text output count

ui.on button "click" clicked
ui.mount root
```

建置：

```sh
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

`index.html` 是標準 HTML 入口；`style.css` 來自可在編譯期解析的 `ui.style`；`app.js` 是瀏覽器可直接執行的 JavaScript；`app.ts` 是同一份 SE 邏輯產生的 TypeScript-compatible 版本。

## UI 基礎 API

目前 browser runtime 提供：

- `ui.el tag [text]`：建立 HTML element。
- `ui.add parent child`：加入子節點。
- `ui.mount node [target]`：掛到 `#app` 或指定 selector。
- `ui.find selector` / `ui.all selector`：查找 DOM。
- `ui.attr node name value`：設定 attribute。
- `ui.text node value` / `ui.html node value`：更新內容。
- `ui.value node` / `ui.set_value node value`：表單值。
- `ui.class node name [enabled]`：切換 class。
- `ui.on node event handler`：瀏覽器事件。
- `ui.fetch url [options]`：瀏覽器 Fetch API 包裝。

## CSS

字面量 selector + Map 會直接被抽成 `style.css`：

```se
ui.style ".card" [
    "padding": "1rem",
    "border-radius": "12px"
]
```

動態 `ui.style` 仍可在 browser runtime 建立 `<style>`。

## 新控制語法

此階段也加入：

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

Compound assignment 在 AST 階段會被簡化成一般 assignment + binary operation，因此 Interpreter、native path 與 web compiler 可以共用既有語意，而不是維護五套重複執行邏輯。

## HTML / CSS / JS / TS 的關係

SE 不把四種語言的所有符號原封不動搬進來，而是把共同概念集中到 SE：

- HTML → `ui.el`、`ui.add`、`ui.attr`、`ui.mount`
- CSS → `ui.style`
- JavaScript → SE 的變數、函式、if/else、迴圈、事件邏輯
- TypeScript → SE 的靜態型別資訊逐步映射；目前 `app.ts` 是 TypeScript-compatible 輸出，後續會保留更多 SE type annotation

這符合 `Simple at every level`：使用者主要寫 SE，compiler 負責產生瀏覽器真正需要的格式。

## 目前邊界

這是 browser compiler 的第一個 foundation，不代表完整 SE runtime 已全部移植到瀏覽器。特別是：

- server-only `web.listen`、檔案系統、native FFI 不會直接轉成 browser code。
- 泛型在 browser output 目前 runtime-erased。
- user type 可以產生 JavaScript class，但 method 中對 SE implicit field 的完整 `this` 語意仍需要後續加深。
- `app.ts` 目前是 TypeScript-compatible，而不是已完整保留所有 SE 型別的高精度 TS emitter。
- 資料庫密碼、MongoDB URI、私密 token 不應放進 frontend `.se`；瀏覽器應呼叫 SE backend/API。

## Database 方向

既有 `db` 本地 Text key/value store 保持相容。大型外部資料庫會走統一 adapter 層：MongoDB 放在 server-side adapter；Google Apps Script 使用 HTTPS Web App JSON adapter。這樣可以讓 frontend、backend 與 DB 的責任分開，而不是把資料庫憑證編譯進 `app.js`。
