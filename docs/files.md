# Files

The shortest file operations are built into S:

```text
try
    write "hello.txt" "Hello"
    append "hello.txt" " from S"
    text = read "hello.txt"
    say text
else err
    say err
```

For a managed file object:

```text
try
    file = open "hello.txt"
    say file.read
else err
    say err
```

`FileData` owns its stream with RAII. Explicit `file.close` is available, but leaving the resource lifetime also closes it automatically. File contents are binary-safe byte streams represented as S Text for the S 0.2 text API; UTF-8 content is preserved without platform-specific transcoding.
