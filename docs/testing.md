# SE project checks and tests

SE 0.4 adds project-wide tooling so larger applications do not have to check files one by one.

## Check a whole project

```sh
se check-all .
```

SE recursively finds `.se` files (and legacy `.s` files), parses them, runs the static checker, prints `PASS` / `FAIL`, and returns a non-zero exit code when any file fails. Build folders, `.git`, and `node_modules` are ignored.

You can also check a subdirectory:

```sh
se check-all backend
```

## Test runner

Test files end in `_test.se`:

```se
value = 2 + 2
if value != 4
    fail "2 + 2 should be 4"
```

Run all tests recursively:

```sh
se test .
```

A test passes when it type-checks and finishes without an SE error or runtime failure. `fail "message"` makes a test fail. The runner prints a final passed/failed summary and returns a non-zero exit code if any test fails, so it can be used in CI.

## New projects

`se new app NAME` now creates:

```text
NAME/
├── src/main.se
└── tests/main_test.se
```

`se new web NAME` creates a backend test folder as well as the HTML/CSS/JavaScript/TypeScript frontend scaffold.

This is the first testing layer. Future versions can add a richer `test` standard module with assertions, fixtures, setup/teardown, filtering, and test metadata while keeping simple tests valid.
