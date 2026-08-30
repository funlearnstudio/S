# SE 0.6 Advanced Guide

[繁體中文版](advanced-0.6-zh-TW.md)

SE 0.6 extends the language with higher-order collection helpers, lexical closures, generic functions, value-based matching, managed Tasks, Option/Result helpers, a lightweight persistent database and HTTPS support while keeping the same low-punctuation model.

> This is a versioned guide. Generic functions are established here; a complete generic user-type/trait system is a separate later stage. HTTPS currently uses system `curl`, and the built-in database is a lightweight text key/value store rather than a replacement for a relational database.

## Higher-order collections

```se
use collections

filtered = collections.filter nums predicate
mapped = collections.map nums transform
total = collections.reduce nums 0 add
part = collections.slice nums 1 4
first = collections.take nums 2
rest = collections.drop nums 2
```

Sorting helpers include `sort_by`, `sort_by_desc`, and callback-based `sort_with`.

## Closures and function values

```se
make make_adder base
    make inner value
        give base + value
    give inner

add10 = make_adder 10
say add10 5
```

Nested `make` captures the lexical environment. The `function` module can also provide helpers such as binding/partial application and pipelines.

## Generic functions

```se
make identity[T] value:T -> T
    give value

number = identity 42
text = identity "SE"
```

The checker binds generic parameters at call sites and keeps repeated uses of the same type parameter consistent.

## Value-based match

```se
match value
    case 1
        say "one"
    case 2
        say "two"
    else
        say "other"
```

This stage uses equality/value matching. Destructuring, guards and full exhaustiveness belong to deeper later type/match work unless implemented by the revision in use.

## Option

```se
use option

name = option.some "Steve"
if option.is_some name
    say option.value name
```

Helpers cover some/none checks, value extraction and fallback behavior.

## Result

```se
use result

answer = result.ok 42
problem = result.err "invalid input"
```

Result makes success/failure explicit as data when that model is preferable to control-flow errors.

## Managed async Task

```se
use async

job = async.run slow_work 21
answer = async.await job
say answer
```

The runtime provides managed task/await behavior. This stage should not be described as unrestricted parallel execution of arbitrary interpreter VM code.

## Lightweight database

```se
use db

store = db.open "data.se-db"
db.set store "name" "SE"
name = db.get store "name"
```

This API is a persistent local text key/value store suitable for small tools, configuration and learning. Relational SQL features are outside this API's scope.

## HTTPS

```se
use https

body = try https.get "https://example.com"
say body
```

The current TLS transport uses system `curl`, relying on its mature certificate/TLS implementation instead of reimplementing TLS inside SE.

## Design rule

The purpose of these features is not to add more symbols. They should extend familiar SE concepts—`make`, collections, modules and `try`—into more advanced programs.
