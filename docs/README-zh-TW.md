# SE 文件總覽

這是 SE 的繁體中文主要文件入口。

> 第一次學 SE，建議依序閱讀：**快速開始 → 完整教學 → 語言參考**。想了解 Compiler / Runtime 再讀 **技術參考**。

[English documentation](README.md)

## 1. 開始學 SE

- [安裝](installation.md) — 預編譯安裝器、原始碼編譯、PATH 與平台注意事項。
- [快速開始](getting-started-zh-TW.md) — 安裝、確認版本、第一支程式與 CLI。
- [完整教學](tutorial-zh-TW.md) — 變數、流程控制、函式、集合、型別、模組、錯誤、async、Web 等。
- [範例索引](examples-zh-TW.md) — 依主題整理可執行範例。

## 2. 語言參考

- [語言參考](language-reference-zh-TW.md)
- [語法速查](syntax-reference-zh-TW.md)
- [型別與方法](types-zh-TW.md)
- [型別轉換](conversions-zh-TW.md)
- [Collections](collections-zh-TW.md)
- [Modules](modules-zh-TW.md)
- [錯誤處理](errors-zh-TW.md)
- [檔案](files-zh-TW.md)
- [路徑](paths-zh-TW.md)
- [時間](time-zh-TW.md)

## 3. Web 開發

- [SE Web Language](web-language-0.8-zh-TW.md) — Component、HTML/CSS/JS、page 與 generated output。
- [Browser API](browser-api-0.8-zh-TW.md) — HTTP、JSON、Form、routing、cancellation 與 DOM helper。
- [Web 範例](../examples/component-web.se)
- [Browser API 範例](../examples/browser-api.se)

## 4. 進階與底層技術

- [SE 0.6 進階功能](advanced-0.6-zh-TW.md)
- [SE 0.6 API](advanced-0.6-api-zh-TW.md)
- [Generic User Types](generic-user-types-0.7-zh-TW.md)
- [Native Interoperability](native-interop-zh-TW.md)
- [Compiler Architecture](compiler-architecture-zh-TW.md)
- [Runtime Design](runtime-design-zh-TW.md)
- [技術參考](technical-reference-zh-TW.md)

## 5. Stable / Current Source / Roadmap

SE 的正式 Release、GitHub `main` 與帶版本號的設計文件不是完全同一件事：

- **Stable Release**：一般安裝與 CLI 使用以目前 GitHub Release 為準。
- **Current Source**：`main` 可能已包含更新的 Compiler / Web 工作，但版本字串仍處於 stable line。
- **Versioned Design / Roadmap**：0.7 / 0.8 / 0.9 / 1.0 文件描述對應階段；Roadmap 內容不會自動等於目前正式功能。

請看 [Roadmap](roadmap-zh-TW.md) 了解規劃。

## 文件整理規則

1. 新使用者 source 一律使用 `.se`；`.s` 只出現在 compatibility/history。
2. CLI 一律寫 `se`。
3. Stable behavior、current-source addition、roadmap/design work 要明確分開。
4. Example 應對應它所在 revision 的實際 implementation。
5. 英文與繁中主要文件保持相同章節結構與術語。
6. 同一主題只保留一份 canonical guide/reference；舊入口改成導向，不再複製整篇內容。
