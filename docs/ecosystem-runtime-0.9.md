# SE 0.9 Ecosystem Runtime

[繁體中文版](ecosystem-runtime-0.9-zh-TW.md)

This versioned stage expands runtime modules without inventing a separate language syntax for each ecosystem.

## Unified network module

The `net` direction unifies HTTP and HTTPS behind one API while keeping older `http` / `https` modules compatible.

```se
use net

body = try net.get "https://example.com"
reply = try net.request "GET" "https://example.com/api" ""
```

The implementation in this stage uses the system `curl` transport and supports common request/download operations.

## Node.js and Next.js bridges

```se
use node
use next

version = try node.version
code = try node.run "server.js"
try next.build "."
```

These bridges reuse the process-execution model and require the corresponding external tools to be installed.

## Data helpers

The `data` module operates on normal SE List/Map/Set values rather than introducing another collection system.

```se
use data

items = [1, 2]
data.append items 3
last = data.pop items

user = ["name": "SE"]
data.set user "level" 9
```

## Math expansion

The existing `math` module can grow with constants, trigonometry, logarithms, numeric helpers, combinatorics, special functions and statistics while preserving the same module/call model.

## Game display backend

The `game` direction can generate/display HTML Canvas output without requiring an SDL/Pygame installation.

```se
use game

screen = game.new 800 600 "My Game"
game.background screen "#111827"
game.rect screen 50 50 120 80 "#22c55e" true
try game.show screen
```

The stage covers basic drawing/export/runtime input state. More advanced sprite/audio/physics/full callback-loop work remains separate unless implemented by the revision being used.

## Design principle

Ecosystem expansion should reuse `use`, function calls, collections and `try` instead of making every external platform a new syntax subsystem.
