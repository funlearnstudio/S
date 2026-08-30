# SE Standard Library / SE 標準函式庫

SE standard/runtime modules are imported with `use` and share the same module system as normal SE source.

SE 的 standard/runtime module 透過 `use` 載入，和一般 SE source 共用同一套 module system。

```se
use file
use path
use time
use math
use json
use collections
```

Most built-in modules are implemented by the SE runtime rather than as ordinary source files in this directory. The `std/` tree therefore documents the standard-library surface and installation layout; it is not a promise that every runtime module is implemented as `.se` source here.

多數 built-in module 由 SE Runtime 實作，而不是全部以 `.se` source 放在這個目錄。因此 `std/` 主要記錄標準函式庫介面與安裝結構，不代表每個 runtime module 都會在這裡找到對應 `.se` 檔案。

For user-facing module documentation, start at:

使用者文件請從這裡開始：

- [English documentation](../docs/README.md)
- [繁體中文文件](../docs/README-zh-TW.md)
- [Modules](../docs/modules.md) / [模組系統](../docs/modules-zh-TW.md)

Implementation details may still use historical internal `s` names for compatibility, but user-facing naming is **SE**, CLI `se`, and source `.se`.

內部實作為了相容性仍可能保留歷史 `s` 名稱，但使用者-facing 名稱統一為 **SE**、CLI `se`、source `.se`。
