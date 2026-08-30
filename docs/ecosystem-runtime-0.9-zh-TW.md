# SE 0.9 Ecosystem Runtime

這一階段不增加新的語法，而是把既有 `use`、函式呼叫、List、Map、`try` 等概念延伸到更多領域。

## 統一 HTTP / HTTPS

舊的 `http` 與 `https` 模組保持相容；新的 `net` 是統一介面，同一套 API 接受 `http://` 與 `https://`：

```se
use net

body = try net.get "https://example.com"
reply = try net.request "GET" "https://example.com/api" ""
```

`net` 使用系統 `curl`，支援 GET、POST、JSON POST、任意 method request 與 download。

## Node.js / Next.js

```se
use node
use next

version = try node.version
say version

code = try node.run "server.js"
result = try node.eval "console.log(2 + 2)"

try next.build "."
```

`node` 是 Node/npm/npx bridge；`next` 重用同一個程序執行模型，提供 create/dev/build/start/lint。這些功能需要系統已安裝對應工具。

## Python 式資料管理

```se
use data

items = [1, 2]
data.append items 3
data.insert items 0 0
last = data.pop items

user = ["name": "SE"]
data.set user "level" 9
name = data.get user "name"
data.delete user "level"
```

目前包含 append、extend、insert、pop、clear、copy、get、set、update、delete、has、keys、values、items。它們直接操作 SE 原本的 List/Map/Set，不建立第二套資料結構。

## Math 擴充

既有 `use math` 保持不變，但功能擴充為：

- 常數：pi、e、tau、inf
- 基礎：sqrt、cbrt、abs、floor、ceil、round、trunc、pow、min、max、clamp、lerp、map_range、sign
- 三角：sin、cos、tan、asin、acos、atan、atan2、degrees、radians
- 雙曲：sinh、cosh、tanh、asinh、acosh、atanh
- 指數/對數：exp、exp2、expm1、log、log10、log2、log1p
- 數值：hypot、fmod、remainder、copysign、nextafter、isfinite、isinf、isnan
- 整數/組合：gcd、lcm、factorial、comb、perm
- 特殊函數：gamma、lgamma、erf、erfc
- 統計：sum、mean、median、variance、stddev

## Game 顯示器

`game` 是第一個不依賴 SDL/Pygame 安裝的遊戲顯示 backend。它使用標準 HTML Canvas，因此產物可以直接在瀏覽器顯示。

```se
use game

screen = game.new 800 600 "My Game"
game.background screen "#111827"
game.rect screen 50 50 120 80 "#22c55e" true
game.circle screen 350 200 40 "#60a5fa" true
game.line screen 0 500 800 500 "white" 2
game.text screen "Hello SE" 300 550 32 "white"

try game.show screen
```

也可以 `game.save screen "game.html"`，或用 `game.html screen` 取得完整 HTML。生成的 runtime 已維護 keyboard set 與 mouse position/down state，並保留 `game.script` 作為進階 Canvas/animation escape hatch。

目前 game 已能建立視窗/Canvas、背景、矩形、圓、線、文字、HTML export、save/show 與鍵鼠 runtime state；sprite sheet、音訊 mixer、碰撞/physics、完整 SE callback game loop 仍是後續深度工作。這些會繼續放在同一個 `game` 模組，不新增一套語言。
