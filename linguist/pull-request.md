# Add SE Language Support

[繁體中文說明](pull-request-zh-TW.md)

> When SE becomes eligible, use GitHub Linguist's **current** pull-request template. This file is only a prepared source of SE-specific information and must not replace the upstream template.

## Language

SE

## Implementation / homepage

```text
https://github.com/funlearnstudio/SE
```

## Source extension

```text
.se
```

## Public usage evidence

Before submission, gather the public code-search evidence required by Linguist's current contributor rules. Confirm both the indexed-file threshold and reasonable distribution across unrelated repositories/users. Do not open the upstream PR before those requirements are satisfied.

## TextMate grammar

Official grammar source:

```text
https://github.com/funlearnstudio/SE/blob/main/vscode/syntaxes/se.tmLanguage.json
```

Scope:

```text
source.se
```

License: MIT, from the SE repository.

## Description

SE is a low-punctuation, statically checked programming language built around the principle **“Simple at every level.”** It uses indentation-based blocks and aims to keep beginner and advanced programs readable by moving implementation complexity into the compiler and runtime.

## Representative syntax

```se
make identity[T] value:T -> T
    give value

number = int "42"

match number
    case 42
        say "answer"
    else
        say string number
```

## Samples

Use real-world, representative SE programs rather than tutorial-only Hello World files. Record the original source and license for each sample according to Linguist's current submission rules.

## Final submission check

Before opening the upstream PR, re-check Linguist's current usage threshold, PR template, grammar procedure, generated-ID procedure, sample requirements and CI instructions. Do not rely on this draft as a frozen copy of upstream policy.
