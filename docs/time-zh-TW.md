# 時間與 Duration

本文件是 `time.md` 的繁體中文版。

SE 提供 Duration literal 與 `time` module。

## Duration

```se
wait 500ms
wait 2s
wait 1min
```

支援單位：

- `ms`：毫秒
- `s`：秒
- `min`：分鐘

`wait` 不接受負 Duration。

## time.now

```se
use time

now = time.now
say now
```

`time.now` 回傳 Runtime 的 Time value。

## 用途

Duration 適合重試間隔、簡單動畫/輪詢、CLI 等待與教學程式。更完整的日期格式化、時區與 calendar API 尚不是目前 SE time module 的完整能力範圍。
