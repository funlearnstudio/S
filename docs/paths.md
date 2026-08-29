# Paths

Paths use `std::filesystem` under the runtime rather than hard-coded `/` rules.

```text
use path

file = path.join "data" "user.txt"
say path.name file
say path.ext file
say path.parent file
say path.exists file
say path.is_file file
say path.is_dir file
```

`path.join` accepts one or more Text/Path values and returns a Path. File APIs accept Text or Path.
