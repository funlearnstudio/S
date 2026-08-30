# SE File I/O

[繁體中文版](files-zh-TW.md)

SE provides direct file helpers for common text workflows plus managed file values for longer-lived resources.

## Read

```se
try
    text = read "hello.txt"
    say text
else err
    say err.message
```

A failed read produces a recoverable file error.

## Write

```se
write "hello.txt" "Hello SE"
```

`write` replaces the target file contents.

## Append

```se
append "log.txt" "new line\n"
```

## Managed file value

```se
try
    file = open "data.txt"
    say file.read
else err
    say err.message
```

The runtime owns the underlying file resource. Explicit close operations may be available, but ordinary SE code does not need to manage raw stream pointers or manual allocation.

## file module

The module form is also available:

```se
use file

text = file.read "data.txt"
file.write "out.txt" text
```

## Paths

File APIs work with path values where supported:

```se
use path

p = path.join "data" "users.txt"
if path.exists p
    say read p
```

## Error model

File failures use SE recoverable errors rather than crashing the interpreter. Static syntax/type problems remain compiler errors and are handled before execution.

For binary-safe buffers, use `Bytes` APIs rather than assuming arbitrary binary data is text.
