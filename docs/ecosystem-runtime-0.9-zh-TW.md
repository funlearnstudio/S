# SE 0.9 Ecosystem Runtime

[English version](ecosystem-runtime-0.9.md)

這份版本化文件描述 0.9 ecosystem/runtime stage：擴展 module 能力，但不替每個 ecosystem 發明新的語言 syntax。

## Unified Network Module

`net` 的方向是用一套 API 統一 HTTP / HTTPS，同時保留舊 `http` / `https` module compatibility。

```se
use net

body = try net.get "https://example.com"
reply = try net.request "GET" "https://example.com/api" ""
```

此 stage 的 implementation 使用系統 `curl` transport，支援常見 request/download operation。

## Node.js / Next.js Bridge

```se
use node
use next

version = try node.version
code = try node.run "server.js"
try next.build "."
```

這些 bridge 重用 process execution model，並要求系統已安裝對應 external tool。

## Data Helpers

`data` module 直接操作既有 SE List / Map / Set，不建立第二套 collection system。

```se
use data

items = [1, 2]
data.append items 3
last = data.pop items

user = ["name": "SE"]
data.set user "level" 9
```

## Math Expansion

既有 `math` module 可以在不改變 module/call model 的前提下擴充常數、三角、log、numeric helper、combinatorics、special function 與 statistics。

## Game Display Backend

`game` 方向可以產生／顯示 HTML Canvas，而不要求安裝 SDL / Pygame：

```se
use game

screen = game.new 800 600 "My Game"
game.background screen "#111827"
game.rect screen 50 50 120 80 "#22c55e" true
try game.show screen
```

此 stage 包含基本 drawing/export/input runtime state；sprite/audio/physics/full callback loop 等更深能力，只有在實際 revision 已完成時才能視為可用。

## 設計原則

Ecosystem 擴充應重用 `use`、function call、collection 與 `try`，而不是把每個外部平台都變成新的 syntax subsystem。
