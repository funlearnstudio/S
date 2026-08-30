# GitHub Syntax Highlighting for SE

[繁體中文版](github-syntax-highlighting-zh-TW.md)

SE source uses the `.se` extension and the TextMate scope `source.se`.

The repository's grammar lives at:

```text
vscode/syntaxes/se.tmLanguage.json
```

It is used by the SE VS Code extension and can also serve as the grammar source for a future GitHub Linguist submission.

## GitHub and Linguist

GitHub uses the open-source **Linguist** project for language detection, repository language statistics, and syntax-highlighting language classification. Adding a local grammar or `.gitattributes` entry cannot globally register a brand-new language on GitHub.

For a new language/extension, Linguist requires evidence of real public usage. Its current contributor guidance requires at least 2,000 indexed files for ordinary extensions (or 200 for extensions/files expected only once per repository), excluding forks and with reasonable distribution across repositories/users.

## What SE can do now

1. Keep the official TextMate grammar maintained in this repository.
2. Use the SE VS Code extension for accurate local highlighting.
3. Keep public `.se` examples and projects representative and non-trivial.
4. When public usage meets Linguist requirements, prepare an upstream Linguist contribution using the maintained grammar and real-world samples.

## Future Linguist entry

A future language entry would contain fields such as:

```yaml
SE:
  type: programming
  aliases:
  - se
  extensions:
  - ".se"
  interpreters:
  - se
  tm_scope: source.se
```

The language ID must be generated using Linguist's tooling rather than chosen manually. Color and editor-mode choices should be treated as project/branding decisions and verified when preparing the actual upstream PR.

## Upstream checklist

A future Linguist submission should include:

- a language entry in `lib/linguist/languages.yml`
- the maintained TextMate grammar added through Linguist's grammar tooling
- representative real-world `.se` samples with source/license information
- generated language ID
- any required heuristics/tests
- evidence that `.se` meets Linguist's public-usage requirement
- all required Linguist CI checks passing

## Do not fake another language

Do not map `.se` files to an unrelated language just to obtain colors, for example:

```text
*.se linguist-language=Python
```

That would make repository language statistics and highlighting misleading. Until SE is accepted upstream, the VS Code extension remains the accurate local highlighting path.

## References

GitHub documents that repository language detection is powered by Linguist. Linguist's own contributor documentation is the source of truth for current submission and usage requirements; those requirements can change, so verify them again before opening an upstream contribution.
