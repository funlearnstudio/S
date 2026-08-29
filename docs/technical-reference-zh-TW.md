# SE 0.3 技術文件（繁體中文）

這份文件說明 SE 目前的實作架構、編譯流程、型別與 runtime 設計、module resolution、native ABI、測試與 distribution 邊界。它描述的是目前 repository 中可見的 SE 0.3 實作，而不是未完成的未來規格。

## 1. 專案定位

SE 是以 C++20 實作的低標點、縮排式、安全優先程式語言。使用者-facing 名稱為 **SE**，CLI 為 `se`，主要 source extension 為 `.se`。

目前內部 C++ namespace、header path 與部分 compatibility macro 仍保留 `s`，例如：

```cpp
namespace s
#include "s/parser.hpp"
```

這些是 implementation detail，不代表使用者 CLI 仍叫 `s`。

## 2. Compiler pipeline

目前主要 pipeline：

```text
.se source
    ↓
Lexer
    ↓
Tokens + INDENT / DEDENT
    ↓
Pratt Parser
    ↓
AST
    ↓
Static Checker / Type Inference
    ├── Interpreter → immediate execution
    └── C++ Backend → C++20 → system compiler → native executable
```

`se check` 走 frontend + static checker；`se run` 再交給 Interpreter；`se build` 產生 C++20 並呼叫系統 C++ compiler。

## 3. Lexer

Lexer 負責一般 tokenization，也直接處理 indentation。

Block 不使用 `{}`，所以 lexer 會根據縮排產生：

```text
INDENT
DEDENT
```

例如：

```se
if ready
    say "yes"
say "done"
```

parser 看到的 block boundary 由 `INDENT` / `DEDENT` 明確表示，而不是靠 parser 猜測 whitespace。

## 4. Parser

Expression parsing 使用 Pratt Parser，讓 operator precedence 與 associativity 集中處理。

主要 precedence 由高至低包括：

```text
**
unary - / not
* / %
+ -
..
comparison
equality
and
or
```

`**` 為 right-associative。

Statement 層處理 assignment、control flow、function、type、module import、error handling 等語法。

## 5. AST

Lexer/Parser 不直接執行程式，而是建立 AST。Checker、Interpreter 與 C++ backend 共享相同 AST，避免三套語意各自解析 source。

這個分層讓：

```text
syntax
semantic checking
execution
native code generation
```

可以相對獨立演進。

## 6. Static Checker 與型別推斷

SE 會在可推斷的地方自動得到型別：

```se
age = 15
name = "Steve"
ready = true
```

不要求使用者重複寫：

```text
Int age = 15
```

但推斷後仍會檢查不相容 assignment、未知名稱、錯誤 function usage、collection element type 等問題。

因此 SE 的目標是：

> 少寫型別，不等於沒有型別系統。

## 7. Runtime value model

Runtime 以 C++ managed value representation 表示 SE values。使用者程式不直接接觸 raw pointer、`new`、`delete`、`malloc` 或 `free`。

主要 value family 包括：

```text
None
Int
Num
Bool
Text
Bytes
List
Map
Set
Function
Object
Error
Path / File / Duration 等 runtime values
Native managed handles
```

需要共享生命週期的 runtime object 由 runtime ownership 機制管理。

## 8. Safety boundary

Runtime 會主動檢查可能產生不安全行為的操作，例如：

- List index 越界
- 無效 member access
- function arity 錯誤
- 不合法 runtime operation
- file/native failure

這些應轉成 SE error，而不是把 C++ undefined behavior 暴露給 SE 程式。

SE 不打算讓一般程式直接使用 raw pointer、任意 C++ class ABI、template internals 或 manual memory management。

## 9. Functions

函式：

```se
make add a b
    give a + b
```

呼叫：

```se
answer = add 5 3
```

SE 採低標點 call syntax。Interpreter 使用 lexical environment 執行 function；backend 需要產生等價 native 行為。

## 10. User-defined Types

Type：

```se
type Player
    name = ""
    hp = 100

    make hit damage
        hp = hp - damage
```

Field type 從 default value 推斷。Method 使用與一般 function 相同的 `make`。

Method 內未被 local variable shadow 的 field 名稱可解析為 current object field，因此一般程式不需要強制寫 `self`。

每個：

```se
player = Player
```

建立自己的 object field storage。

## 11. Collections

### List

```se
nums = [1, 2, 3]
nums.add 4
nums.remove 2
```

### Map

目前 Map key 以 Text 為主要支援形式：

```se
user = ["name": "Steve"]
say user["name"]
```

Map iteration：

```se
for key value in user
    say key
    say value
```

### Set

```se
values = set [1, 2, 2, 3]
```

Set 保留 unique values。

## 12. Module system

普通 source module 使用：

```se
use player
```

SE 0.3 module resolution 優先尋找 `.se`：

```text
player.se
player/player.se
```

並暫時保留 legacy `.s` migration compatibility。

Built-in standard modules目前包含：

```text
file
path
time
```

Top-level 名稱若以 `_` 開頭視為 private convention。其他公開名稱可被 importing module 使用。

Loader 會建立 dependency graph 並拒絕 circular import，例如：

