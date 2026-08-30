# SE Runtime Design

[繁體中文版](runtime-design-zh-TW.md)

The SE runtime turns a simple language surface into checked, managed execution behavior.

## Runtime value families

Important values include:

```text
None
Bool
Int / Num
Text
Bytes
List / Map / Set
Function / Callable
Object
Module
Error
File / Path
Duration / Time
NativeHandle
Task
```

## Managed ownership

Collections, objects, functions, files and native handles use runtime-managed lifetime rules. Ordinary SE code does not directly manipulate raw pointers, `new/delete`, `malloc/free` or C++ ownership primitives.

## Checked operations

Runtime operations validate inputs before performing unsafe host-language actions. Examples include:

- list index bounds
- invalid member access
- wrong function arity
- invalid condition/value types
- division and invalid arithmetic cases
- file/native failures

Failures become SE errors instead of C++ undefined behavior where the runtime can detect them.

## Functions and closures

Functions are runtime values. Nested `make` functions can capture lexical environments, enabling closures and function helpers such as binding/partial application where provided by the standard runtime.

## Objects

User-defined objects keep independent field storage. Method execution can resolve current-object fields without exposing raw object layout to SE source.

## Error values

Recoverable failures carry SE-level information such as message, kind and source location where available. `try / else err` handles these runtime failures; compiler/checker errors remain separate.

## Resources

File and native resources use managed lifetime handling. Opaque native handles may carry registered cleanup functions so cleanup occurs when the managed value is released.

## Tasks and concurrency boundary

Task-style async/await is managed by the runtime. Callbacks that enter the interpreter VM are serialized as required by the implementation; this should not be described as unrestricted parallel execution of arbitrary SE code.

## Platform modules

Features such as JSON, process execution, HTTP/HTTPS, DB adapters and other platform services are exposed primarily through runtime modules instead of adding special syntax for each subsystem.

## Browser runtime

SE Web has a generated browser runtime for browser-side HTTP, navigation, request cancellation and DOM helpers. It is a separate target environment: server filesystem/native/database-secret capabilities do not automatically become browser APIs.

## Design rule

> Keep complexity inside the runtime when that makes ordinary SE code safer and simpler.

The runtime may be internally sophisticated, but user programs should not be forced to manually manage memory, ABI details or host-language boilerplate.
