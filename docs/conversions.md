# SE Core Type Conversions

[繁體中文版](conversions-zh-TW.md)

SE provides core conversion helpers without requiring a module import. These helpers convert values into SE's existing runtime types; they do not create duplicate `String`, `Double`, `Float`, or `Char` runtime type systems.

## Text

```se
say text 123
say string 123
```

Both return `Text`.

## Integer

```se
age = int "15"
count = integer "42"
```

`int` and `integer` return `Int`. Conversions that cannot produce a valid integer fail with a conversion error rather than silently returning zero.

## Number

```se
price = num "19.95"
a = double "3.14"
b = float 2
```

`num`, `double`, and `float` all return SE `Num` values.

## Boolean

```se
ready = bool "true"
enabled = boolean "FALSE"
```

`bool` and `boolean` return `Bool`. Text conversion accepts case variants of true/false supported by the runtime.

## Character helper

SE does not currently define a separate `Char` runtime type. `char` returns a `Text` value containing exactly one Unicode character.

```se
letter = char "A"
han = char "中"
letter2 = char 65
```

Invalid empty/multi-character input is rejected.

## With ask

```se
number = int ask "Enter a number: "
```

`ask` stays simple and returns Text; programs convert explicitly when they need another type.

## Alias table

| Helper | Result |
| --- | --- |
| `text value` | `Text` |
| `string value` | `Text` |
| `int value` | `Int` |
| `integer value` | `Int` |
| `num value` | `Num` |
| `double value` | `Num` |
| `float value` | `Num` |
| `bool value` | `Bool` |
| `boolean value` | `Bool` |
| `char value` | one-character `Text` |
