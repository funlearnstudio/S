# SE Time and Durations

[繁體中文版](time-zh-TW.md)

SE provides duration literals and a `time` module for simple runtime timing tasks.

## Duration literals

```se
wait 500ms
wait 2s
wait 1min
```

Supported literal units include milliseconds (`ms`), seconds (`s`) and minutes (`min`). Negative durations are not valid for `wait`.

## Current time

```se
use time

now = time.now
say now
```

`time.now` returns the runtime time value provided by the current implementation.

## Runtime model

The implementation uses C++ chrono-based timing internally. Ordinary SE code works with SE duration/time values rather than C++ chrono types.

## Scope

The current time API is intentionally small. It is suitable for waits, retry delays, teaching programs and lightweight timing. A full calendar/time-zone/date-formatting framework should not be assumed unless documented by a later version.
