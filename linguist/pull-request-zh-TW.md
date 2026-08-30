# SE GitHub Linguist PR 準備說明

[English draft](pull-request.md)

> SE 真正符合資格後，提交時必須使用 GitHub Linguist **當時最新**的 pull-request template。這份文件只是 SE-specific 資料整理，不是 upstream template 的替代品。

## Language

SE

## Implementation / Homepage

```text
https://github.com/funlearnstudio/SE
```

## Source Extension

```text
.se
```

## Public Usage Evidence

提交前要依 Linguist 最新 contributor rule 蒐集 public code-search evidence，確認 indexed-file threshold 已達標，並且使用情況合理分散在不同 repository / user。沒有符合 requirement 前不要開 upstream PR。

## TextMate Grammar

官方 grammar：

```text
https://github.com/funlearnstudio/SE/blob/main/vscode/syntaxes/se.tmLanguage.json
```

Scope：

```text
source.se
```

License：MIT，來源為 SE repository。

## Description

SE 是一門低標點、靜態檢查的程式語言，核心原則是 **Simple at every level.**。SE 使用 indentation-based block，並把較多 implementation complexity 留在 Compiler / Runtime，使入門與進階程式都維持可讀。

## Representative Syntax

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

真正提交時應使用有代表性的 real-world SE program，不要只放 Hello World / tutorial sample。每份 sample 的原始來源與 license 都要依 Linguist 最新規則標明。

## 最後檢查

開 upstream PR 前重新確認 Linguist 最新的 usage threshold、PR template、grammar procedure、generated language ID、sample requirement 與 CI 指令。不要把這份 draft 當成永久不變的 upstream policy。
