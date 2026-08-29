# 測試系統

本文件是 `testing.md` 的繁體中文版。

SE 內建專案測試流程，不需要先安裝第三方測試框架。

## 測試檔命名

`se test` 會遞迴尋找：

```text
*_test.se
*_test.s
```

新的測試建議使用 `.se`。

## test module

```se
use test

test.ok true
test.equal 2 + 2 4
test.not_equal 1 2
```

也可以主動失敗：

```se
test.fail "not implemented"
```

Assertion failure 會以 `AssertionError` 結束該測試。

## 執行測試

```bash
se test .
```

或指定資料夾：

```bash
se test backend
```

測試流程會先做 static check，再執行測試程式。

## 全專案檢查

```bash
se check-all .
```

`check-all` 會檢查找到的 SE source，適合在 commit 或 CI 前快速驗證整個專案。

## Web route 測試

Web router 可以不真的開 port 就測試：

```se
use web
use test

make home body
    give "hello"

web.get "/" home
body = web.handle "GET" "/" ""
status = web.handle_status "GET" "/" ""

test.equal body "hello"
test.equal status 200
```

## 專案內部測試

SE 本身使用 CTest、CLI tests、interpreter/native parity、FFI Bytes、bindgen、generics 與 advanced native smoke tests 驗證 compiler/runtime 行為。跨平台 CI 目前涵蓋 Ubuntu、macOS、Windows。
