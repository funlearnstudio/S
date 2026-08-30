# SE Language Reference

[繁體中文版](language-reference-zh-TW.md)

This reference summarizes the user-facing language model. For examples and explanation, see [Tutorial](tutorial.md). For compiler internals, see [Technical Reference](technical-reference.md).

## Source files and comments

New source files use `.se`.

```se
# comment
say "Hello"
```

Legacy `.s` may remain for migration compatibility.

## Literals

```se
123
3.14
true
false
"text"
[1, 2, 3]
["name": "SE"]
set [1, 2, 3]
500ms
2s
1min
```

## Variables and assignment

```se
name = "SE"
count = 1
count += 1
count -= 1
count *= 2
count /= 2
count %= 3
```

Compound assignment reuses normal assignment/binary-operation semantics.

## Operators

Arithmetic: `+ - * / % **`

Comparison: `== != > >= < <=`

Logic: `and or not`

Membership: `in`

Range: `1..10`

Approximate precedence from high to low:

```text
**
unary - / not
* / %
+ -
..
comparison
== !=
and
or
```

`**` is right-associative.

## Conditions

```se
if score >= 90
    say "A"
else if score >= 80
    say "B"
else
    say "C"
```

Some newer source revisions may also accept `elif` as an alias for `else if`; use syntax provided by the revision you built.

## Loops

```se
repeat 3
    say "Hi"

for item in items
    say item

for key value in map_value
    say key
    say value

while running
    update
```

## Functions

```se
make add a b
    give a + b

answer = add 2 3
```

SE uses low-punctuation calls. Parentheses may be used where explicit grouping is needed.

## Typed and generic functions

```se
make identity[T] value:T -> T
    give value
```

Type annotations are optional where inference is sufficient.

## User-defined types

```se
type User
    name = ""
    score = 0

    make hello
        say name

user = User
    name = "SE"
```

Each object has its own field storage. Methods can resolve unshadowed fields on the current object without requiring a mandatory `self.` prefix.

## Collections

```se
list = [1, 2, 3]
map_value = ["name": "SE"]
set_value = set [1, 2, 2, 3]

list.add 4
say map_value["name"]

if 3 in set_value
    say "found"
```

## Members and indexing

```se
value.member
list[index]
map_value["key"]
value.help
```

`.help` can expose basic member/help information for supported values.

## Modules

```se
use path
```

Top-level names beginning with `_` follow the module-private convention. Circular imports are rejected.

## Error handling

```se
try
    text = read "data.txt"
    say text
else err
    say err.message
```

Create a runtime error:

```se
fail "message"
```

Propagate a fallible expression from a function where supported:

```se
make load
    give try read "data.txt"
```

## Match / case

```se
match status
    case 200
        say "ok"
    case 404
        say "not found"
    else
        say "other"
```

Current matching is value/equality based rather than a complete destructuring pattern system.

## Async

```se
job = async.run work 21
answer = async.await job
```

SE Web also lowers supported `async.await` expressions in browser event handlers to JavaScript `await`.

## Web source

```se
make Button text
    html
        button text

    css
        padding 12

    js
        when click
            say text

page "/"
    Button "Save"
```

Build with:

```bash
se web build app.se dist
```

For browser requests, routing and forms, see [Browser API](browser-api-0.8.md).
