# SE 0.4 platform direction

SE 0.4 starts turning the language from a small native language into a broader application platform while keeping the rule: **simple at every level**.

## Implemented on the `se-0.4-platform` branch

### Built-in help

Every value can expose contextual help through `.help`.

```se
nums = [1, 2, 3]
nums.help
```

The runtime reports the value type and useful members. Unknown-member diagnostics also point users toward `.help`.

### CLI help and diagnostics

```sh
se help
se doctor
```

`se doctor` reports the SE version, platform, C++ compiler selection, package-home environment, and current directory.

### General application projects

```sh
se new app my-app
```

Creates a normal SE application with `src/main.se` and a minimal README.

### Web project layout

```sh
se new web my-site
```

Creates a project containing:

- `backend/main.se`
- `frontend/index.html`
- `frontend/style.css`
- `frontend/app.js`
- `frontend/app.ts`

This gives SE projects a stable layout for combining an SE backend with HTML, CSS, JavaScript, and TypeScript. The built-in HTTP server/API runtime is **not implemented yet**; the project generator deliberately says so instead of pretending the backend is already complete.

### Standard modules added in this branch

`math` provides `pi`, `sqrt`, `abs`, `floor`, `ceil`, `round`, `pow`, `min`, and `max`.

`random` provides inclusive `random.int min max` and zero-argument `random.num` for a value from 0 to 1.

`os` provides `platform`, `cwd`, `getenv`, and `has_env`.

The checker knows their signatures, so these modules participate in SE static checking rather than being runtime-only helpers.

### Package-home migration

The module loader now prefers `SE_HOME/packages` and falls back to the legacy `S_HOME/packages` location.

### Existing native-language bridge

SE already has `.snative`, a C ABI bridge, Bytes interop, opaque native handles, cleanup support, and `se bind`. This is the base for calling libraries written in C and C++ and for creating adapters to other languages.

## Next language-level features

To reach a Python/C++-like advanced ceiling without making beginner syntax heavy, SE should add advanced capabilities in layers:

1. generics / type parameters
2. interfaces or traits
3. iterators and richer collection operations
4. closures and first-class functions
5. async / await and tasks
6. concurrency primitives
7. richer pattern matching
8. enums / tagged unions
9. explicit low-level and `unsafe` facilities for systems work
10. stronger ownership/lifetime rules where native resource control is needed

These should remain optional: simple programs should not need to understand them.

## Standard-library groups

Existing core modules include `file`, `path`, and `time`. SE 0.4 adds the first `math`, `random`, and `os` APIs. The next standard-library groups are planned as modules instead of hundreds of global functions:

- `text` — text processing
- `collections` — collection helpers
- `json` — JSON parse/stringify
- `process` — child processes
- `net` — sockets and network primitives
- `http` — HTTP client/server
- `web` — routing, requests, responses, static files, JSON APIs
- `async` — tasks, timers, asynchronous I/O
- `test` — assertions and test runner
- `crypto` — safe high-level hashing/encoding APIs

The implementation rule is that libraries can be complex internally while their public SE API remains short and predictable.

## Web target

The intended stack is:

```text
HTML / CSS / JS / TS
        ↕
     web bridge
        ↕
   SE backend/API
        ↕
 HTTP / JSON / files / database adapters
```

Longer-term, browser-side SE can target WebAssembly while still allowing normal HTML/CSS/JS/TS projects.

## Other-language interop

Interop should grow in layers:

```text
SE
├─ C ABI      (existing foundation)
├─ C++        (through C ABI wrappers / generated bindings)
├─ Python     (planned adapter)
├─ JavaScript (planned native/Wasm bridge)
├─ TypeScript (planned typed JS bridge definitions)
├─ Rust       (C ABI adapter)
├─ Go         (C ABI adapter)
└─ Java/.NET  (FFI/JNI/PInvoke-style adapters)
```

The compiler should keep one stable ABI boundary instead of implementing a completely separate foreign-function system for every language.

## Diagnostics direction

SE errors should try to answer three questions:

1. What happened?
2. Where did it happen?
3. What can I do next?

The `.help` system and member guidance are the first step. Future diagnostics should add spelling suggestions, fix-its, unused-variable warnings, unreachable-code detection, suspicious-condition detection, and optional strict linting.

## Application ceiling

The long-term target is one language usable for:

- beginner scripts
- CLI tools
- desktop applications
- backend services
- web projects
- games
- data / AI applications
- native libraries
- embedded and systems-oriented code through explicit low-level facilities

This document is a roadmap. Only items explicitly listed in the **Implemented** section should be treated as present in the current branch.
