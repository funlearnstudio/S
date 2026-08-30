# SE Documentation

This is the main English documentation index for SE.

> If you are new to SE, read **Getting Started → Tutorial → Language Reference**. Read **Technical Reference** when you want compiler/runtime internals.

[繁體中文文件](README-zh-TW.md)

## 1. Learn SE

- [Installation](installation.md) — installers, source builds, PATH and platform notes.
- [Getting Started](getting-started.md) — install, verify, run the first program and learn the CLI.
- [Tutorial](tutorial.md) — variables, control flow, functions, collections, types, modules, errors, async, Web and more.
- [Examples](examples.md) — runnable examples grouped by topic.
- [Testing and Project Checks](testing.md) — `se check`, `check-all`, `test`, CTest and CI principles.

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

- [Advanced SE 0.6 Guide](advanced-0.6.md)
- [Advanced SE 0.6 API](advanced-0.6-api.md)
- [Native Interoperability](native-interop.md)
- [Compiler Architecture](compiler-architecture.md)
- [Runtime Design](runtime-design.md)
- [Technical Reference](technical-reference.md)

## 5. Tooling and editor support

- [VS Code Extension](../vscode/README.md) — highlighting, IntelliSense, navigation and Run/Check/Build commands.
- [GitHub Syntax Highlighting](github-syntax-highlighting.md) — TextMate grammar and GitHub Linguist requirements.
- [Linguist Submission Bundle](../linguist/README.md) — preparation material for a future upstream Linguist contribution.

## 6. Versioned development documents

These documents describe specific development stages and are kept separate from the stable language reference:

- [SE 0.7 Type System: Structured TypeRef](type-system-0.7.md)
- [Generic User Types 0.7](generic-user-types-0.7.md)
- [Database Adapters 0.8](database-adapters-0.8.md)
- [SE Web Language 0.8](web-language-0.8.md)
- [Browser API 0.8](browser-api-0.8.md)
- [Ecosystem Runtime 0.9](ecosystem-runtime-0.9.md)
- [Depth Roadmap 0.7 → 1.0](depth-roadmap-0.7-1.0.md)
- [Roadmap](roadmap.md)

A versioned design document is not automatically a promise that every item in it is part of the current stable release.

## 7. Stable vs current source

- **Stable release:** installation and ordinary CLI usage follow the current GitHub Release.
- **Current source:** `main` may contain newer compiler/Web work while the reported language version still matches the stable line.
- **Roadmap/design:** future-stage work is labelled separately from released behavior.

## Documentation rules

1. New user-facing source uses `.se`; legacy `.s` appears only for compatibility/history.
2. Commands use `se`.
3. Stable behavior, current-source additions and roadmap/design work are labelled separately.
4. Examples should match the implementation revision they are stored with.
5. English and Traditional Chinese primary guides use matching structure and terminology.
6. Each topic has one canonical guide/reference; older duplicate entry points link to the canonical document instead of copying it.
