# Errors

Operations that can fail must be handled or propagated.

```text
try
    text = read "hello.txt"
    say text
else err
    say err.kind
    say err
```

Inside a function, `try expr` propagates a recoverable runtime error to the caller:

```text
make load
    give try read "hello.txt"
```

Create an application error with `fail`:

```text
make check score
    if score < 0
        fail "Invalid score"
    give score
```

An Error carries a message, source path, line, and kind. `say err` prints the message; `err.message`, `err.source`, `err.line`, and `err.kind` expose details when needed.

File/native failures use this model rather than crashing the interpreter. Static language/type errors remain compiler errors and are not catchable as runtime errors.
