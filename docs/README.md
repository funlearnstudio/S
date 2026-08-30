# SE Documentation

This is the main English documentation index for SE.

> If you are new to SE, read **Getting Started → Tutorial → Language Reference**. Read **Technical Reference** when you want compiler/runtime internals.

[繁體中文文件](README-zh-TW.md)

## 1. Learn SE

- [Installation](installation.md) — installers, source builds, PATH and platform notes.
- [Getting Started](getting-started.md) — install, verify, run the first program and learn the CLI.
- [Tutorial](tutorial.md) — variables, control flow, functions, collections, types, modules, errors, async, Web and more.
- [Examples](examples.md) — runnable examples grouped by topic.

## 2. Language reference

- [Language Reference](language-reference.md)
- [Syntax Reference](syntax-reference.md)
- [Types and Methods](types.md)
- [Conversions](conversions.md)
- [Collections](collections.md)
- [Modules](modules.md)
- [Errors](errors.md)
- [Files](files.md)
- [Paths](paths.md)
- [Time](time.md)

## 3. Web development

- [SE Web Language](web-language-0.8.md) — components, HTML/CSS/JS sections, pages and generated output.
- [Browser API](browser-api-0.8.md) — HTTP requests, JSON, forms, routing, cancellation and DOM helpers.
- [Web example](../examples/component-web.se)
- [Browser API example](../examples/browser-api.se)

## 4. Advanced and systems topics

- [Advanced SE 0.6 API](advanced-0.6-api.md)
- [Generic User Types](generic-user-types-0.7.md)
- [Native Interoperability](native-interop.md)
- [Compiler Architecture](compiler-architecture.md)
- [Runtime Design](runtime-design.md)
- [Technical Reference](technical-reference.md)

## 5. Stable vs versioned design work

The stable public release and the repository's versioned design documents are not the same thing.

- **Stable release:** installation and ordinary CLI usage follow the current GitHub Release.
- **Current source:** `main` may contain newer compiler/Web work while the reported language version still matches the stable line.
- **Versioned design documents:** files labelled 0.7/0.8/0.9/1.0 describe the stage named in the document; roadmap material is not automatically a stable feature.

See [Roadmap](roadmap.md) for planned work.

## Documentation rules

To keep the documentation consistent:

1. New user-facing source uses `.se`; legacy `.s` appears only for compatibility/history.
2. Commands use `se`.
3. Stable behavior, current-source additions and roadmap/design work are labelled separately.
4. Examples should match the implementation revision they are stored with.
5. English and Traditional Chinese primary guides use matching structure and terminology.
6. Each topic has one canonical guide/reference; older duplicate entry points link to the canonical document instead of copying it.
