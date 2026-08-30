# SE 0.4 平台基礎

[English version](platform-0.4.md)

這份版本化文件記錄 0.4 platform stage：SE 開始從小型可執行語言擴展成 project/tooling platform，同時維持 **Simple at every level**。

## Contextual Help

Value 可以透過 `.help` 查基本說明：

```se
nums = [1, 2, 3]
say nums.help
```

Unknown-member diagnostics 也可以引導使用者查看這個 help path。

## CLI Tooling

```bash
se help
se doctor
```

`se doctor` 用來查看版本、平台、compiler/tool availability、package-home 設定與目前目錄等環境資訊。

## Project Scaffolding

```bash
se new app myapp
se new web mysite
```

0.4 建立 App / Web project structure；後續版本才加入更深入的 HTTP/router/browser 行為。因此這份文件是歷史 version stage，不是目前完整 Web Reference。

## 此階段加入的 Standard Modules

### math

```se
use math
say math.pi
say math.sqrt 25
```

### random

```se
use random
n = random.int 1 10
x = random.num
```

### os

```se
use os
say os.platform
say os.cwd
```

## Package Home Naming

平台命名遷移優先使用：

```text
SE_HOME/packages
```

需要 compatibility 時仍可保留舊 `S_HOME/packages` fallback。

## Native Bridge Foundation

C ABI / `.snative` / Bytes / managed handle / `se bind` 形成 systems interoperability layer，讓一般 SE 程式不需要理解任意 C++ ABI detail。

## 設計方向

0.4 留下的重要方向仍延續到現在：新能力應優先透過一致的 module、runtime service 與 tooling 加入，而不是大量增加新語法。

目前正式用法請看 [文件總覽](README-zh-TW.md)。後續歷史 stage 可看 [SE 0.5 Platform](platform-0.5-zh-TW.md) 與 [SE 0.6 進階功能](advanced-0.6-zh-TW.md)。
