# SE Testing and Project Checks

[繁體中文版](testing-zh-TW.md)

SE provides project-wide static checking and a lightweight test runner so larger projects do not need to validate files one by one.

## Check one file

```bash
se check app.se
```

## Check a source tree

```bash
se check-all .
```

You can also check a subdirectory:

```bash
se check-all backend
```

The command recursively discovers SE source, skips common generated/dependency directories, reports pass/fail results and returns a failing exit code when checks fail, making it suitable for CI.

## Test files

Test files conventionally end in `_test.se`.

```se
use test

test.equal 4 2 + 2
test.ok true
```

A simple test may also call `fail` directly when a condition is wrong.

## Run tests

```bash
se test .
```

The test runner recursively finds test files, checks/runs them, prints a summary and returns a non-zero exit code when any test fails.

## Project scaffolding

`se new app NAME` creates an application structure with source and tests. `se new web NAME` creates the Web/project scaffold supported by the current CLI.

## Compiler regression tests

The SE implementation itself also uses CMake/CTest for lower-level regression coverage:

```bash
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Implementation-level coverage includes lexer/parser/checker behavior, interpreter execution, native backend parity, native ABI/Bytes/resource lifetime, Web builds and CLI smoke tests.

## CI principle

A change is not considered validated only because the compiler builds. Language changes should also have focused regression tests that demonstrate the intended SE behavior.