```text
a → b → a
```

## 13. Error model

Recoverable runtime failure 可以用：

```se
try
    text = read "data.txt"
else err
    say err
```

自訂錯誤：

```se
fail "message"
```

Propagation：

```se
make load
    give try read "data.txt"
```

Error object 可提供：

```text
message
source
line
kind
```

Static syntax/type error 則屬 compiler/checker error，不是一般 runtime `try` 的目標。

## 14. File / Path / Time

File I/O 由 runtime 管理 resource lifetime。Managed file resource 使用 RAII 思路避免使用者自己管理底層 stream。

Path 使用 `std::filesystem`，避免硬編碼不同 OS path separator。

Duration literals：

```text
500ms
2s
1min
```

Time implementation 使用 `std::chrono`。

## 15. Bytes

`Bytes` 是 binary-safe byte buffer：

```se
data = bytes "hello"
say data.len
```

設計上 Text 與 Bytes 分離，讓 binary/native API 不必把任意 byte stream 假裝成字串。

## 16. Native C ABI Bridge

Native module metadata 使用 `.snative`：

```text
library native_test
add s_test_add Int Int -> Int
bytes_len s_test_bytes_len Bytes -> Int
```

SE code 仍透過普通：

```se
use native_test
```

使用 native module。

Bridge 支援的主要 ABI values 包括：

```text
Int
Num
Bool
Text
Bytes
None
opaque managed handles
```

C++ library 不直接把任意 C++ ABI 暴露給 SE；推薦結構是：

```text
C++ library
    ↓
C ABI wrapper
    ↓
.snative metadata
    ↓
SE
```

## 17. Bytes ABI ownership

公開 C ABI 使用 `SBytesView`，SE 0.3 可保留/提供 SE naming compatibility alias。

Buffer 從 SE 傳給 C 時，native code 在 call 期間取得 borrowed view。

Native function 回傳 byte view 時，runtime 將資料 copy 成 SE-owned buffer，再交回 SE 程式。這樣 ownership 不必由 SE 使用者手動管理。

## 18. Opaque Native Handles

Native resource 可以宣告 handle type 與 cleanup symbol，例如：

```text
make_counter s_test_counter_create -> Handle:Counter cleanup s_test_counter_destroy
```

SE 收到的是 managed handle，不是 raw address。Resource lifetime 結束時，runtime 執行對應 cleanup function。

## 19. Binding Generator

`.sbind` 可以描述簡單 native API：

```text
module mathx
library mathx
header mathx.hpp

add math_add Int Int -> Int
```

執行：

```bash
se bind mathx.sbind generated
```

目前 generator 的輸出包含：

```text
mathx.snative
mathx_bindings.h
mathx_bindings.cpp
```

不支援的 signature 應直接報錯，而不是產生假的 incomplete wrapper。

## 20. CLI

### REPL

```bash
se
```

### Version

```bash
se --version
```

### Static check

```bash
se check app.se
```

### Interpreter

```bash
se run app.se
```

### Native build

```bash
se build app.se
```

### Binding generator

```bash
se bind module.sbind output-directory
```

## 21. Native build backend

`se build` 的目標流程：

```text
source/module graph
→ parse
→ check
→ C++20 generation
→ system C++ compiler
→ executable
```

這個設計目前不依賴 LLVM，因此 generated C++ 相對容易檢查，也能直接使用成熟的 C++ compiler toolchain。未來仍可以增加其他 backend，而不必移除 C++ backend。

## 22. CMake / Build system

SE 0.3 project 要求：

```text
CMake 3.20+
C++20
```

Release build：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

測試：

```bash
ctest --test-dir build --output-on-failure
```

Install：

```bash
cmake --install build --prefix "$HOME/.local"
```

目前使用者-facing executable target 為 `se`。安裝時 std data 放入 `share/se/std`。

## 23. Warning policy

GCC / Clang build 使用嚴格 warning：

```text
-Wall
-Wextra
-Wpedantic
-Werror
```

MSVC 對應採 `/W4 /WX`，compiler implementation 自身不應依賴忽略 warning 才能 build。

## 24. Testing strategy

測試除了 unit behavior，也應涵蓋：

```text
Lexer / Parser / Checker
Interpreter
CLI check/run/build
Object/types
Collections
Modules
Errors
Native ABI
Bytes including binary bytes
Native resource cleanup
Binding generator
Interpreter/native parity
```

Interpreter/native parity 的目的，是避免同一份 SE source 在 `se run` 與 `se build` 產生不同語意。

## 25. Compatibility

SE 0.3 正在從舊 S naming 遷移：

```text
Language: S → SE
CLI: s → se
Source: .s → .se
```

內部 namespace/header path 可以暫時保留 `s` 以降低不必要 implementation churn。舊 `.s` source compatibility 也可以在 migration period 保留，但新文件與新程式應以 `.se` 為主。

## 26. 設計原則

SE 的核心原則是：

> Simple at every level.

這不表示所有 advanced feature 都被拿掉，而是要求 complexity 優先存在於 compiler、runtime、standard library 與 native bridge，而不是要求一般使用者反覆處理 punctuation、manual memory、ABI details 或大量樣板程式。