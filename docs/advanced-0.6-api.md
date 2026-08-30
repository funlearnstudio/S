# SE 0.6 Advanced API Reference

[繁體中文版](advanced-0.6-api-zh-TW.md) · [Concept guide](advanced-0.6.md)

This page is the compact API reference for the 0.6 advanced runtime layer.

## collections

```text
collections.filter list predicate -> List
collections.map list transform -> List
collections.reduce list initial reducer -> value
collections.sort_by list key -> List
collections.sort_by_desc list key -> List
collections.sort_with list comparator -> List
collections.slice list start end -> List
collections.take list count -> List
collections.drop list count -> List
```

These helpers generally return new lists rather than exposing host-container mutation details.

## function

```text
function.bind fn values... -> Function
function.call fn values... -> value
function.pipe value fn... -> value
```

Nested `make` functions also support lexical closures.

## async

```text
async.run fn values... -> Task
async.ready task -> Bool
async.await task -> value
```

Task failure can participate in the recoverable error model. Interpreter VM callbacks are managed/serialized as required by the implementation; this API is not a promise of unrestricted parallel VM execution.

## option

```text
option.some value -> Option
option.none -> Option
option.is_some option -> Bool
option.is_none option -> Bool
option.value option -> value
option.or option fallback -> value
```

Extracting a value from `none` produces an Option-level error.

## result

```text
result.ok value -> Result
result.err text -> Result
result.is_ok result -> Bool
result.is_err result -> Bool
result.value result -> value
result.error result -> Text
result.or result fallback -> value
```

## match helpers

```text
match.value value pattern handler ... fallback -> value
match.option option some_handler none_handler -> value
match.result result ok_handler error_handler -> value
```

The language also has the `match / case / else` statement.

## local database

```text
db.open path -> Database
db.set db key value -> None
db.get db key -> Option
db.has db key -> Bool
db.remove db key -> Bool
db.keys db -> List
db.save db -> None
```

This API is a local persistent Text key/value store, not a relational SQL database.

## HTTPS

```text
https.get url -> Text
https.post url body -> Text
https.post_json url json_text -> Text
```

The 0.6 HTTPS transport uses system `curl` for TLS/certificate handling.

## Generic function syntax

```se
make identity[T] value:T -> T
    give value
```

0.6 generic support is centered on generic functions. Deeper generic user-type work is documented in the 0.7 versioned documents.

## Value match syntax

```se
match status
    case 1
        say "ready"
    case 2
        say "running"
    else
        say "unknown"
```

This stage uses value/equality patterns rather than full destructuring/guards.
