# Native interoperability

S 0.2 starts with a C ABI bridge. Ordinary S code imports a native module the same way as a source module:

```text
use native_test

say add 20 22
say square 4.0
```

A `.snative` binding file is intentionally separate from normal S source:

```text
library native_test
add s_test_add Int Int -> Int
square s_test_square Num -> Num
```

The S 0.2 runtime directly supports common homogeneous C ABI signatures for Int, Num, Bool, and Text, plus opaque handles. A handle is never exposed as a raw address. It owns a native resource through a managed deleter and keeps the shared library loaded while the resource exists.

A native declaration may mark an operation `fallible`; a negative C integer result is converted into an S `NativeError` and must be handled with `try`.

For C++ libraries, the supported 0.2 path is:

```text
C++ library
    -> small extern-C wrapper
    -> .snative metadata
    -> S module
```

S 0.2 deliberately does not attempt to expose arbitrary C++ templates, class ABIs, multiple inheritance, operator overloads, or raw pointers. Mixed/complex signatures should use a small C wrapper. The in-repository `native_test` shared library is the integration reference.
