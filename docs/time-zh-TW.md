# SE 時間與 Duration

[English version](time.md)

SE 提供 Duration literal 與 `time` module，處理簡單的 runtime timing 工作。

## Duration Literal

```se
wait 500ms
wait 2s
wait 1min
```

目前常用單位包含毫秒 `ms`、秒 `s`、分鐘 `min`。`wait` 不接受負 Duration。

## Current Time

```se
use time

now = time.now
say now
```

`time.now` 回傳目前 implementation 提供的 Runtime Time value。

## Runtime Model

底層實作使用 C++ chrono-based timing；一般 SE code 使用 SE 自己的 Duration / Time value，不需要接觸 C++ chrono type。

## 目前能力範圍

目前 time API 刻意維持精簡，適合等待、retry delay、教學程式與輕量 timing。除非後續版本文件明確加入，否則不應把它描述成完整 calendar / timezone / date-formatting framework。
