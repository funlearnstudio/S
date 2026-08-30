# SE Web Browser API（0.8）

SE Web 現在內建瀏覽器端的 `browser` runtime。`se web build` 會把 runtime 一起輸出到 `app.js`，因此一般 API 請求、JSON 傳送、頁面切換、表單讀取與常見 DOM 操作不需要再寫 `js native`。

## HTTP / HTTPS

```se
make LoadData
    html
        button "Load"

    js
        when click
            try
                task = browser.get_json "https://example.com/api/data" ["timeout": 8000, "retries": 2, "throw_http": true]
                result = async.await task
                say result.status
                say result.data
            else err
                say err.message
```

支援：

- `browser.request`
- `browser.get`
- `browser.get_json`
- `browser.post`
- `browser.post_json`
- `browser.put_json`
- `browser.patch_json`
- `browser.delete`
- `browser.submit_json`
- `browser.upload`

request option 可使用 Map 傳入：

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

Browser runtime 會處理 timeout、AbortController cancellation、offline、一般 network/CORS failure、JSON parse error、HTTP error、429、408、425、500、502、503、504 retry，以及 `Retry-After`。

GET / HEAD 預設會對相同 request key 做 in-flight deduplication，避免使用者連點造成同一請求重複送出。

## 取消請求

```se
browser.cancel "load-users"
browser.cancel_all()
```

要讓 request 可以用固定名稱取消，傳入 `key`：

```se
task = browser.get "/api/users" ["key": "load-users", "timeout": 10000]
```

`examples/browser-api.se` 也有真正的 slow request，可以在請求進行中按另一個按鈕呼叫 `browser.cancel "slow-request"`，確認 AbortController 會實際中止底層 request。

## JSON 傳送

```se
payload = ["name": "SE", "message": "Hello"]
options = ["throw_http": true]
task = browser.post_json "/api/messages" payload options
result = async.await task
```

## 表單

```se
data = browser.form_json "#profile-form"
task = browser.post_json "/api/profile" data
result = async.await task
```

也可以直接：

```se
task = browser.submit_json "/api/profile" "#profile-form"
```

含檔案的 `<form>` 可以交給：

```se
task = browser.upload "/api/upload" "#upload-form"
```

## 多頁面

同一個 SE Web source 可以有多個 `page`：

```se
page "/"
    Home

page "/settings"
    Settings

page "/about"
    About
```

build 仍然輸出標準 `index.html` / `style.css` / `app.js` / `app.ts`。頁面由 generated browser router 管理，並使用 History API。

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

同站且對應到已宣告 `page` 的一般 `<a href>` 也會被 router 接管，不需要整頁重新載入。

直接在靜態主機輸入 `/settings` 時，主機仍需要把未知 route rewrite/fallback 到 `index.html`；這是一般 History API SPA 的部署需求，不是 SE 專用格式。

## DOM helper

常見 UI 更新可以直接從 SE event 使用：

```se
browser.text "#status" "Saved"
browser.value "#name"
browser.set_value "#name" "SE"
browser.show "#dialog"
browser.hide "#dialog"
browser.disable "#save"
browser.enable "#save"
```

另有 `browser.attr`、`browser.html`、`browser.pretty`、`browser.online`。

`browser.html` 會直接設定 `innerHTML`，因此不要把未信任的使用者輸入直接交給它；一般文字請使用 `browser.text`。

## Browser event lowering

Component event handler 會輸出成 `async` JavaScript handler，因此：

```se
task = browser.get "/api/data"
result = async.await task
```

會真正降低為 JavaScript `await`，不是回傳未完成的 Promise。

Browser event 目前也支援 assignment、`if` / `else if` / `else`、`repeat`、`while`、`for`、`try` / `else`、value-based `match`、`give`、`fail`、List / Map / Set / Index / Range expression，以及一般 function/member call。

## 完整範例

請看：

```text
examples/browser-api.se
```

這個單檔範例包含 `/`、`/api`、`/send`、`/about`，並測試 GET JSON、POST JSON、HTTP error、retry、timeout、實際取消、表單轉 JSON、頁面導航與外部頁面。
