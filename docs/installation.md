# SE Installation / SE 安裝說明

This guide explains how to build and install SE from source on macOS, Linux and Windows.

本文件說明如何在 macOS、Linux 與 Windows 從原始碼編譯並安裝 SE。

> Current development version / 目前開發版本：`SE 0.6.0-dev`

---

## English

### 1. Requirements

Install the following first:

- Git
- CMake 3.20 or newer
- a C++20-capable compiler
- `curl` if you want to use SE's HTTPS module

Typical compilers:

- macOS: Apple Clang from Xcode Command Line Tools
- Linux: GCC or Clang with C++20 support
- Windows: Visual Studio 2022 / MSVC with C++ desktop development tools

### 2. Download SE

```bash
git clone https://github.com/funlearnstudio/SE.git
cd SE
```

### 3. Configure

macOS / Linux:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Windows:

```powershell
cmake -S . -B build
```

### 4. Build

macOS / Linux:

```bash
cmake --build build --parallel
```

Windows:

```powershell
cmake --build build --config Release
```

### 5. Run the test suite

macOS / Linux:

```bash
ctest --test-dir build --output-on-failure
```

Windows:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

It is recommended to run the tests before installation so you know the local compiler/runtime build is working correctly.

### 6. Install on macOS / Linux

Install into your user directory:

```bash
cmake --install build --prefix "$HOME/.local"
```

Make the `se` command available in the current terminal:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

Verify:

```bash
se --version
se doctor
```

To make the PATH change permanent, add this line to `~/.zshrc` or `~/.bashrc`:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

For zsh, reload it with:

```bash
source ~/.zshrc
```

For bash:

```bash
source ~/.bashrc
```

### 7. Install on Windows

From a Visual Studio / MSVC terminal:

```powershell
cmake --install build --config Release --prefix "$env:USERPROFILE\.local"
```

The executable will be installed under the selected prefix. Add its `bin` directory to the Windows `PATH`, then open a new terminal and run:

```powershell
se --version
se doctor
```

### 8. First SE program

Create a file named `hello.se`:

```se
say "Hello SE"
```

Run it:

```bash
se run hello.se
```

Static-check it:

```bash
se check hello.se
```

Build a native executable:

```bash
se build hello.se
```

### 9. Update SE later

Inside your SE source directory:

```bash
git checkout main
git pull
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build --prefix "$HOME/.local"
```

On Windows, use the corresponding Release multi-config commands instead of the Unix commands above.

### 10. Troubleshooting

If `se` is not found after installation, check that your install `bin` directory is in `PATH`.

macOS / Linux:

```bash
echo "$PATH"
which se
```

Windows PowerShell:

```powershell
$env:PATH
Get-Command se
```

If configuration fails, verify CMake and your compiler:

```bash
cmake --version
c++ --version
```

If HTTPS calls fail, verify that `curl` is installed and available:

```bash
curl --version
```

---

## 中文

### 1. 安裝需求

請先準備：

- Git
- CMake 3.20 以上
- 支援 C++20 的編譯器
- 如果要使用 SE 的 HTTPS 模組，需要 `curl`

常見環境：

- macOS：Xcode Command Line Tools 提供的 Apple Clang
- Linux：支援 C++20 的 GCC 或 Clang
- Windows：Visual Studio 2022 / MSVC，並安裝 C++ Desktop Development 工具

### 2. 下載 SE

```bash
git clone https://github.com/funlearnstudio/SE.git
cd SE
```

### 3. 設定編譯環境

macOS / Linux：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Windows：

```powershell
cmake -S . -B build
```

### 4. 編譯

macOS / Linux：

```bash
cmake --build build --parallel
```

Windows：

```powershell
cmake --build build --config Release
```

### 5. 執行測試

macOS / Linux：

```bash
ctest --test-dir build --output-on-failure
```

Windows：

```powershell
ctest --test-dir build -C Release --output-on-failure
```

建議在正式安裝前先跑測試，確認你電腦上的編譯器與 SE Runtime 可以正常工作。

### 6. macOS / Linux 安裝

安裝到自己的使用者目錄：

```bash
cmake --install build --prefix "$HOME/.local"
```

讓目前這個 Terminal 可以找到 `se`：

```bash
export PATH="$HOME/.local/bin:$PATH"
```

確認是否安裝成功：

```bash
se --version
se doctor
```

如果希望重新打開 Terminal 之後仍可以使用 `se`，把下面這行加入 `~/.zshrc` 或 `~/.bashrc`：

```bash
export PATH="$HOME/.local/bin:$PATH"
```

使用 zsh 時重新載入：

```bash
source ~/.zshrc
```

使用 bash：

```bash
source ~/.bashrc
```

### 7. Windows 安裝

在 Visual Studio / MSVC Terminal 中執行：

```powershell
cmake --install build --config Release --prefix "$env:USERPROFILE\.local"
```

安裝完成後，把安裝位置中的 `bin` 資料夾加入 Windows `PATH`，重新開啟 Terminal，再執行：

```powershell
se --version
se doctor
```

### 8. 第一個 SE 程式

建立 `hello.se`：

```se
say "Hello SE"
```

直接執行：

```bash
se run hello.se
```

只做靜態檢查：

```bash
se check hello.se
```

編譯成 Native executable：

```bash
se build hello.se
```

### 9. 之後更新 SE

進入 SE 原始碼資料夾：

```bash
git checkout main
git pull
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build --prefix "$HOME/.local"
```

Windows 則使用前面對應的 Release multi-config 指令。

### 10. 常見問題

如果安裝完成後出現 `se: command not found`，通常是安裝位置的 `bin` 還沒有加入 `PATH`。

macOS / Linux 可檢查：

```bash
echo "$PATH"
which se
```

Windows PowerShell：

```powershell
$env:PATH
Get-Command se
```

如果 CMake 設定失敗，先確認版本與編譯器：

```bash
cmake --version
c++ --version
```

如果 HTTPS 功能不能使用，確認系統有 `curl`：

```bash
curl --version
```

---

## Next steps / 下一步

After installation / 安裝完成後：

```bash
se help
se doctor
se new app myapp
se new web mysite
```

Then read / 接著可以閱讀：

- [Getting Started](getting-started.md)
- [SE 完整語言教學](complete-language-guide-zh-TW.md)
- [SE 技術文件](technical-reference-zh-TW.md)
- [SE 0.6 進階功能](advanced-0.6-zh-TW.md)
