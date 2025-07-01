---
layout: default
title: RBO
parent: Lua API
permalink: lua-api/types/RBO
---

{% raw %}

# class RBO





User Data RBO

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaRBOs.cpp#L139-L148" target="_blank">source</a>]







---



## fields
---

### RBO.target
---
```lua
RBO.target : GL {
    POINTS: integer,
    LINES: integer,
    LINE_LOOP: integer,
    LINE_STRIP: integer,
    TRIANGLES: integer,
    TRIANGLE_STRIP: integer,
    TRIANGLE_FAN: integer,
    QUADS: integer,
    QUAD_STRIP: integer,
    POLYGON: integer,
    LINE_STRIP_ADJACENCY: integer,
    LINES_ADJACENCY: integer,
    ...(+259)
}
```










### RBO.format
---
```lua
RBO.format : GL {
    POINTS: integer,
    LINES: integer,
    LINE_LOOP: integer,
    LINE_STRIP: integer,
    TRIANGLES: integer,
    TRIANGLE_STRIP: integer,
    TRIANGLE_FAN: integer,
    QUADS: integer,
    QUAD_STRIP: integer,
    POLYGON: integer,
    LINE_STRIP_ADJACENCY: integer,
    LINES_ADJACENCY: integer,
    ...(+259)
}
```










### RBO.xsize
---
```lua
RBO.xsize : integer
```










### RBO.ysize
---
```lua
RBO.ysize : integer
```










### RBO.valid
---
```lua
RBO.valid : boolean
```










### RBO.samples
---
```lua
RBO.samples : integer
```



will return globalRendering->msaaLevel for multisampled RBO or 0 otherwise










{% endraw %}