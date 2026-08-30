# SE Web Language (0.8)

[繁體中文版](web-language-0.8-zh-TW.md)

SE Web keeps HTML, CSS and browser behavior recognizable while expressing common structure with SE indentation and components.

> One language, not necessarily one file.

## Components

A Web component reuses `make`:

```se
make Button text
    html
        button text

    css
        padding 12
        border_radius 8

    js
        when click
            say text
```

Use it like a normal SE call:

```se
Button "Save"
Button "Cancel"
```

The compiler associates the `html`, `css`, and `js` sections with the component and creates internal instance identities for event binding and scoped styling.

Zero-argument components are also valid:

```se
make Navbar
    html
        nav "SE"

page "/"
    Navbar
```

## HTML

```se
html
    main
        h1 "Hello"
        p "Welcome"
        button "Start"
```

HTML tags remain ordinary Web concepts rather than becoming a large set of special lexer keywords.

## CSS

```se
css
    padding 12
    border_radius 8
    background "white"

    button:hover
        opacity 0.8
```

SE-style underscore property names map to CSS dashes, such as `border_radius` → `border-radius`. Numeric values use the compiler's CSS value rules, including px defaults where appropriate. Component CSS is scoped to the component.

## Browser behavior

```se
js
    when click
        count += 1
        if count >= 10
            say "10+"
        else if count >= 5
            say "5+"
        else
            say count
```

Browser event lowering supports common SE control flow and expressions, including assignments, conditionals, loops, error handling, value matching, collection expressions, function/member calls and browser async operations supported by the current compiler.

## Pages

```se
page "/"
    Home

page "/settings"
    Settings

page "/about"
    About
```

The generated browser router can switch declared pages with the History API. See [Browser API](browser-api-0.8.md) for `browser.go`, `browser.replace`, back/forward, HTTP requests and forms.

## Build

```bash
se web build app.se dist
```

Generated files:

```text
dist/
  index.html
  style.css
  app.js
  app.ts
```

The output uses ordinary Web files. Inspecting generated HTML/CSS/JS is intentionally part of the learning model.

## Native escape hatch

For platform features not yet mapped by SE Web:

```se
html
    native "<dialog open>Advanced HTML</dialog>"
```

```se
css
    native "accent-color: auto;"
```

```se
js
    native "console.info('native browser code')"
```

Native Web code follows normal HTML/CSS/JavaScript safety rules and is the developer's responsibility.

## Browser/server boundary

Do not put database passwords, API secrets, or server-only credentials into browser source.

```text
SE browser components
    ↓ HTTPS / API
SE backend
    ↓
database / external services
```

Server-only APIs such as filesystem access, native FFI, or server listeners do not become browser JavaScript automatically.

## Legacy Web DSL

The earlier `ui.*` authoring path remains available for compatibility. New component source uses `make` + `html/css/js/page`; existing `ui.*` programs do not need to be rewritten immediately.

## Current boundaries

SE Web is an evolving compiler target. Generated `app.ts` is a TypeScript-compatible companion output rather than a promise that every SE type annotation is preserved exactly. Hosting configuration is still responsible for History API route fallback. Advanced browser platform features may still require `native` code.

For request handling, JSON/forms, cancellation and navigation, continue with [Browser API](browser-api-0.8.md).
