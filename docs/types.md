# SE Types and Methods

[繁體中文版](types-zh-TW.md)

SE follows a **infer when possible, annotate when useful** approach. The language remains statically checked even when explicit type syntax is omitted.

## Core value families

Common checker/runtime types include:

```text
None
Bool
Int
Num
Text
Bytes
List
Map
Set
Function
Object
Error
Duration
Path / File
Module
NativeHandle
```

## Type inference

```se
age = 15
name = "SE"
active = true
```

The checker can infer obvious types from literals and expressions.

## Typed functions

```se
make add a:Int b:Int -> Int
    give a + b
```

Annotations are useful at API boundaries or when inference is not sufficient.

## Generic functions

```se
make identity[T] value:T -> T
    give value
```

The same generic parameter keeps a consistent type relationship within the call.

## User-defined types

```se
type Player
    name = ""
    hp = 100

    make hit damage
        hp = hp - damage

    make alive
        give hp > 0

player = Player
    name = "Steve"

player.hit 20
say player.hp
```

Field types are inferred from defaults unless explicitly annotated. Assigning an incompatible value is a checker error.

## Method field lookup

Inside a method, an unshadowed field name can resolve to the current object's field:

```se
make hit damage
    hp = hp - damage
```

SE does not require a mandatory `self.` prefix for every field access.

## Object lifetime

Each type construction creates independent field storage. Ordinary SE code does not manually manage object memory or raw pointers; lifetime is managed by the runtime.

## Collection typing

List/Map/Set values preserve type information where the checker can infer it. Some platform or heterogeneous data paths may produce broader/unknown types when an exact static type is unavailable.

## Unknown

`Unknown` represents data whose precise type cannot be determined statically, such as heterogeneous JSON or some dynamic platform APIs. It does not disable checking globally; operations may still require more specific information.

## Option and Result

SE also provides managed Option/Result helpers for explicit value-level absence or success/failure.

```se
use option
maybe = option.some "SE"

use result
answer = result.ok 42
```

## Versioned generic user-type work

Generic functions are part of the established language. Generic user types and deeper type-system work are documented separately in versioned design/reference documents so future work is not confused with the stable release.
