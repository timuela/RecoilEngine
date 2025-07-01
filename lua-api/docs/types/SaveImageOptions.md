---
layout: default
title: SaveImageOptions
parent: Lua API
permalink: lua-api/types/SaveImageOptions
---

{% raw %}

# class SaveImageOptions





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6390-L6396" target="_blank">source</a>]







---



## fields
---

### SaveImageOptions.alpha
---
```lua
SaveImageOptions.alpha : boolean
```



(Default: `false`)








### SaveImageOptions.yflip
---
```lua
SaveImageOptions.yflip : boolean
```



(Default: `true`)








### SaveImageOptions.grayscale16bit
---
```lua
SaveImageOptions.grayscale16bit : boolean
```



(Default: `false`)








### SaveImageOptions.readbuffer
---
```lua
SaveImageOptions.readbuffer : GL {
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



(Default: current read buffer)










{% endraw %}