# Language Guide

S programs are read from top to bottom. A newline ends a statement and four spaces create a block. Tabs count as four spaces, but consistent spaces are recommended.

## Values and variables

```s
age = 15
pi = 3.14
name = "Steve"
ready = true
names = ["Amy", "Bob"]
```

S infers types and rejects mixed operations such as `"5" + 10`. Variables must exist before use. List indexing is bounds checked.

`Text` and `List` have `.len`. Text also has `.upper` and `.lower`. No parentheses are needed because these members take no input.

## Decisions and loops

```s
if age >= 15
    say "yes"
else
    say "no"

repeat 3
    say "again"

for number in 1..10
    say number
```

Ranges include both ends. A descending range such as `5..1` is valid.

## Functions

```s
make square x
    give x * x

answer = square 5
```

Function input follows its name. Parentheses group an argument only when needed: `add (5 + 3) 2`.

## Input

`ask` always returns `Text` in 0.1. This keeps input predictable; explicit conversions are planned rather than hidden automatic conversion.
