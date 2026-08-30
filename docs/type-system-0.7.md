# SE 0.7 Type Depth: Structured TypeRef

[繁體中文版](type-system-0.7-zh-TW.md)

This versioned document describes the 0.7 type-system stage. It is not a statement that every 0.7 roadmap item is part of the stable 0.6 release.

## Structured type references

Existing annotations remain familiar:

```se
make add a:Int b:Int -> Int
    give a + b
```

Structured TypeRef allows nested type information to remain explicit in the AST/checker:

```text
List[Int]
Set[Text]
Map[Text, User]
List[Map[Text, Int]]
```

## Generic collection annotations

```se
make first[T] values:List[T] -> T
    give values[0]
```

A call can infer `T` from the collection element type and reuse the same relationship for other parameters/results.

## Map keys

At this stage, Map runtime keys remain Text-based, so annotations use forms such as:

```text
Map[Text, Int]
Map[Text, User]
```

Unsupported key-type combinations should be rejected rather than silently degrading to Unknown.

## Recursive TypeRef AST

Conceptually:

```text
TypeRef
├── name
└── args[]
```

For example `List[Map[Text, T]]` is represented structurally, not as one flat string. This lets parser/checker/backend code preserve nested generic positions.

## Compatibility

Untyped functions, explicitly typed functions and generic functions remain valid:

```se
make hello name
    say name

make add a:Int b:Int -> Int
    give a + b

make identity[T] value:T -> T
    give value
```

## Boundary of this stage

Structured TypeRef and nested annotations are a foundation. Generic user types, typed Option/Result integration, richer higher-order collection typing, choice/sum types and deeper exhaustive matching are separate stages unless the implementation revision explicitly includes them.
