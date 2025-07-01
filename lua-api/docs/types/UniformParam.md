---
layout: default
title: UniformParam
parent: Lua API
permalink: lua-api/types/UniformParam
---

{% raw %}

# class UniformParam


- supers: { [string]: (T|T[]) }




A table of uniform name to value.

The Uniforms are the values you send along with the shader-program. To use
them in the shader-program declare them like this: `uniform float frame;`

Specify a Lua array to initialize GLSL arrays.

The engine will automatically fill in an appropriately named uniform for team
colour if it is declared;

```glsl
uniform vec4 teamColor;
```

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L553-L569" target="_blank">source</a>]







---




{% endraw %}