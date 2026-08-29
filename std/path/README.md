# path

`path` keeps path handling cross-platform.

```text
use path

file = path.join "data" "user.txt"
say path.name file
say path.ext file
say path.parent file
say path.exists file
```

Available operations in S 0.2: `join`, `name`, `ext`, `parent`, `exists`, `is_file`, and `is_dir`.
