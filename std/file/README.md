# file

`file` is the standard file I/O module. The shortest forms are also available as built-ins:

```text
try
    write "hello.txt" "Hello"
    append "hello.txt" " from S"
    text = read "hello.txt"
    say text
else err
    say err
```

`open` returns an automatically managed file resource. Leaving its lifetime closes the underlying stream.
