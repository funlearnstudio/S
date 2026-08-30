# SE GitHub Linguist Submission Bundle

[English version](README.md)

這個目錄保存未來提交 `github-linguist/linguist` upstream PR 所需的準備資料。

SE 維護中的 TextMate grammar：

```text
vscode/syntaxes/se.tmLanguage.json
```

Scope：

```text
source.se
```

## Eligibility Gate

GitHub Linguist 在接受新 programming language / extension 前，要求真實公開使用量。依目前 contributor guidance，普通 extension 至少需要 2,000 個 indexed public files；若該 extension/file 合理上每個 repository 只會出現一次，門檻為 200。Fork 不計入，而且使用情況要合理分散在不同 user / repository。

在符合最新 Linguist requirement 前，不應提前開 upstream PR。人工大量建立 repository / file 來墊高數量不屬於有效 evidence，也可能在 review 時被過濾。

規則可能改變，所以真正提交前要再次確認 Linguist 最新 contributor documentation。

## Bundle 內容

- `language-entry.yml` — 尚未產生 language ID 前的 proposed metadata。
- `sample.se` — submission preparation 使用的代表性 SE sample。
- `pull-request.md` — 未來 Linguist PR 的 draft information。

## Upstream 流程

當 SE 符合資格後：

1. Fork `github-linguist/linguist`。
2. 依當時 contribution rule 加入 `lib/linguist/languages.yml` entry。
3. 用 Linguist grammar tooling 加入 SE 官方 grammar。
4. 加入有來源／license 資訊的真實 SE samples。
5. 用 Linguist tooling 產生 language ID，不要手動亂選。
6. 補齊需要的 heuristic / tests。
7. 執行要求的 test suite。
8. 使用當時最新 PR template，並附 public-usage evidence。

## 重要邊界

這個 bundle 只是準備資料。它存在於 SE repository，**不代表 GitHub.com 現在已經正式辨識 `.se` 為 SE language**。

使用者-facing 說明請看 [GitHub Syntax Highlighting](../docs/github-syntax-highlighting-zh-TW.md)。
