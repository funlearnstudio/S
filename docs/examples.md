# SE Examples

[繁體中文版](examples-zh-TW.md)

Runnable examples live in `examples/`. Use this page as a map rather than duplicating every source file into the documentation.

## Language basics

- `examples/ask.se` — input.
- `examples/function.se` — functions.
- `examples/collections.se` — List/Map/Set basics.
- `examples/errors.se` — recoverable errors.
- `examples/files.se` — file operations.
- `examples/advanced.se` — broader language/runtime features.
- `examples/modern-control.se` — newer control-flow examples.

All primary examples use `.se`. Legacy `.s` support remains only for compatibility testing and migration paths.

## Web

- `examples/component-web.se` — `make` + `html/css/js/page` component model.
- `examples/browser-api.se` — browser requests, JSON sending, cancellation, forms and multi-page navigation.
- `examples/web-app.se` — Web example using the supported Web path.

Build a component Web example:

```bash
se web build examples/browser-api.se dist
```

When using an uninstalled source build:

```bash
./build/se web build examples/browser-api.se dist
```

## Backend / platform

- `examples/database-adapters.se` — database adapter examples.
- `examples/ecosystem.se` — runtime/ecosystem features.
- `examples/game.se` — game/runtime example.

## How to use examples

Run an SE program:

```bash
se run examples/advanced.se
```

Static check:

```bash
se check examples/advanced.se
```

If an example exists on GitHub `main` but not in your local checkout, update the repository first:

```bash
git switch main
git pull --ff-only
```

Examples document the implementation revision they are stored with; versioned design documents may describe future or newer-stage capabilities separately.
