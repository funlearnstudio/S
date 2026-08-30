# Add SE language support

> Use GitHub Linguist's current pull-request template when submitting. This file is a prepared source of the SE-specific information, not a replacement for that template.

## Language

SE

## Implementation / homepage

https://github.com/funlearnstudio/SE

## Source extension

`.se`

## Public usage evidence

GitHub Code Search query:

```text
NOT is:fork path:*.se
```

Before submission, record the current indexed file count and confirm that the results are reasonably distributed across unrelated users and repositories. The upstream PR must not be opened until the documented Linguist usage requirement is satisfied.

## TextMate grammar

Official grammar source:

https://github.com/funlearnstudio/SE/blob/main/vscode/syntaxes/se.tmLanguage.json

Scope:

```text
source.se
```

License: MIT, same repository license as SE.

## Description

SE is a low-punctuation, statically checked programming language built around the principle “Simple at every level.” It uses indentation-based blocks and aims to keep beginner and advanced syntax compact while moving complexity into the compiler and runtime.

## Representative syntax

```se
use random

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

Use real-world, representative SE programs. Do not submit Hello World/tutorial-only samples. State the source and license for every sample in the upstream PR.
