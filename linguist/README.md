# SE GitHub Linguist Submission Bundle

[繁體中文版](README-zh-TW.md)

This directory keeps the files needed for a future upstream `github-linguist/linguist` submission.

SE's maintained TextMate grammar is:

```text
vscode/syntaxes/se.tmLanguage.json
```

with scope:

```text
source.se
```

## Eligibility gate

GitHub Linguist requires real public usage before accepting a new programming language/extension. Under the current contributor guidance, ordinary extensions need at least 2,000 indexed public files (200 for extensions/files expected only once per repository), excluding forks and with reasonable distribution across users/repositories.

Do not open an upstream PR until the current Linguist requirements are met. Artificially generating repositories/files to inflate usage is not valid evidence and may be filtered during review.

Requirements can change; verify Linguist's current contributor documentation again before submission.

## Files in this bundle

- `language-entry.yml` — proposed language metadata before generated ID assignment.
- `sample.se` — representative SE sample for submission preparation.
- `pull-request.md` — draft information for the future Linguist PR.

## Upstream process

When SE is eligible:

1. Fork `github-linguist/linguist`.
2. Add the SE entry to `lib/linguist/languages.yml` following current contribution rules.
3. Add the official grammar through Linguist's grammar tooling.
4. Add representative real-world SE samples with source/license information.
5. Generate the language ID with Linguist's tooling; do not invent it manually.
6. Add any required heuristics/tests.
7. Run the required test suite.
8. Open the PR using Linguist's current template and include public-usage evidence.

## Important boundary

This bundle is preparation material only. Its presence in the SE repository does not mean GitHub.com already recognizes `.se` as the SE language.

For the user-facing explanation, see [GitHub Syntax Highlighting](../docs/github-syntax-highlighting.md).
