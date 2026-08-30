# `path` Module / `path` 模組

The `path` module keeps path handling platform-aware so SE programs do not need to hard-code path separators.

`path` module 提供 platform-aware 的路徑操作，讓 SE 程式不需要自己處理不同作業系統的 path separator。

```se
use path

file = path.join "data" "user.txt"
say path.name file
say path.ext file
say path.parent file
say path.exists file
say path.is_file file
say path.is_dir file
```

Common operations / 常用操作：

```text
join
name
ext
parent
exists
is_file
is_dir
```

For the full user-facing reference / 完整使用說明：

- [Paths](../../docs/paths.md)
- [路徑工具](../../docs/paths-zh-TW.md)
