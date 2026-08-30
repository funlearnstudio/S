# SE Database Adapters（0.8 foundation）

SE 保留原本的本機 `db.open / db.set / db.get` Text key/value store，同時加入統一的遠端 database adapter 介面。遠端資料庫不各自發明一整套 SE 語法，而是共用 `db.connect`、`db.exec`、`db.adapter`。

## MongoDB

```se
use db

uri = "mongodb+srv://USER:PASSWORD@cluster.example.mongodb.net/"
mongo = try db.connect "mongodb" uri "myapp" "users"

user = try db.exec mongo "find_one" "{\"name\":\"SE\"}"
say user
```

支援的 MongoDB actions：

- `find_one`
- `find_many`
- `insert_one`
- `insert_many`
- `update_one`
- `replace_one`
- `delete_one`
- `delete_many`
- `count`

`update_one` payload 範例：

```json
{
  "filter": {"name": "SE"},
  "update": {"$set": {"score": 10}},
  "options": {"upsert": true}
}
```

MongoDB adapter 在真正執行 `db.exec` 時使用 Node.js 的官方 `mongodb` package。專案環境需要：

```sh
npm install mongodb
```

`db.connect` 本身只建立連線描述，不會立刻連到 MongoDB；因此可以在沒有網路的測試中驗證 adapter 設定。

## Google Apps Script（GAS）

GAS adapter 連到已部署的 Apps Script Web App HTTPS endpoint：

```se
use db

store = try db.connect "gas" "https://script.google.com/macros/s/DEPLOYMENT_ID/exec"
result = try db.exec store "list" "{}"
say result
```

SE 送出的 JSON envelope 是：

```json
{
  "action": "list",
  "payload": "{}"
}
```

GAS 可以用 `doPost` 接收：

```javascript
function doPost(e) {
  const request = JSON.parse(e.postData.contents);
  const payload = JSON.parse(request.payload || "{}");

  let result;
  if (request.action === "list") {
    result = { ok: true, rows: [] };
  } else {
    result = { ok: false, error: "unknown action" };
  }

  return ContentService
    .createTextOutput(JSON.stringify(result))
    .setMimeType(ContentService.MimeType.JSON);
}
```

GAS transport 使用 HTTPS/curl，會跟隨 Apps Script Web App redirect。

## 為什麼使用統一介面

同類能力集中成：

```se
db.connect adapter ...
db.exec connection action payload
db.adapter connection
```

因此之後加入 PostgreSQL、MySQL、Redis 等 adapter 時，不必再讓 SE 出現一堆互不相容的頂層語法。這符合 `Simple at every level`：資料庫差異由 adapter/runtime 承擔，SE 程式保持一致。

## 安全界線

MongoDB URI、帳號密碼、API token 等秘密只能放在 SE server/backend 環境，例如環境變數。不要把它們寫進 browser `frontend/app.se`，因為 `se web build` 會產生使用者可以直接讀到的 `app.js`。

推薦模式：

```text
Browser SE
   ↓ HTTPS
SE backend/API
   ↓
db.connect / db.exec
   ↓
MongoDB / GAS / future adapters
```

## 目前界線

- 遠端 payload/result 目前以 JSON Text 為交換格式，還沒有直接映射成完整 typed document model。
- MongoDB runtime adapter 目前依賴 Node.js + `mongodb` package；它還不是內嵌 C++ MongoDB driver。
- GAS adapter 依賴 curl。
- CI 不使用真實第三方 credentials；測試只驗證 adapter 建立、型別、跨平台 runtime linkage。真實服務測試應使用專門的 integration environment。
