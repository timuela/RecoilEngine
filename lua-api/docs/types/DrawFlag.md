---
layout: default
title: DrawFlag
parent: Lua API
permalink: lua-api/types/DrawFlag
---

{% raw %}

# alias DrawFlag
---



```lua
(alias) DrawFlag = (0|1|2|4|8|16|32|128)
    | 0 -- No Draw
    | 1 -- Opaque Pass
    | 2 -- Alpha Pass
    | 4 -- Reflection Pass
    | 8 -- Refraction Pass
    | 16 -- Shadow pass - Opaque Objects
    | 32 -- Shadow pass - Transparent Objects
    | 128 -- Icon - Possibly Radar Icons

```




Drawing Flags

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaUnsyncedRead.cpp#L2110-L2122" target="_blank">source</a>]


{% endraw %}