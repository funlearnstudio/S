# SE 的 GitHub Syntax Highlighting

[English version](github-syntax-highlighting.md)

SE source 使用 `.se` extension，TextMate scope 為 `source.se`。

Repository 內的 grammar 位於：

```text
vscode/syntaxes/se.tmLanguage.json
```

這份 grammar 目前可供 SE VS Code extension 使用，也可以成為未來提交 GitHub Linguist 時的 grammar source。

## GitHub / Linguist

GitHub 使用 open-source **Linguist** 做 language detection、repository language statistics 與 syntax-highlighting language classification。只在自己的 repository 放 grammar 或 `.gitattributes`，不能讓 GitHub 全站正式註冊一門新語言。

依目前 Linguist contributor guidance，普通 extension 要加入新語言／extension，需要有真實公開使用證據：一般情況至少 2,000 個 indexed files；若該 extension/file 合理上每個 repository 只會出現一次，門檻則是 200。Fork 不計入，而且使用情況要合理分散在不同 repository / user。

## SE 現在可以做什麼

1. 持續維護 repository 內的官方 TextMate grammar。
2. 使用 SE VS Code extension 提供準確的本機 highlighting。
3. 讓公開 `.se` 專案與 example 保持真實、完整，不只放 Hello World。
4. 等公開使用量符合 Linguist 要求後，再準備 upstream contribution。

## 未來 Linguist Entry

概念上會包含：

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

`language_id` 應由 Linguist tooling 產生，不應手動亂選。Color / editor mode 也應在真正提交 upstream PR 時再確認。

## Upstream Checklist

未來提交 Linguist 時應包含：

- `lib/linguist/languages.yml` 的 language entry
- 透過 Linguist grammar tooling 加入官方 TextMate grammar
- 有來源與 license 資訊的真實 `.se` samples
- generated language ID
- 需要時的 heuristic / tests
- `.se` 已達 public-usage requirement 的證據
- Linguist CI 全部通過

## 不要假裝成別的語言

不要為了讓 GitHub 顯示顏色就寫：

```text
*.se linguist-language=Python
```

這會讓 language statistics 與 highlighting 變得不正確。SE 尚未被 Linguist upstream 接受前，VS Code extension 才是準確的本機 highlighting 方式。

## 注意

GitHub 確認 repository language detection 使用 Linguist；Linguist contributor documentation 則是 submission / usage requirement 的 source of truth。這些規則未來可能更新，因此真正準備 upstream PR 前應重新確認最新要求。
