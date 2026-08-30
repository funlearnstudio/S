# SE 0.7 Generic User Types

[繁體中文版](generic-user-types-0.7-zh-TW.md)

This versioned document describes the generic user-type stage built on Structured TypeRef. It is separate from the stable 0.6 generic-function reference.

## Basic syntax

```se
type Box[T]
    value:T

box = Box[Int]
    value = 42
```

`Box[Int]` specializes `T` as `Int`, so `value` is statically checked as `Int`.

## Multiple type parameters

```se
type Pair[A, B]
    first:A
    second:B

pair = Pair[Text, Int]
    first = "SE"
    second = 7
```

Type argument arity is checked statically.

## Typed and required fields

Existing default fields remain valid:

```se
type User
    name = ""
```

Explicit typed fields can use defaults:

```se
type User
    name:Text = ""
```

A typed field without a default is required during object initialization:

```se
type User
    name:Text
    age:Int

user = User
    name = "Milo"
    age = 15
```

## Methods and type parameters

```se
type Box[T]
    value:T

    make get -> T
        give value

    make replace next:T
        value = next
```

For `Box[Int]`, `get` returns `Int` and `replace` accepts `Int`.

## Generic functions over generic user types

```se
make unwrap[T] box:Box[T] -> T
    give box.value
```

Passing `Box[Int]` binds `T` to `Int`, including the return type.

## Nested specialization

Generic substitution can continue through nested user/collection types:

```se
type Wrapper[T]
    item:Box[T]
```

## Runtime model

This stage uses runtime erasure for type parameters: specializations share the runtime type representation while the static checker preserves and enforces specialization information.

## Compatibility

Non-generic user types, default fields, methods and ordinary initialization remain valid. Generic type application uses forms such as `Box[Int]`, keeping type specialization visually distinct from ordinary value indexing.

## Current boundaries

This stage covers invariant generic user types and specialization. Traits/interfaces, variance, generic constraints, higher-kinded types and specialization overloading are not implied unless a later implementation explicitly provides them.
