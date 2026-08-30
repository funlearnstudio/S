# SE Error Handling

[繁體中文版](errors-zh-TW.md)

SE separates compile-time/checker errors from recoverable runtime errors.

## try / else

```se
try
    text = read "data.txt"
    say text
else err
    say err.message
```

If a recoverable runtime failure occurs inside the `try` block, control moves to `else err`.

## Error values

An error can expose details such as:

```text
message
kind
source
line
```

Example:

```se
say err.message
say err.kind
```

## fail

Create an application/runtime error:

```se
make check score
    if score < 0
        fail "Invalid score"
    give score
```

`fail` stops the current flow unless an outer recoverable-error boundary handles it.

## try expression

Inside supported expression contexts, `try expr` can propagate a recoverable failure:

```se
make load
    give try read "data.txt"
```

The exact expression form depends on the operation being called; use `try` for operations that are defined as fallible.

## Compile-time errors

Syntax errors, unresolved names, incompatible types and other checker failures are compiler errors. They are not ordinary runtime values and cannot be caught by a program-level `try` block.

## Option and Result

Option/Result helpers are useful when absence or success/failure should be represented as ordinary data rather than control-flow failure.

```se
use result

r = result.err "invalid"
if result.is_err r
    say result.error r
```

Use runtime errors for exceptional/fallible operations and Option/Result when explicit value-level handling makes the API clearer.
