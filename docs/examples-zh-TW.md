# SE 範例索引

[English version](examples.md)

可執行範例集中放在 `examples/`。這份文件只做分類索引，不再把每個 source 重複複製進 docs。

## 語言基礎

- `examples/ask.se` — 輸入。
- `examples/function.se` — 函式。
- `examples/collections.se` — List / Map / Set 基礎。
- `examples/errors.se` — Recoverable error。
- `examples/files.se` — File operation。
- `examples/advanced.se` — 較完整的語言／Runtime 功能。
- `examples/modern-control.se` — 新一點的 control-flow 範例。

所有主要 example 都統一使用 `.se`。舊 `.s` 僅保留在相容性測試或 migration path，不再作為正式範例副檔名。

## Web

- `examples/component-web.se` — `make` + `html/css/js/page` Component model。
- `examples/browser-api.se` — Browser request、JSON 傳送、取消 request、Form 與 multi-page navigation。
- `examples/web-app.se` — 其他 Web 範例。

建置 Component Web example：

```bash
se web build examples/browser-api.se dist
```

如果使用 repository 內尚未 install 的 build：

```bash
./build/se web build examples/browser-api.se dist
```

## Backend / Platform

- `examples/database-adapters.se` — Database adapter。
- `examples/ecosystem.se` — Runtime / ecosystem 功能。
- `examples/game.se` — Game/runtime example。

## 怎麼執行 Example

```bash
se run examples/advanced.se
```

Static check：

```bash
se check examples/advanced.se
```

如果 GitHub `main` 已經有某個 example，但本機 checkout 找不到，先同步 repository：

```bash
git switch main
git pull --ff-only
```

Example 代表它所在 revision 的實際 implementation；帶版本號的設計文件可能另外描述未來或更新階段功能，兩者不要混為一談。
