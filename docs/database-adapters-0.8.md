# SE Database Adapters (0.8 foundation)

[繁體中文版](database-adapters-0.8-zh-TW.md)

This versioned document describes the 0.8 database-adapter foundation. SE keeps the local `db.open` / `db.set` / `db.get` text key/value store while adding a shared adapter model for remote data services.

## Unified adapter interface

```se
use db

connection = try db.connect adapter endpoint database collection
result = try db.exec connection action payload
say db.adapter connection
```

The goal is to keep database differences inside adapters/runtime rather than inventing unrelated top-level syntax for every backend.

## MongoDB adapter

Conceptually:

```se
use db

mongo = try db.connect "mongodb" uri "myapp" "users"
user = try db.exec mongo "find_one" query_json
```

Supported actions in this stage include find, insert, update/replace, delete and count operations documented by the implementation.

The current MongoDB execution path depends on Node.js and the official `mongodb` package. `db.connect` creates a connection description; actual remote work occurs when executing an action.

## Google Apps Script adapter

A deployed Apps Script Web App can be used as an HTTPS adapter endpoint:

```se
store = try db.connect "gas" endpoint
result = try db.exec store "list" "{}"
```

The adapter sends an action/payload envelope and receives the service response through the HTTPS transport.

## Server/browser boundary

Database connection details belong on the backend side of an application.

```text
SE browser app
    ↓ HTTPS / API
SE backend
    ↓ db.connect / db.exec
remote database/service
```

Do not assume browser-generated JavaScript can safely contain backend connection information.

## Current boundaries

- Remote payload/result exchange is JSON-text oriented in this foundation.
- MongoDB depends on the external Node.js driver rather than an embedded C++ driver.
- GAS transport depends on the HTTPS/curl path.
- CI validates adapter/runtime behavior without requiring live third-party accounts.

Future adapters can reuse the same `db.connect` / `db.exec` model instead of adding another language syntax layer.
