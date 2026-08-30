# SE Database Adapters（0.8 foundation）

[English version](database-adapters-0.8.md)

這份版本化文件描述 0.8 database-adapter foundation。SE 保留本機 `db.open` / `db.set` / `db.get` Text key/value store，同時建立共用 remote data adapter model。

## 統一 Adapter Interface

```se
use db

connection = try db.connect adapter endpoint database collection
result = try db.exec connection action payload
say db.adapter connection
```

目標是把不同 database/service 的差異留在 adapter / Runtime，而不是每個 backend 都發明新的 top-level 語法。

## MongoDB Adapter

概念寫法：

```se
use db

mongo = try db.connect "mongodb" uri "myapp" "users"
user = try db.exec mongo "find_one" query_json
```

此階段 implementation 的 action 涵蓋 find、insert、update/replace、delete、count 等常見操作。

目前 MongoDB execution path 依賴 Node.js 與官方 `mongodb` package。`db.connect` 建立 connection description，真正 remote operation 在 `db.exec` 時發生。

## Google Apps Script Adapter

已部署的 Apps Script Web App 可作為 HTTPS adapter endpoint：

```se
store = try db.connect "gas" endpoint
result = try db.exec store "list" "{}"
```

Adapter 會送出 action/payload envelope，再透過 HTTPS transport 取得 service response。

## Server / Browser 邊界

Database connection information 應保留在 backend：

```text
SE browser app
    ↓ HTTPS / API
SE backend
    ↓ db.connect / db.exec
remote database / service
```

不要把 database URI、credential、token 等只適合 server 保存的資訊寫進會輸出成 browser JavaScript 的 source。

## 目前邊界

- Remote payload/result 在此 foundation 主要以 JSON Text 交換。
- MongoDB 依賴 external Node.js driver，不是 embedded C++ driver。
- GAS transport 使用 HTTPS/curl path。
- CI 可驗證 adapter/runtime 行為，但不應依賴真實第三方帳號或 credential。

後續 adapter 應繼續重用 `db.connect` / `db.exec`，而不是再增加新的語言 syntax layer。
