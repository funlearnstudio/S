# Modules

A module is an S source file selected with one line:

```text
use player

p = Player
p.hit 20
say p.hp
```

For `use player`, S 0.2 searches in this order:

1. The importing file's project directory.
2. Built-in standard modules (`file`, `path`, `time`).
3. `$S_HOME/packages` when configured.
4. Native `.snative` module metadata where applicable.

Top-level names beginning with `_` stay private to their module. Other top-level types, functions, and assigned names are exported. If two imports provide the same public name, `s check` reports a collision instead of choosing silently.

Circular imports such as `a -> b -> a` are rejected with the import chain. `s check`, `s run`, and `s build` all load the complete dependency graph before execution or code generation.
