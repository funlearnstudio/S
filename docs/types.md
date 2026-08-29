# Types and methods

S 0.2 adds user-defined types without constructors, `new`, pointers, or mandatory `self` syntax.

```text
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
say player.alive
```

Field types are inferred from their defaults. Assigning an incompatible value is a static error. Methods use the existing `make` keyword. Inside a method, an unshadowed field name refers to the current object.

Each `Player` expression creates a fresh object with its own field storage. Object and native resource lifetime is managed by the runtime; S code does not manage memory manually.
