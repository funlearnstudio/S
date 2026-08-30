# SE Collections

[繁體中文版](collections-zh-TW.md)

SE provides managed `List`, `Map`, and `Set` collections. Collection operations are checked by the runtime instead of exposing raw C++ iterators or memory.

## List

```se
nums = [1, 2, 3]
nums.add 4
nums.remove 2
say nums.len
say nums[0]
```

Out-of-range indexing produces an SE error instead of reading unknown memory.

## Map

```se
user = ["name": "SE", "score": 100]
say user["name"]
```

Map values work naturally with JSON-style data.

Iterate key/value pairs:

```se
for key value in user
    say key
    say value
```

## Set

```se
values = set [1, 2, 2, 3]
values.add 4
values.remove 2

if 3 in values
    say "found"
```

Sets keep unique values.

## Higher-order collection helpers

```se
use collections

filtered = collections.filter nums predicate
mapped = collections.map nums transform
sum = collections.reduce nums 0 add
part = collections.slice nums 1 3
first = collections.take nums 2
rest = collections.drop nums 2
```

Sorting helpers include:

```se
ordered = collections.sort_by users "score"
desc = collections.sort_by_desc users "score"
ordered2 = collections.sort_with nums before
```

These helpers generally return new lists instead of exposing mutation through C++ container internals.

## Type information

The checker preserves collection element/value information when it can infer it. Heterogeneous JSON or dynamic platform data may produce broader/unknown types where an exact static type is not available.
