# SE 0.4 平台基礎

本文件是 `platform-0.4.md` 的繁體中文版。

SE 0.4 的重點是讓語言從「可以執行程式」開始長成「可以建立專案與自我診斷的平台」。

## `.help`

任何值都可以查詢基本說明：

```se
nums = [1, 2, 3]
say nums.help
```

當 Checker 發現不存在的 member，也會提示可以使用 `value.help`。

## CLI help

```bash
se help
```

顯示目前可用指令。

## doctor

```bash
se doctor
```

用來查看版本、平台、C++ compiler、`SE_HOME` / `S_HOME`、目前目錄，以及 Node/TypeScript bridge 的環境需求。

## 建立 App

```bash
se new app myapp
```

產生基本專案結構與測試檔。

## 建立 Web 專案

```bash
se new web mysite
```

0.4 建立了 backend/frontend scaffold；後續 0.5 才把真正 HTTP server/router 與 browser bridge 補完整。

## math

```se
use math
say math.pi
say math.sqrt 25
say math.pow 2 8
```

包含 `sqrt`、`abs`、`floor`、`ceil`、`round`、`pow`、`min`、`max`。

## random

```se
use random
n = random.int 1 10
x = random.num
```

## os

```se
use os
say os.platform
say os.cwd
```

也提供環境變數相關功能。

## 套件目錄

0.4 開始優先使用：

```text
SE_HOME/packages
```

並保留 `S_HOME/packages` 作為舊名稱 fallback。

## 版本定位

0.4 是 platform foundation，不代表當時所有 Web、DB、HTTPS 或完整 package ecosystem 都已完成。後續能力請看 0.5、0.6 文件。
