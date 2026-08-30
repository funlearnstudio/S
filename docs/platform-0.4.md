# SE 0.4 Platform Foundation

[繁體中文版](platform-0.4-zh-TW.md)

This versioned document records the 0.4 platform stage: SE began expanding from a small executable language into a project/tooling platform while keeping **Simple at every level**.

## Contextual help

Values can expose basic help through `.help`:

```se
nums = [1, 2, 3]
say nums.help
```

Unknown-member diagnostics can direct users toward this help path.

## CLI tooling

```bash
se help
se doctor
```

`se doctor` reports useful environment/toolchain information such as version, platform, compiler/tool availability, package-home settings and current directory.

## Project scaffolding

```bash
se new app myapp
se new web mysite
```

0.4 established application/Web project structure. Later platform versions added deeper HTTP/router/browser behavior; this document should be read as a historical version stage, not the current full Web reference.

## Standard modules introduced at this stage

### math

```se
use math
say math.pi
say math.sqrt 25
```

### random

```se
use random
n = random.int 1 10
x = random.num
```

### os

```se
use os
say os.platform
say os.cwd
```

## Package-home naming

The platform migration prefers:

```text
SE_HOME/packages
```

with legacy `S_HOME/packages` fallback where compatibility remains.

## Native bridge foundation

The C ABI / `.snative` / Bytes / managed-handle / `se bind` path provides the systems interoperability layer without forcing ordinary SE programs to understand C++ ABI details.

## Design direction

0.4's broader lesson is still used by current SE: new capabilities should usually arrive as predictable modules, runtime services and tooling rather than large amounts of new syntax.

For current behavior, use the main [Documentation Index](README.md). For later historical stages, see [SE 0.5 Platform](platform-0.5.md) and [SE 0.6 Advanced Guide](advanced-0.6.md).
