# Time and durations

Duration literals are real S values rather than strings:

```text
wait 500ms
wait 2s
wait 1min
```

Current time comes from the standard module:

```text
use time

now = time.now
say now
```

The runtime uses `std::chrono`. `time.now` prints a stable UTC ISO-style representation in S 0.2.
