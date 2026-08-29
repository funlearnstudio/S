# SE 快速開始

本文件是 `getting-started.md` 的繁體中文版。

## 1. 建置 SE

需求：CMake 3.20+ 與支援 C++20 的編譯器。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

macOS / Linux 安裝到使用者目錄：

```bash
cmake --install build --prefix "$HOME/.local"
export PATH="$HOME/.local/bin:$PATH"
se --version
```

## 2. 第一支程式

建立 `hello.se`：

```se
say "Hello SE"
```

執行：

```bash
se run hello.se
```

檢查：

```bash
se check hello.se
```

編譯：

```bash
se build hello.se
./hello
```

## 3. 建立專案

一般 App：

```bash
se new app myapp
cd myapp
se check-all .
se test .
se run src/main.se
```

Web 專案：

```bash
se new web mysite
cd mysite
se test backend
se run backend/main.se
```

## 4. REPL

直接輸入：

```bash
se
```

即可進入互動模式。

## 5. 下一步

推薦依序閱讀：

1. `tutorial-zh-TW.md`
2. `complete-language-guide-zh-TW.md`
3. `technical-reference-zh-TW.md`
4. `advanced-0.6-zh-TW.md`

SE 的新原始碼使用 `.se`；舊 `.s` 目前仍保留相容性。
