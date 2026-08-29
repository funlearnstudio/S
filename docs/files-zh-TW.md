# 檔案操作

本文件是 `files.md` 的繁體中文版。

SE 提供簡單的文字檔案讀寫與 File 物件。

## read

```se
text = read "hello.txt"
say text
```

讀取失敗時會產生 FileError，因此實務上建議：

```se
try
    text = read "hello.txt"
    say text
else err
    say err.message
```

## write

```se
write "hello.txt" "Hello SE"
```

`write` 會覆寫檔案。

## append

```se
append "log.txt" "new line\n"
```

## open

```se
try
    file = open "data.txt"
else err
    say err.message
```

`open` 回傳 managed File value，由 Runtime 管理資源。

## file module

也可以：

```se
use file

text = file.read "data.txt"
file.write "out.txt" text
```

## 路徑

檔案 API 可以搭配 `use path`：

```se
use path

p = path.join "data" "users.txt"
if path.exists p
    say read p
```

SE 的方向是讓日常檔案工作簡單，同時把錯誤明確交給 `try` 處理。
