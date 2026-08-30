# SE Native Interoperability

[English version](native-interop.md)

SE 使用 C ABI bridge，讓一般 SE source 可以呼叫原生 C/C++ library，而不用直接暴露任意 C++ ABI detail。

## Import Native Module

```se
use native_test

say add 20 22
say square 4.0
```

一般 SE code 和 source module 一樣使用 `use`。

## `.snative` Metadata

Native module description 可以描述 library、exported symbol、argument/result type，以及需要時的 cleanup / error behavior。

```text
library native_test
add s_test_add Int Int -> Int
square s_test_square Num -> Num
bytes_len s_test_bytes_len Bytes -> Int
```

## 支援的 ABI Value Family

目前 bridge 涵蓋常見值：

```text
Int
Num
Bool
Text
Bytes
None
managed opaque handles
```

## Bytes Ownership

Binary data 在 C ABI boundary 使用 pointer + size view，而不是 C-string semantics，因此可以正確處理內含 `0x00` 的資料。

SE 傳給 native code 的 buffer 在 call 期間是 borrowed view；native 回傳 byte view 後，Runtime 會 copy 成 SE-owned storage，再交回 SE code。一般使用者不需要自己處理 buffer lifetime。

## Managed Opaque Handle

Native resource 可以使用 named handle + cleanup function：

```text
make_counter s_test_counter_create -> Handle:Counter cleanup s_test_counter_destroy
counter_value s_test_counter_value Handle:Counter -> Int
```

SE 收到 managed handle，而不是 raw address。Managed resource 被釋放時，Runtime 執行已註冊的 cleanup function。

## Fallible Native Call

在支援的 declaration 中，native failure 可以進入 SE recoverable error model，讓使用者用 `try` 處理，而不是讓 Interpreter 因 native failure 直接崩潰。

## Binding Generator

`.sbind`：

```text
module mathx
library mathx
header mathx.hpp

add math_add Int Int -> Int
sqrt math_sqrt Num -> Num
copy math_copy Bytes -> Bytes
```

執行：

```bash
se bind mathx.sbind generated
```

常見輸出：

```text
generated/mathx.snative
generated/mathx_bindings.h
generated/mathx_bindings.cpp
```

遇到不支援的 signature 應明確報錯，而不是產生不完整 wrapper。

## 建議邊界

```text
C++ library
    ↓
C ABI wrapper
    ↓
.snative metadata
    ↓
SE module
```

SE 不直接暴露任意 C++ template、class ABI layout、multiple inheritance 或 operator-overload internals。

## Safety Boundary

Native interop 是刻意提供的 systems-level escape hatch。SE 可以管理自己這一側的 ownership 與 error conversion，但無法保證外部 native library 本身一定 memory-safe，因此 native boundary 應保持小、明確且有測試。
