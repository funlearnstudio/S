# SE 路徑工具

[English version](paths.md)

`path` module 提供 platform-aware 的 path operation。Runtime 會使用 filesystem-aware API，而不是把 `/` 等平台細節硬寫進一般 SE source。

## 使用 Module

```se
use path
```

## Join

```se
p = path.join "data" "users" "a.txt"
```

`path.join` 會依平台正確規則組合 path segment。

## Name / Extension / Parent

```se
say path.name p
say path.ext p
say path.parent p
```

## Exists / Kind

```se
if path.exists p
    say "found"

if path.is_file p
    say "file"

if path.is_dir p
    say "directory"
```

## 搭配 File API

File operation 在支援的位置可接受 path-related value：

```se
p = path.join "data" "users.txt"
if path.exists p
    say read p
```

目標是讓一般 SE code 保持 portable，不需要自己處理不同 OS 的 path separator。
