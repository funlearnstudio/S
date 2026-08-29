# SE 教學（繁體中文）

SE 的目標是讓程式從入門到進階都維持簡單、低標點、可讀，而且把複雜度留在 compiler/runtime 裡。SE 0.3 的主要原始碼副檔名是 `.se`，命令列工具是 `se`。

## 1. 安裝與確認

從原始碼安裝需要 CMake 3.20+ 與 C++20 compiler。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
cmake --install build --prefix "$HOME/.local"
```

macOS/Linux 若 `se` 找不到，確認 PATH：

```bash
export PATH="$HOME/.local/bin:$PATH"
se --version
```

正常應顯示：

```text
SE 0.3.0
```

## 2. 第一個程式

建立 `hello.se`：

```se
say "Hello SE"
```

執行：

```bash
se run hello.se
```

只檢查語法與型別：

```bash
se check hello.se
```

編譯為 native executable：

```bash
se build hello.se
./hello
```

## 3. 變數與基本值

SE 會從值推斷型別：

```se
name = "Steve"
age = 15
score = 98.5
ready = true
```

常用內建值類型包括 `Int`、`Num`、`Bool`、`Text`、`Bytes`、collections，以及使用者自訂 type。

## 4. 輸入與輸出

```se
name = ask "What is your name?"
say "Hello " + name
```

`ask` 取得輸入，`say` 輸出值。

## 5. 數學、比較與邏輯

```se
a = 10
b = 3

say a + b
say a - b
say a * b
say a / b
say a % b
say a ** 2

say a > b
say a == b
say a != b
```

SE 支援 `and`、`or`、`not`，並使用一般 expression precedence。

## 6. if / else

SE 用縮排形成 block，不使用 `{}`：

```se
age = 15

if age >= 15
    say "yes"
else
    say "no"
```

## 7. 迴圈

固定次數：

```se
repeat 3
    say "Hi"
```

遍歷資料：

```se
nums = [1, 2, 3]

for n in nums
    say n
```

條件迴圈：

```se
n = 0

while n < 3
    say n
    n = n + 1
```

Range：

```se
for n in 1..10
    say n
```

## 8. 函式

函式使用 `make`，回傳使用 `give`：

```se
make add a b
    give a + b

answer = add 5 3
say answer
```

SE 的呼叫語法刻意減少不必要括號。

## 9. List、Map、Set

List：

```se
nums = [1, 2, 3]
nums.add 4
nums.remove 2
say nums.len
```

SE 會在 runtime 檢查 List index，避免越界造成 undefined behavior。

Map：

```se
user = ["name": "Steve", "age": "15"]
say user["name"]

for key value in user
    say key
    say value
```

Set：

```se
values = set [1, 2, 2, 3]
values.add 4

if 3 in values
    say "found"
```

## 10. Text 與 Bytes

Text 常用 member：

```se
text = "Hello"
say text.len
say text.upper
say text.lower
```

Bytes：

```se
data = bytes "hello"
say data.len
```

Bytes 是 binary-safe 的 byte buffer，可用於 native interoperability。

## 11. 自訂 Type 與 Method

```se
type Player
    name = ""
    hp = 100

    make hit damage
        hp = hp - damage

    make alive
        give hp > 0

player = Player
    name = "Steve"

player.hit 20
say player.name
say player.hp
say player.alive
```

Field 型別由預設值推斷。Method 內可直接使用 field 名稱，不必強制寫 `self`。

## 12. Modules

`player.se`：

```se
type Player
    hp = 100
```

`main.se`：

```se
use player

p = Player
say p.hp
```

SE 0.3 會優先搜尋 `.se` module；為遷移相容，也可搜尋舊 `.s`。以下底線開頭的 top-level 名稱視為 module-private：

```se
_secret = 123
```

Circular imports 會被拒絕並顯示 dependency chain。

## 13. 錯誤處理

可恢復錯誤：

```se
try
    text = read "hello.txt"
    say text
else err
    say err.kind
    say err.message
```

建立自己的錯誤：

```se
make check score
    if score < 0
        fail "Invalid score"
    give score
```

函式內可以使用 `try expr` 把 recoverable error 往呼叫端傳遞：

```se
make load
    give try read "hello.txt"
```

## 14. Files、Path、Time

File：

```se
try
    write "hello.txt" "Hello"
    append "hello.txt" " SE"
    text = read "hello.txt"
    say text
else err
    say err
```

Path：

```se
use path

file = path.join "data" "user.txt"
say path.name file
say path.ext file
say path.exists file
```

Time：

```se
use time

say time.now
wait 500ms
wait 2s
wait 1min
```

## 15. REPL

直接執行：

```bash
se
```

然後：

```text
> say "Hello"
Hello
```

縮排 block 在 REPL 中以空白行完成。

## 16. Native interoperability

一般 SE 程式仍然只需要：

```se
use native_test
say add 20 22
```

`.snative` 描述 native C ABI：

```text
library native_test
add s_test_add Int Int -> Int
bytes_len s_test_bytes_len Bytes -> Int
```

SE 的 native bridge 支援 `Int`、`Num`、`Bool`、`Text`、`Bytes`、`None` 與 opaque managed handles。Native resource 可以指定 cleanup function，由 runtime 管理生命週期。

Binding generator：

```bash
se bind mathx.sbind generated
```

會產生 `.snative`、C header 與 C++ wrapper。

## 17. 建議學習順序

第一次學 SE 時，可以照這個順序：輸出與變數 → `if` → loop → function → List/Map/Set → type/method → module → error → file/path/time → native interoperability。

SE 的設計原則不是把功能拿掉，而是讓功能存在時仍保持少樣板、低標點與可理解的行為。