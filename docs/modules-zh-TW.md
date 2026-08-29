# 模組系統

本文件是 `modules.md` 的繁體中文版。

SE 使用 `use` 載入模組。

```se
use math
say math.sqrt 25
```

## 本機模組

假設：

```text
app/
├── main.se
└── tools.se
```

`main.se`：

```se
use tools
```

Module loader 會解析本機來源檔、標準模組與套件搜尋路徑。

## 私有名稱

以下劃線開頭的名稱採 private convention：

```se
_secret = 123
```

它用來表示模組內部實作，不應作為公開 API。

## 依賴圖

Loader 會建立 module dependency graph，並偵測 circular import。循環相依會回報錯誤，而不是無限載入。

## 標準 / Built-in modules

目前包含或可使用的模組包含 `file`、`path`、`time`、`math`、`random`、`os`、`json`、`text`、`collections`、`test`、`process`、`http`、`web`、`js`、`ts`，以及 SE 0.6 的 `function`、`async`、`option`、`result`、`match`、`db`、`https`。

## 套件搜尋

SE 優先使用 `SE_HOME/packages`，並保留舊的 `S_HOME/packages` fallback。

模組系統的方向是：使用簡單的 `use name`，讓路徑解析、循環偵測與 API 邊界由 loader 處理。
