# SE 檔案操作

[English version](files.md)

SE 提供簡單的 file helper，也提供 managed file value 給需要較長生命週期的資源。

## Read

```se
try
    text = read "hello.txt"
    say text
else err
    say err.message
```

讀取失敗會產生 recoverable file error。

## Write

```se
write "hello.txt" "Hello SE"
```

`write` 會覆蓋目標檔案內容。

## Append

```se
append "log.txt" "new line\n"
```

## Managed File Value

```se
try
    file = open "data.txt"
    say file.read
else err
    say err.message
```

底層 file resource 由 Runtime 管理。即使有 explicit close operation，一般 SE code 也不需要管理 raw stream pointer 或 manual allocation。

## file Module

也可以使用 module form：

```se
use file

text = file.read "data.txt"
file.write "out.txt" text
```

## Paths

File API 可在支援的位置搭配 Path value：

```se
use path

p = path.join "data" "users.txt"
if path.exists p
    say read p
```

## Error Model

File failure 使用 SE recoverable error，不會直接讓 Interpreter 因底層 I/O failure 崩潰。Syntax/type problem 則屬於 execution 前的 compiler error。

處理任意 binary buffer 時應使用 `Bytes` API，不要把所有 binary data 都當成 Text。
