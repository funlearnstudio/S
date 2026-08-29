# Native interoperability

S 0.2 uses a C ABI bridge. Normal S code imports native modules with the same `use` syntax as source modules:

```text
use native_test

say add 20 22
say square 4.0
```

Native metadata lives in a `.snative` file:

```text
library native_test
add s_test_add Int Int -> Int
square s_test_square Num -> Num
bytes_len s_test_bytes_len Bytes -> Int
```

## Supported values

The S 0.2 bridge supports `Int`, `Num`, `Bool`, `Text`, `Bytes`, `None`, and opaque native handles.

### Bytes

S owns byte buffers. A buffer can be created from text with:

```text
data = bytes "hello"
say data.len
```

The public C header `s/native.h` defines the ABI view:

```c
typedef struct SBytesView {
    const uint8_t* data;
    size_t size;
} SBytesView;
```

A buffer passed to C is borrowed for the duration of the call. When C returns an `SBytesView`, S copies its contents into a new S-owned buffer before returning to S code. This keeps buffer lifetime management inside the runtime.

### Opaque handles

Native resources can be represented by a named handle with an automatic cleanup function:

```text
make_counter s_test_counter_create -> Handle:Counter cleanup s_test_counter_destroy
counter_value s_test_counter_value Handle:Counter -> Int
```

S code receives a managed handle rather than an address. When the managed resource is released, the registered cleanup function runs once.

### Native errors

A native declaration can be marked `fallible`. A negative integer result becomes an S `NativeError` and can be handled with `try`:

```text
try
    value = native_error 1
else err
    say err.kind
```

## Binding generator

For simple APIs, S can generate a C ABI wrapper and metadata from a `.sbind` definition:

```text
module mathx
library mathx
header mathx.hpp

add math_add Int Int -> Int
sqrt math_sqrt Num -> Num
copy math_copy Bytes -> Bytes
```

Run:

```bash
s bind mathx.sbind generated
```

It generates:

```text
generated/mathx.snative
generated/mathx_bindings.h
generated/mathx_bindings.cpp
```

The S 0.2 generator supports `Int`, `Num`, `Bool`, `Text`, `Bytes`, and `None`. Unsupported definitions are rejected with an error rather than producing an incomplete wrapper.

For C++ libraries, the recommended path is:

```text
C++ library
    -> generated or handwritten C ABI wrapper
    -> .snative metadata
    -> S module
```

S 0.2 does not attempt to expose arbitrary C++ templates, class ABI details, multiple inheritance, or operator overloads directly.
