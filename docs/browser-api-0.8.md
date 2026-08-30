# SE Web Browser API (0.8)

[繁體中文版](browser-api-0.8-zh-TW.md)

`se web build` injects the SE browser runtime into generated `app.js`. Component event code can therefore perform HTTP requests, send JSON/forms, navigate declared pages, cancel requests, and update common DOM state without embedding a large `js native` helper.

## 1. Requests

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

Available helpers:

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

## 2. Request options

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

The runtime handles timeouts, cancellation, offline state, network/CORS failures, JSON parse errors, HTTP errors, retryable statuses, exponential backoff, and `Retry-After`.

CORS is controlled by the server/browser security model. The runtime can report a failed network request but cannot bypass a server that does not permit the origin.

## 3. Cancellation and deduplication

Give a request a stable key:

```se
task = browser.get "/api/users" ["key": "load-users"]
```

Cancel it:

```se
browser.cancel "load-users"
```

Cancel all active keyed requests:

```se
browser.cancel_all()
```

GET/HEAD requests can deduplicate identical in-flight requests to reduce accidental duplicate traffic.

## 4. Send JSON

```se
payload = ["name": "SE", "message": "Hello"]
options = ["throw_http": true]
task = browser.post_json "/api/messages" payload options
result = async.await task
```

Use `put_json` and `patch_json` for the corresponding methods.

## 5. Forms and uploads

Read a form as JSON-compatible data:

```se
data = browser.form_json "#profile-form"
```

Submit it as JSON:

```se
task = browser.submit_json "/api/profile" "#profile-form"
result = async.await task
```

For a form containing files:

```se
task = browser.upload "/api/upload" "#upload-form"
```

## 6. Pages and navigation

Declare pages:

```se
page "/"
    Home

page "/settings"
    Settings

page "/about"
    About
```

Navigate:

```se
browser.go "/settings"
browser.replace "/login"
browser.back()
browser.forward()
browser.reload()
```

Open an external URL:

```se
browser.open "https://example.com"
```

The generated router uses the History API. Static hosting must rewrite/fallback unknown application routes such as `/settings` to `index.html` when the user loads them directly.

## 7. DOM helpers

```se
browser.text "#status" "Saved"
browser.value "#name"
browser.set_value "#name" "SE"
browser.show "#dialog"
browser.hide "#dialog"
browser.disable "#save"
browser.enable "#save"
```

Additional helpers include `browser.attr`, `browser.html`, `browser.pretty`, and `browser.online`.

`browser.html` writes `innerHTML`. Do not pass untrusted user content to it; use `browser.text` for plain text.

## 8. Async event handlers

Component event handlers are emitted as async JavaScript handlers, so:

```se
task = browser.get "/api/data"
result = async.await task
```

lowers to a real JavaScript `await` in browser output.

## 9. Error handling

```se
try
    task = browser.get_json "/api/data" ["throw_http": true]
    result = async.await task
    browser.text "#status" "Loaded"
else err
    browser.text "#status" err.message
```

Treat authentication, authorization, CSRF protection, database credentials, and server-side validation as backend responsibilities. Secrets must not be embedded in browser `.se` source.

## 10. Complete example

See:

```text
examples/browser-api.se
```

It exercises GET/POST JSON, errors, retry, timeout, real cancellation, form conversion, page navigation, and external URLs.
