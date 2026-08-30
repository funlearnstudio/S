# SE 測試與專案檢查

[English version](testing.md)

SE 提供 project-wide static checking 與輕量 test runner，讓大型專案不需要一個檔案一個檔案手動驗證。

## 檢查單一檔案

```bash
se check app.se
```

## 檢查整個 Source Tree

```bash
se check-all .
```

也可以只檢查子目錄：

```bash
se check-all backend
```

指令會遞迴尋找 SE source、略過常見 generated/dependency directory、輸出 pass/fail，並在失敗時回傳 non-zero exit code，因此可用在 CI。

## Test File

Test file 慣例使用 `_test.se`：

```se
use test

test.equal 4 2 + 2
test.ok true
```

簡單測試也可以在條件錯誤時直接使用 `fail`。

## 執行 Tests

```bash
se test .
```

Test runner 會遞迴找 test file、檢查並執行、輸出 summary；任一 test 失敗時回傳 non-zero exit code。

## Project Scaffolding

`se new app NAME` 會建立包含 source/test 的 application structure；`se new web NAME` 建立目前 CLI 支援的 Web/project scaffold。

## Compiler Regression Tests

SE implementation 本身另外使用 CMake / CTest 做較底層 regression：

```bash
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Implementation-level coverage 包含 Lexer/Parser/Checker、Interpreter、Native backend parity、Native ABI/Bytes/resource lifetime、Web build 與 CLI smoke test。

## CI 原則

Compiler 能成功 build 不代表語言變更就已驗證。Language change 應加入對應 regression test，證明預期的 SE behavior 真的能工作。
