# SE GitHub Linguist submission bundle

This directory keeps the files and wording needed for the future upstream `github-linguist/linguist` submission.

SE already has an official TextMate grammar at `vscode/syntaxes/se.tmLanguage.json` with scope `source.se`.

## Upstream eligibility gate

GitHub Linguist currently requires widespread public usage before accepting a new language. For a normal source extension such as `.se`, the current documented threshold is at least 2,000 indexed public files from the last year, excluding forks, with a reasonable distribution across distinct users and repositories.

Evidence query:

```text
NOT is:fork path:*.se
```

Do not submit the upstream PR before the usage requirement is met. Artificially creating files or repositories to inflate the count is not valid evidence and can be filtered out during review.

## Files in this bundle

- `language-entry.yml` — proposed `languages.yml` entry before `language_id` generation.
- `sample.se` — representative non-Hello-World SE sample for `samples/SE/`.
- `pull-request.md` — draft information to paste into the Linguist PR template when eligible.

## Upstream procedure once eligible

1. Fork `github-linguist/linguist`.
2. Add the SE entry from `language-entry.yml` to `lib/linguist/languages.yml` in alphabetical order.
3. Run `script/add-grammar` against the official SE grammar source.
4. Add representative real-world SE samples under `samples/SE/`.
5. Run `script/update-ids`; do not invent a language ID manually.
6. Run `bundle exec rake test`.
7. Open the PR using Linguist's current PR template and include public usage evidence.
