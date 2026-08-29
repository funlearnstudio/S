# 範例程式導覽

本文件是 `examples.md` 的繁體中文版。

`examples/` 目錄用來展示可直接執行的 SE 程式。建議學習時先閱讀原始碼，再用 `se check` 與 `se run` 驗證。

## 基本流程

```bash
se check examples/function.s
se run examples/function.s
```

舊的 `.s` 範例目前仍可使用；新的程式建議使用 `.se`。

## 主要範例類型

- `ask.s`：輸入與輸出。
- `function.s`：`make` 與 `give`。
- `collections.s`：List / Map / Set。
- `errors.s`：`try` / `fail`。
- `files.s`：檔案 I/O。
- `type.s` / methods 類範例：user-defined type 與 method。
- modules 範例：`use` 與模組載入。
- `platform.se`：JSON、text、process 等平台 API。
- `web-router.se`：Web router 與 request/response。
- `generics.se`：SE 0.6 泛型函式。
- `advanced.se`：filter/map/reduce、closure、pattern matching、Option/Result、async、database 等 0.6 功能。

## 建議順序

先看基本 I/O 與函式，再看 collections、type、module、errors，最後進入 `platform.se`、Web 與 `advanced.se`。

所有範例都應視為「可以跑的文件」。如果教學文字和程式行為不同，以目前測試通過的實作為準，並回頭修正文件。
