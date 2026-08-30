# SE Tutorial

[繁體中文版](tutorial-zh-TW.md)

SE is designed around one rule: **Simple at every level.** It uses indentation, low-punctuation calls, type inference, checked runtime operations, and a shared language model across interpreted, native, and Web workflows.

This tutorial focuses on user-facing SE. For compiler internals, see [Technical Reference](technical-reference.md).

## 1. Hello SE

```se
say "Hello SE"
```

Run:

```bash
se run hello.se
```

Check without running:

```bash
se check hello.se
```

## 2. Variables and values

SE infers common types from values:

```se
name = "Steve"
age = 15
score = 98.5
ready = true
```

Common runtime value families include `None`, `Int`, `Num`, `Bool`, `Text`, `Bytes`, `List`, `Map`, `Set`, functions, objects, errors, durations, paths/files, and managed native handles.

## 3. Input and output

```se
name = ask "Your name?"
say "Hello " + name
```

`say` prints a value. `ask` prompts and returns input text.

## 4. Arithmetic, comparison and logic

```se
a = 10
b = 3

say a + b
say a - b
say a * b
say a / b
say a % b
say a ** 2

say a > b
say a == b
say a != b
say ready and a > b
say not ready
```

## 5. Conditions

```se
score = 85

if score >= 90
    say "A"
else if score >= 80
    say "B"
else
    say "C"
```

SE blocks use indentation rather than braces. A newer source branch may also provide `elif` as an alias; use only syntax present in the revision you built.

## 6. Loops

Repeat a fixed number of times:

```se
repeat 3
    say "Hi"
```

Iterate a collection:

```se
nums = [1, 2, 3]

for n in nums
    say n
```

Conditional loop:

```se
n = 0
while n < 3
    say n
    n += 1
```

Range:

```se
for n in 1..10
    say n
```

## 7. Functions

Functions use `make`; return values use `give`:

```se
make add a b
    give a + b

answer = add 5 3
say answer
```

Calls intentionally avoid unnecessary parentheses.

Functions can also be values and closures:

```se
make make_adder base
    make inner value
        give base + value
    give inner

add10 = make_adder 10
say add10 5
```

## 8. List, Map and Set

List:

```se
nums = [1, 2, 3]
nums.add 4
say nums.len
say nums[0]
```

Map:

```se
user = ["name": "Steve", "role": "student"]
say user["name"]

for key value in user
    say key
    say value
```

Set:

```se
values = set [1, 2, 2, 3]
values.add 4

if 3 in values
    say "found"
```

SE checks invalid collection operations rather than exposing C++ undefined behavior.

## 9. Text and Bytes

```se
text = "Hello"
say text.len
say text.upper
say text.lower
```

Binary-safe data uses `Bytes`:

```se
data = bytes "hello"
say data.len
```

Text and Bytes are separate so binary APIs do not need to pretend arbitrary bytes are text.

## 10. User-defined types

```se
type Player
    name = ""
    hp = 100

    make hit damage
        hp = hp - damage

    make alive
        give hp > 0

player = Player
    name = "Steve"

player.hit 20
say player.hp
```

Methods can access fields directly; ordinary SE code does not require a mandatory `self.` prefix for every field access.

## 11. Modules

`player.se`:

```se
type Player
    hp = 100
```

`main.se`:

```se
use player

p = Player
say p.hp
```

New source uses `.se`. Top-level names beginning with `_` follow the module-private convention. Circular imports are rejected.

## 12. Error handling

Recover a failure:

```se
try
    text = read "data.txt"
    say text
else err
    say err.message
```

Create an error:

```se
make check score
    if score < 0
        fail "Invalid score"
    give score
```

Propagate a recoverable error:

```se
make load
    give try read "data.txt"
```

## 13. Pattern matching

SE provides value-based `match` / `case`:

```se
match status
    case 200
        say "ok"
    case 404
        say "not found"
    else
        say "other"
```

This is value/equality matching, not a full destructuring pattern system.

## 14. Option and Result

Advanced SE provides helpers for values that may be absent or operations that may succeed/fail. Use the Option/Result APIs when explicit value-level handling is clearer than exceptions/errors.

See [Advanced SE 0.6](advanced-0.6-zh-TW.md) for the current API examples.

## 15. Async tasks

```se
job = async.run slow_work 21
answer = async.await job
say answer
```

Tasks are managed runtime tasks. They should not be interpreted as unrestricted parallel execution of arbitrary SE VM code.

## 16. Files, paths and time

Files:

```se
write "hello.txt" "Hello"
append "hello.txt" " SE"
text = read "hello.txt"
say text
```

Paths:

```se
use path
file = path.join "data" "user.txt"
say path.exists file
```

Time and durations:

```se
use time
say time.now
wait 500ms
wait 2s
```

## 17. JSON and HTTP

JSON support maps JSON objects to Map, arrays to List, strings to Text, numbers to numeric values, booleans to Bool, and null to None.

Backend HTTP example:

```se
use http
body = http.get "http://example.com/"
say body
```

HTTPS is also available and currently delegates TLS transport to the system `curl` tool.

## 18. Tests

Test files conventionally use `*_test.se`:

```se
use test

test.equal 4 2 + 2
test.ok true
```

Run a project test suite:

```bash
se test .
```

## 19. SE Web

A Web component reuses `make`:

```se
make Button text
    html
        button text

    css
        padding 12
        border_radius 8

    js
        when click
            say text

page "/"
    Button "Save"
```

Build:

```bash
se web build app.se dist
```

Generated output is standard HTML, CSS and JavaScript/TypeScript. See [SE Web Language](web-language-0.8.md).

## 20. Browser API and pages

SE Web can perform browser-side requests and navigation:

```se
make LoadUsers
    html
        button "Load"

    js
        when click
            try
                options = ["timeout": 8000, "retries": 2]
                task = browser.get_json "/api/users" options
                result = async.await task
                say result.data
            else err
                say err.message

page "/"
    LoadUsers

page "/about"
    About
```

Navigate with `browser.go`, send JSON with `browser.post_json`, and cancel keyed requests with `browser.cancel`. See [Browser API](browser-api-0.8.md).

## 21. Native interoperability

Native libraries are exposed through a C ABI description rather than arbitrary C++ ABI details. Ordinary SE code still imports a module normally:

```se
use native_test
say add 20 22
```

SE supports scalar values, Text, Bytes and managed opaque handles across the native bridge. See [Native Interoperability](native-interop.md).

## 22. Interpreter, native and Web workflows

```text
se check app.se          static checking
se run app.se            interpreter
se build app.se          C++20 native backend
se web build app.se dist Web compiler
```

The goal is for the language model to stay familiar while the execution target changes.

## Next steps

- [Language Reference](language-reference.md) for exact language details.
- [Technical Reference](technical-reference.md) for compiler/runtime architecture.
- [SE Web Language](web-language-0.8.md) for Web authoring.
- [Browser API](browser-api-0.8.md) for browser requests, forms and routing.
