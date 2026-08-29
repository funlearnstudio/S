# S 0.2.0

S 0.2.0 expands the existing S 0.1 language and compiler without breaking S 0.1 syntax compatibility.

## Core language

- User-defined `type` declarations with fields and methods.
- Modules and `use` imports.
- List, Map, and Set collection support.
- Recoverable errors with `try`, `fail`, and structured Error values.
- File, path, time, and Duration runtime support.

## Native interoperability

- C ABI support for Int, Num, Bool, Text, and Bytes.
- Binary-safe byte buffers through `SBytesView` with S-owned lifetime semantics.
- Native errors mapped to S `NativeError`.
- Opaque native handles with automatic deleters and resource cleanup.
- `s bind` baseline binding generator producing `.snative` metadata, C declarations, and C++ wrappers.

## Validation

The release candidate was validated on Ubuntu GCC and macOS AppleClang with warnings treated as errors (`-Wall -Wextra -Wpedantic -Werror`).

The final validation suite includes:

- S 0.1 regression coverage.
- S 0.2 core tests.
- 11/11 CTest passing on Ubuntu and macOS.
- `s check`, `s run`, and `s build` smoke tests.
- Interpreter/native output parity.
- C ABI scalar, Text, and Bytes tests.
- Binary Bytes regression using non-text bytes including `0x00` and `0xFF`.
- Opaque-handle automatic cleanup tests.
- Binding-generator end-to-end generation, compilation, S execution, native build, and parity validation.

## Version

S 0.2.0
