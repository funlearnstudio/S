# SE 0.6 Advanced API Reference

Implemented modules and syntax:

```text
collections.filter List Function -> List
collections.map List Function -> List
collections.reduce List initial Function -> value
collections.sort_by List Text -> List
collections.sort_by_desc List Text -> List
collections.sort_with List Function -> List
collections.slice List Int Int -> List
collections.take List Int -> List
collections.drop List Int -> List

function.bind Function values... -> Function
function.call Function values... -> value
function.pipe value Function... -> value

async.run Function values... -> Task
async.await Task -> value
async.ready Task -> Bool

option.some value -> Option
option.none -> Option
option.is_some Option -> Bool
option.is_none Option -> Bool
option.value Option -> value
option.or Option fallback -> value

result.ok value -> Result
result.err Text -> Result
result.is_ok Result -> Bool
result.is_err Result -> Bool
result.value Result -> value
result.error Result -> Text
result.or Result fallback -> value

match.value value pattern handler ... fallback -> value
match.option Option some_handler none_handler -> value
match.result Result ok_handler error_handler -> value

db.open Text -> Database
db.set Database Text Text -> None
db.get Database Text -> Option
db.has Database Text -> Bool
db.remove Database Text -> Bool
db.keys Database -> List
db.save Database -> None

https.get Text -> Text
https.post Text Text -> Text
https.post_json Text Text -> Text
```

Generic function syntax:

```se
make identity[T] value:T -> T
    give value
```

Value pattern matching syntax:

```se
match value
    case 1
        say "one"
    case 2
        say "two"
    else
        say "other"
```

SE uses named `make` functions plus function values and `function.bind` as the low-punctuation alternative to inline lambda syntax.
