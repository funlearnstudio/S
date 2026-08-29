# Syntax Reference

| Purpose | Syntax |
|---|---|
| Output | `say value` |
| Input | `name = ask "Question"` |
| Assignment | `name = value` |
| List | `[1, 2, 3]` |
| Inclusive range | `1..10` |
| Branch | `if condition` / `else` |
| Fixed loop | `repeat count` |
| Value loop | `for name in values` |
| Conditional loop | `while condition` |
| Function | `make name input` |
| Return | `give value` |

Precedence, high to low: `**`, unary `-`/`not`, `* / %`, `+ -`, `..`, comparisons, equality, `and`, `or`. `**` is right associative.

Comments begin with `#`. Text uses double quotes and supports `\\n`, `\\t`, escaped quotes, and escaped backslashes.
