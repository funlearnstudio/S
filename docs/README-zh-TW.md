# SE 文件總覽

這是 SE 的繁體中文主要文件入口。

> 第一次學 SE，建議依序閱讀：**快速開始 → 完整教學 → 技術參考**。

[English documentation](README.md)

## 1. 開始學 SE

- [安裝](installation.md) — 預編譯安裝器、原始碼編譯、PATH 與平台注意事項。
- [快速開始](getting-started-zh-TW.md) — 安裝、確認版本、第一支程式與 CLI。
- [完整教學](tutorial-zh-TW.md) — 變數、流程控制、函式、集合、型別、模組、錯誤、async、Web 等。
- [範例](examples-zh-TW.md) — 小型可執行範例。

## 2. 語言參考

- [語言參考](language-reference-zh-TW.md)
- [語法參考](syntax-reference-zh-TW.md)
- [型別與方法](types-zh-TW.md)
- [型別轉換](conversions-zh-TW.md)
- [Collections](collections-zh-TW.md)
- [Modules](modules-zh-TW.md)
- [錯誤處理](errors-zh-TW.md)
- [檔案](files-zh-TW.md)
- [路徑](paths-zh-TW.md)
- [時間](time-zh-TW.md)

## 3. Web 開發

- [SE Web Language](web-language-0.8-zh-TW.md) — Component、HTML/CSS/JS、page 與輸出檔案。
- [Browser API](browser-api-0.8-zh-TW.md) — HTTP、JSON、表單、頁面導航、取消請求與 DOM helper。
- [Web 範例](../examples/component-web.se)
- [Browser API 完整範例](../examples/browser-api.se)

## 4. 進階與底層技術

- [SE 0.6 進階功能](advanced-0.6-zh-TW.md)
- [SE 0.6 API](advanced-0.6-api-zh-TW.md)
- [Generic User Types](generic-user-types-0.7-zh-TW.md)
- [Native Interoperability](native-interop-zh-TW.md)
- [Compiler Architecture](compiler-architecture-zh-TW.md)
- [Runtime Design](runtime-design-zh-TW.md)
- [技術參考](technical-reference-zh-TW.md)

## 5. 專案狀態與 Roadmap

- [Roadmap](roadmap-zh-TW.md)
- 帶版本號的設計文件描述的是該版本／階段的功能。看到 0.7、0.8、0.9 或 1.0 規劃時，不應自動視為目前正式 Release 已全部提供。

## 文件整理規則

為了避免文件再次變亂，主要文件遵守以下規則：

1. 新的使用者程式一律使用 `.se`，舊 `.s` 只作相容說明。
2. CLI 一律寫 `se`。
3. 正式 Release 與未來 Roadmap 分開說明。
4. 教學範例應對應實際 implementation，不把尚未完成的語法寫成已完成。
5. 英文與繁體中文的主要教學保持相同章節結構與術語。
6. 同一主題只保留一份主要入口；深入細節再連到專門 Reference。
