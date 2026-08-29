# Collections 集合系統

本文件是 `collections.md` 的繁體中文版。

SE 內建 List、Map、Set，並由 Runtime 做基本安全檢查。

## List

```se
nums = [1, 2, 3]
nums.add 4
say nums.len
say nums[0]
```

索引超出範圍時會產生錯誤，不會靜默讀取未知記憶體。

## Map

```se
user = ["name": "SE", "score": 100]
say user["name"]
```

Map 適合儲存 key/value 資料，也可搭配 JSON 與 `collections.sort_by`。

## Set

```se
values = set [1, 2, 2, 3]
```

Set 用於保存不重複元素。

## 迭代

```se
for n in nums
    say n
```

## 高階集合操作

SE 0.6 加入：

```se
use collections

filtered = collections.filter nums predicate
mapped = collections.map nums transform
sum = collections.reduce nums 0 add
part = collections.slice nums 1 3
first = collections.take nums 2
rest = collections.drop nums 2
```

排序：

```se
ordered = collections.sort_by users "score"
desc = collections.sort_by_desc users "score"
ordered2 = collections.sort_with nums before
```

這些高階操作原則上回傳新 List，不直接修改原集合。
