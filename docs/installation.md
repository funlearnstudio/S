# SE Installation / SE 安裝說明

SE 0.6.0 provides prebuilt packages for normal users. You can install and run SE without cloning the repository and without installing Git, CMake, or a C++ compiler.

SE 0.6.0 提供已預先編譯完成的安裝包。一般使用者不需要 Clone 原始碼，也不需要另外安裝 Git、CMake 或 C++ 編譯器，就可以直接安裝與執行 SE。

> Current release / 目前版本：`SE 0.6.0`

---

## English

### Quick install

macOS / Linux:

```bash
curl -fsSL https://raw.githubusercontent.com/funlearnstudio/SE/main/install.sh | sh
```

Windows PowerShell:

```powershell
irm https://raw.githubusercontent.com/funlearnstudio/SE/main/install.ps1 | iex
```

Verify:

```bash
se --version
se doctor
```

The installer detects your platform, downloads the matching package from GitHub Releases, installs it into your user account, and makes the `se` command available through your user `PATH`.

### What does not require a compiler?

These commands work with the prebuilt SE package and do not require CMake or a C++ compiler:

```bash
se
se run hello.se
se check hello.se
se check-all .
se test .
se new app myapp
se new web mysite
```

### Native build exception

`se build` currently uses SE's C++20 backend and invokes a system C++ compiler to produce a standalone native executable:

```bash
se build hello.se
```

Therefore `se build` still requires a C++20 compiler. This is separate from installing and running SE itself. CMake is not required for normal installed use.

### Direct downloads

GitHub Releases publishes:

- `se-0.6.0-macos-arm64.tar.gz` for Apple Silicon Macs
- `se-0.6.0-macos-x64.tar.gz` for Intel Macs
- `se-0.6.0-linux-x64.tar.gz` for Linux x64
- `se-0.6.0-windows-x64.zip` for Windows x64
- `SHA256SUMS.txt` for checksum verification

### First SE program

Create `hello.se`:

```se
say "Hello SE"
```

Run it:

```bash
se run hello.se
```

### Update SE

Running the installer again replaces the installed copy of the same version. Future installers can also select another release with `SE_VERSION`.

macOS / Linux example:

```bash
SE_VERSION=0.6.0 curl -fsSL https://raw.githubusercontent.com/funlearnstudio/SE/main/install.sh | sh
```

On Windows, set `$env:SE_VERSION` before running `install.ps1` when installing a specific supported release.

### Build SE itself from source

Contributors who want to build the SE implementation need:

- Git
- CMake 3.20+
- a C++20 compiler

```bash
git clone https://github.com/funlearnstudio/SE.git
cd SE
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Windows uses the equivalent Release multi-config CMake commands.

### Troubleshooting

macOS / Linux:

```bash
which se
se --version
```

If `$HOME/.local/bin` is not in `PATH`, add this to `~/.zshrc` or `~/.bashrc`:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

Windows PowerShell:

```powershell
Get-Command se
se --version
```

The Windows installer adds its user-level SE bin directory to `PATH`; open a new terminal if the current terminal does not see it yet.

The SE HTTPS module currently uses system `curl`, so HTTPS features require `curl` even though the SE interpreter itself does not.

---

## 中文

### 快速安裝

macOS / Linux：

```bash
curl -fsSL https://raw.githubusercontent.com/funlearnstudio/SE/main/install.sh | sh
```

Windows PowerShell：

```powershell
irm https://raw.githubusercontent.com/funlearnstudio/SE/main/install.ps1 | iex
```

安裝完成後：

```bash
se --version
se doctor
```

安裝器會自動偵測作業系統與 CPU 架構，從 GitHub Releases 下載正確的預編譯 SE，安裝到使用者目錄，並讓 `se` 指令可以直接使用。

### 哪些功能完全不需要 C++ / CMake？

一般使用 SE 時，以下功能都直接由下載好的 SE 執行，不需要 CMake，也不需要 C++ 編譯器：

```bash
se
se run hello.se
se check hello.se
se check-all .
se test .
se new app myapp
se new web mysite
```

所以初學者下載 SE 之後，就可以直接開始寫 `.se` 程式。

### `se build` 是目前唯一的例外

目前 `se build` 會經過 SE 的 C++20 Backend，再呼叫系統 C++ 編譯器產生真正的 Native Executable：

```bash
se build hello.se
```

因此如果要使用 `se build`，目前仍需要 C++20 編譯器。這不影響 SE 本身的安裝、REPL、執行、檢查或測試；一般使用完全不需要先安裝 C++ 或 CMake。

### 可以直接下載的版本

GitHub Releases 會提供：

- `se-0.6.0-macos-arm64.tar.gz`：Apple Silicon Mac
- `se-0.6.0-macos-x64.tar.gz`：Intel Mac
- `se-0.6.0-linux-x64.tar.gz`：Linux x64
- `se-0.6.0-windows-x64.zip`：Windows x64
- `SHA256SUMS.txt`：SHA-256 檢查碼

### 第一個 SE 程式

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

### 更新 SE

之後可以再次執行安裝器來更新或重新安裝。安裝器也支援用 `SE_VERSION` 指定版本。

### 如果你要開發 SE 本身

只有要修改、編譯 SE Compiler / Runtime 本身的人才需要：

- Git
- CMake 3.20+
- C++20 編譯器

```bash
git clone https://github.com/funlearnstudio/SE.git
cd SE
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### 常見問題

macOS / Linux：

```bash
which se
se --version
```

如果 `$HOME/.local/bin` 不在 `PATH`，將下面這行加入 `~/.zshrc` 或 `~/.bashrc`：

```bash
export PATH="$HOME/.local/bin:$PATH"
```

Windows 安裝器會把 SE 的使用者級 `bin` 目錄加入 `PATH`；如果目前的 Terminal 還找不到 `se`，重新開一個 Terminal 即可。

SE 的 HTTPS 模組目前仍使用系統 `curl`，所以只有使用 HTTPS 功能時需要 `curl`；SE Interpreter 本身不依賴它。

---

## Next steps / 下一步

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
