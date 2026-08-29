# Collections

S 0.2 has three typed collection families: List, Map, and Set.

```text
nums = [1, 2, 3]
nums.add 4
nums.remove 2

for n in nums
    say n
```

Map keys are Text in S 0.2:

```text
user = ["name": "Steve", "age": "15"]

say user["name"]
for key value in user
    say key
    say value
```

Sets keep unique values:

```text
nums = set [1, 2, 2, 3]
nums.add 4
nums.remove 2

if 3 in nums
    say "yes"
```

Collection element/value types are inferred from literals and checked by `s check`. List bounds are checked at runtime. Collection mutation does not expose C++ iterators or raw memory to S programs.
