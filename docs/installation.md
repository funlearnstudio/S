# Installing SE

SE release archives contain `bin/se` (or `se.exe`) and the compiler support files under `share/se`. Keep that directory structure intact so `se build` can find the runtime sources it embeds into native executables.

## macOS and Linux

Run `scripts/install.sh`, or download the matching release archive and place it under `~/.local/share/se`. The installer creates `~/.local/bin/se` and adds `~/.local/bin` to PATH when needed.

Uninstall with:

```bash
curl -fsSL https://raw.githubusercontent.com/funlearnstudio/S/main/scripts/uninstall.sh | sh
```

## Windows

Run `scripts/install.ps1`. It installs SE under `%LOCALAPPDATA%\\SE`, adds `%LOCALAPPDATA%\\SE\\bin` to the user PATH, and leaves system PATH untouched.

Uninstall with `scripts/uninstall.ps1`.

## Native builds

`se run` and `se check` use the bundled SE runtime. `se build` also invokes a C++20 compiler. On macOS/Linux it uses `$CXX` or `c++`. On Windows it uses `$env:CXX` when set, otherwise MSVC `cl`. Install a C++20 toolchain if you want native compilation.

## Environment variables

- `SE_HOME`: override the installed SE support-file root.
- `SE_NATIVE_PATH`: additional native-module library search directories.
- `CXX`: choose the C++ compiler used by `se build`.

Legacy `S_HOME` and `S_NATIVE_PATH` remain recognized during migration.
