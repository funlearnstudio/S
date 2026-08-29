# Native Interop 原生互通

本文件是 `native-interop.md` 的繁體中文版。

SE 可以透過 C ABI 與原生 C/C++ 函式庫互通，讓高階 SE 程式在需要時仍能使用系統層能力。

## `.snative`

Native module metadata 描述要載入的原生函式庫、符號與型別。Runtime 在 Unix 類平台透過動態載入機制解析原生符號。

## 支援資料

目前 interop 已涵蓋：

- Int
- Num
- Bool
- Text
- Bytes
- managed native handle

## Bytes ABI

公開 ABI 使用 byte pointer + size，而不是把 binary data 當成 C string。這能正確處理 `0x00` 等任意位元組。

SE Runtime 擁有 SE 端 Bytes；原生端回傳的 byte buffer 會複製進 SE-managed memory，避免 ownership 模糊。

## Managed handle

對需要生命週期管理的 native resource，可使用 opaque handle 與 deleter。Runtime 在 handle 結束生命週期時自動釋放原生資源，避免把 raw pointer 暴露給一般 SE 程式。

## Binding generator

```bash
se bind module.sbind generated
```

會產生 native metadata、C header 與 C++ wrapper。Generator 遇到不支援的 signature 應直接報錯，而不是產生假的 stub。

## 安全邊界

Native interop 是 SE 的「底層出口」，不是日常程式的預設寫法。進入 C ABI 後，SE 無法自動保證外部原生程式庫本身沒有記憶體錯誤，因此應把 unsafe / native 範圍保持小而清楚。

## 測試

專案包含真實 C test library 與 Bytes / handle lifecycle regression tests，用來驗證 binary round-trip、獨立 copy 與 automatic cleanup。
