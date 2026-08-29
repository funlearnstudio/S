# S standard library

S 0.2 standard modules are exposed through the runtime and selected with `use`.

```text
use file
use path
use time
```

The implementation uses C++20 facilities such as `std::filesystem` and `std::chrono` so S code does not depend on Unix-only path rules.
