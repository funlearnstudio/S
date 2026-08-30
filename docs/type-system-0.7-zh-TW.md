# SE 0.7 Type Depth：Structured TypeRef

[English version](type-system-0.7.md)

這份版本化文件描述 0.7 type-system stage，不代表所有 0.7 Roadmap 項目都屬於 stable 0.6 Release。

## Structured Type Reference

既有 annotation 保持熟悉：

```se
make add a:Int b:Int -> Int
    give a + b
```

Structured TypeRef 讓巢狀型別資訊真正保留在 AST / Checker：

```text
List[Int]
Set[Text]
Map[Text, User]
List[Map[Text, Int]]
```

## Generic Collection Annotation

```se
make first[T] values:List[T] -> T
    give values[0]
```

Call site 可以從 collection element 推斷 `T`，並把同一型別關係延伸到其他 parameter / result。

## Map Key

此 stage 的 Map runtime key 仍以 Text 為主，因此 annotation 使用：

```text
Map[Text, Int]
Map[Text, User]
```

不支援的 key type 應被 Checker 明確拒絕，而不是靜默退化成 Unknown。

## Recursive TypeRef AST

概念結構：

```text
TypeRef
├── name
└── args[]
```

例如 `List[Map[Text, T]]` 會被結構化保存，而不是當成一整段字串。Parser、Checker 與 backend 因此能保留 nested generic position。

## Compatibility

沒有 annotation、明確 typed function 與 generic function 都保持有效：

```se
make hello name
    say name

make add a:Int b:Int -> Int
    give a + b

make identity[T] value:T -> T
    give value
```

## 此階段邊界

Structured TypeRef / nested annotation 是 foundation。Generic user type、typed Option/Result、higher-order collection 精確型別、choice/sum type 與更深入 exhaustive match 屬於其他階段，除非使用中的 implementation revision 已明確提供。
