# SE Web Browser API（0.8）

[English version](browser-api-0.8.md)

`se web build` 會把 Browser Runtime 一起產生到 `app.js`。Component event 可以直接做 HTTP request、傳送 JSON/表單、切換 page、取消 request 與更新常見 DOM 狀態，不必為每個網站另外寫大型 `js native` helper。

## Request

```se
options = [
    "timeout": 8000,
    "retries": 2,
    "throw_http": true
]

task = browser.get_json "/api/users" options
result = async.await task
say result.status
say result.data
```

主要 helper：

```text
browser.request
browser.get
browser.get_json
browser.post
browser.post_json
browser.put_json
browser.patch_json
browser.delete
browser.submit_json
browser.upload
```

## Request Options

```se
options = [
    "timeout": 10000,
    "retries": 2,
    "retry_delay": 300,
    "throw_http": true,
    "credentials": "same-origin",
    "key": "load-users"
]
```

Runtime 會處理 timeout、取消、offline、network failure、JSON parse error、HTTP error、可重試 status、backoff 與 `Retry-After`。

CORS 仍由 server 與 browser policy 控制；SE Browser Runtime 可以回報 request failure，但不能繞過 server 的 Origin 規則。

## Cancellation / Deduplication

替 request 指定 key：

```se
task = browser.get "/api/users" ["key": "load-users"]
```

取消：

```se
browser.cancel "load-users"
```

取消全部 active keyed request：

```se
browser.cancel_all()
```

GET/HEAD 可對相同 in-flight request 做 deduplication，降低連點造成的重複 traffic。

## 傳送 JSON

```se
payload = ["name": "SE", "message": "Hello"]
options = ["throw_http": true]
task = browser.post_json "/api/messages" payload options
result = async.await task
```

PUT / PATCH 對應使用 `put_json` / `patch_json`。

## Form / Upload

```se
data = browser.form_json "#profile-form"
task = browser.submit_json "/api/profile" "#profile-form"
result = async.await task
```

包含 file 的 form：

```se
task = browser.upload "/api/upload" "#upload-form"
```

## Pages / Navigation

```se
page "/"
    Home

page "/settings"
    Settings

page "/about"
    About
```

切換頁面：

```se
browser.go "/settings"
browser.replace "/login"
browser.back()
browser.forward()
browser.reload()
```

外部網址：

```se
browser.open "https://example.com"
```

Generated router 使用 History API。部署到 static host 時，如果使用者直接開 `/settings`，host 需要把 application route fallback 到 `index.html`。

## DOM Helpers

```se
browser.text "#status" "Saved"
browser.value "#name"
browser.set_value "#name" "SE"
browser.show "#dialog"
browser.hide "#dialog"
browser.disable "#save"
browser.enable "#save"
```

另外還有 `browser.attr`、`browser.html`、`browser.pretty`、`browser.online`。

`browser.html` 會設定 `innerHTML`；一般文字建議使用 `browser.text`。

## Async Event

Component event 會輸出成 async JavaScript handler，因此：

```se
task = browser.get "/api/data"
result = async.await task
```

會在 browser output 中真正使用 JavaScript `await`。

## Error Handling

```se
try
    task = browser.get_json "/api/data" ["throw_http": true]
    result = async.await task
    browser.text "#status" "Loaded"
else err
    browser.text "#status" err.message
```

Browser source 不應放入只適合 server 端保存的 credential；驗證與資料存取規則應由 backend 負責。

## 完整範例

```text
examples/browser-api.se
```

範例包含 GET/POST JSON、HTTP error、retry、timeout、真正 cancellation、form conversion、page navigation 與 external URL。
