# `time` Module / `time` 模組

SE provides runtime time values through `time`, while duration literals are part of the language.

SE 透過 `time` module 提供 Runtime time value；Duration literal 則屬於語言本身。

```se
use time

now = time.now
say now

wait 500ms
wait 2s
wait 1min
```

Duration suffixes / Duration 單位：

```text
ms   milliseconds / 毫秒
s    seconds / 秒
min  minutes / 分鐘
```

For the full user-facing reference / 完整使用說明：

- [Time and Durations](../../docs/time.md)
- [時間與 Duration](../../docs/time-zh-TW.md)
