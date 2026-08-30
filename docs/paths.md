# SE Paths

[繁體中文版](paths-zh-TW.md)

The `path` module provides platform-aware path operations. Runtime path handling is based on filesystem-aware APIs rather than hard-coded `/` rules.

## Use the module

```se
use path
```

## Join

```se
p = path.join "data" "users" "a.txt"
```

`path.join` combines path segments using the correct platform rules.

## Name, extension and parent

```se
say path.name p
say path.ext p
say path.parent p
```

## Existence and kind

```se
if path.exists p
    say "found"

if path.is_file p
    say "file"

if path.is_dir p
    say "directory"
```

## File API integration

File operations accept path-related values where supported:

```se
p = path.join "data" "users.txt"
if path.exists p
    say read p
```

The goal is to keep path handling portable while avoiding platform separator details in ordinary SE source.
