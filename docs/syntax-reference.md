# SE Syntax Reference

[繁體中文版](syntax-reference-zh-TW.md)

This page is a compact syntax sheet. For explanations, use [Tutorial](tutorial.md) and [Language Reference](language-reference.md).

## Basic forms

| Purpose | Syntax |
| --- | --- |
| Output | `say value` |
| Input | `name = ask "Question"` |
| Assignment | `name = value` |
| Compound assignment | `+= -= *= /= %=` |
| Comment | `# text` |
| List | `[1, 2, 3]` |
| Map | `["key": value]` |
| Set | `set [1, 2, 3]` |
| Range | `1..10` |
| Function | `make name input` |
| Return | `give value` |
| Module | `use module` |
| Runtime error | `fail "message"` |

## Conditions

```se
if condition
    ...
else if other_condition
    ...
else
    ...
```

A newer source revision may also accept `elif` as an alias for `else if`.

## Loops

```se
repeat count
    ...

for item in values
    ...

for key value in map_value
    ...

while condition
    ...
```

## Functions

```se
make add a b
    give a + b

answer = add 2 3
```

Typed/generic form:

```se
make identity[T] value:T -> T
    give value
```

## Types

```se
type User
    name = ""

    make hello
        say name

user = User
    name = "SE"
```

## Error handling

```se
try
    value = read "data.txt"
else err
    say err.message
```

Propagation form:

```se
make load
    give try read "data.txt"
```

## Match

```se
match value
    case 1
        ...
    case 2
        ...
    else
        ...
```

## Web component

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

## Operators

Approximate precedence, high to low:

```text
**
unary - / not
* / %
+ -
..
< <= > >=
== !=
and
or
```

`**` is right-associative.

## Text escapes

Text uses double quotes. Common escapes include newline, tab, escaped quotes and escaped backslashes.

## Low-punctuation calls

Normal calls usually omit parentheses:

```se
add 2 3
```

Use parentheses when grouping is needed to remove ambiguity.
