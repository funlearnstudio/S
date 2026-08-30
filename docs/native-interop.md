# SE Native Interoperability

[繁體中文版](native-interop-zh-TW.md)

SE uses a C ABI bridge so ordinary SE source can call native C/C++ libraries without exposing arbitrary C++ ABI details.

## Importing a native module

```se
use native_test

say add 20 22
say square 4.0
```

Normal SE code uses the same `use` syntax as source modules.

## `.snative` metadata

A native module description can define the library, exported symbol names, argument/result types and optional cleanup/error behavior.

```text
library native_test
add s_test_add Int Int -> Int
square s_test_square Num -> Num
bytes_len s_test_bytes_len Bytes -> Int
```

## Supported ABI value families

The bridge supports common values including:

```text
Int
Num
Bool
Text
Bytes
None
managed opaque handles
```

## Bytes ownership

Binary data uses a pointer + size view at the C ABI boundary rather than C-string semantics. This allows embedded zero bytes.

A buffer passed from SE to native code is borrowed for the duration of the call. When native code returns a byte view, the runtime copies it into SE-owned storage before returning control to SE code. This keeps ordinary buffer lifetime management inside the runtime.

## Managed opaque handles

Native resources can be represented by a named handle with a cleanup function:

```text
make_counter s_test_counter_create -> Handle:Counter cleanup s_test_counter_destroy
counter_value s_test_counter_value Handle:Counter -> Int
```

SE code receives a managed handle rather than a raw address. The registered cleanup function runs when the managed resource is released.

## Fallible native calls

Native declarations can participate in SE's recoverable error model where supported. A native failure becomes an SE-level error that can be handled with `try` rather than crashing the interpreter.

## Binding generator

Describe a simple API in `.sbind`:

```text
module mathx
library mathx
header mathx.hpp

add math_add Int Int -> Int
sqrt math_sqrt Num -> Num
copy math_copy Bytes -> Bytes
```

Generate bindings:

```bash
se bind mathx.sbind generated
```

Typical outputs include:

```text
generated/mathx.snative
generated/mathx_bindings.h
generated/mathx_bindings.cpp
```

Unsupported signatures should be rejected explicitly instead of generating incomplete wrappers.

## Recommended boundary

```text
C++ library
    ↓
C ABI wrapper
    ↓
.snative metadata
    ↓
SE module
```

SE does not attempt to expose arbitrary C++ templates, class ABI layouts, multiple inheritance or operator-overload internals directly.

## Safety boundary

Native interoperability is a deliberate systems-level escape hatch. SE can manage its side of ownership and error conversion, but it cannot guarantee that an external native library itself is memory-safe. Keep native boundaries small, explicit and tested.
