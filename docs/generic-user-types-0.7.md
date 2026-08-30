# SE 0.7 Generic User Types

SE 0.7 extends the type-depth foundation with generic user-defined types while keeping the ordinary `type` model small.

## Basic syntax

```se
type Box[T]
    value:T

box = Box[Int]
    value = 42
```

`Box[Int]` specializes `T` as `Int`, so `value` is statically `Int`.

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

Existing default-value fields remain valid:

```se
type User
    name = ""
```

Explicit annotations may be combined with defaults:

```se
type User
    name:Text = ""
```

A typed field without a default is required during indented initialization:

```se
type User
    name:Text

user = User
    name = "Milo"
```

## Generic methods

An enclosing type parameter is available to methods:

```se
type Box[T]
    value:T

    make get -> T
        give value

    make replace next:T
        value = next
```

For `Box[Int]`, `get` returns `Int` and `replace` accepts `Int`.

## Generic functions over user types

```se
make unwrap[T] box:Box[T] -> T
    give box.value
```

Passing a `Box[Int]` binds `T` to `Int`, including the return type.

## Nested specialization

User types may contain specialized user types:

```se
type Wrapper[T]
    item:Box[T]
```

Substitution also continues recursively through List, Set, and Map types.

## Compatibility and runtime model

Existing non-generic SE 0.6 user types remain source-compatible. Generic type construction uses PascalCase type names such as `Box[Int]`, which also keeps generic type application distinguishable from normal collection indexing.

Generic user types are runtime-erased in this phase. `Box[Int]` and `Box[Text]` share the same runtime `TypeData`; their specialization is retained and enforced by the static checker.

## Current boundary

This phase implements invariant generic user types and specialization. It does not yet add traits/interfaces, variance, generic constraints, higher-kinded types, or specialization overloading.
