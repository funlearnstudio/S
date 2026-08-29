# 路徑工具

本文件是 `paths.md` 的繁體中文版。

```se
use path
```

## join

```se
p = path.join "data" "users" "a.txt"
```

使用平台正確的路徑分隔方式組合路徑。

## name / ext / parent

```se
say path.name p
say path.ext p
say path.parent p
```

## exists

```se
if path.exists p
    say "found"
```

## is_file / is_dir

```se
if path.is_file p
    say "file"

if path.is_dir p
    say "directory"
```

`path` module 回傳或接受 Path/Text 相關值，適合搭配 `file.read`、`file.write` 與一般專案工具使用。
