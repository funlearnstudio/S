# S 0.2 language reference

S keeps indentation blocks, no semicolons, and low-punctuation calls.

## Values and variables

```text
name = "Steve"
age = 15
score = 98.5
ready = true
nums = [1, 2, 3]
```

## Control flow

```text
if age > 10
    say "older than ten"
else
    say "ten or younger"

repeat 3
    say "S"

for n in nums
    say n

while ready
    ready = false
```

## Functions

```text
make add a b
    give a + b

say add 20 22
```

## Types

```text
type Dog
    name = ""
    age = 0

    make birthday
        age = age + 1

dog = Dog
    name = "Milo"

dog.birthday
say dog.age
```

## Modules

```text
use player
```

Public top-level names are imported; names beginning with `_` stay module-private.

## Collections

```text
list = [1, 2, 3]
map = ["name": "Steve"]
set_values = set [1, 2, 2, 3]

list.add 4
if 3 in set_values
    say "yes"
```

## Errors

```text
try
    text = read "data.txt"
    say text
else err
    say err
```

`try expr` propagates a recoverable error from a function. `fail "message"` creates a user error.

## File, path, and time

```text
use path
use time

file = path.join "data" "user.txt"
now = time.now
wait 500ms
```

## Native modules

Native bindings are selected with ordinary `use name`; `.snative` metadata describes the C ABI outside normal S programs.

S 0.1 syntax remains valid in S 0.2.
