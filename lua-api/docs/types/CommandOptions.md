---
layout: default
title: CommandOptions
parent: Lua API
permalink: lua-api/types/CommandOptions
---

{% raw %}

# class CommandOptions





Full command options object for reading from a `Command`.

Note that this has extra fields `internal` and `coded` that are not supported
when creating a command from Lua.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaUtils.cpp#L931-L945" target="_blank">source</a>]







---



## fields
---

### CommandOptions.coded
---
```lua
CommandOptions.coded : (CommandOptionBit|integer)
```



Bitmask of command options.








### CommandOptions.alt
---
```lua
CommandOptions.alt : boolean
```



Alt key pressed.








### CommandOptions.ctrl
---
```lua
CommandOptions.ctrl : boolean
```



Ctrl key pressed.








### CommandOptions.shift
---
```lua
CommandOptions.shift : boolean
```



Shift key pressed.








### CommandOptions.right
---
```lua
CommandOptions.right : boolean
```



Right mouse key pressed.








### CommandOptions.meta
---
```lua
CommandOptions.meta : boolean
```



Meta key (space) pressed.








### CommandOptions.internal
---
```lua
CommandOptions.internal : boolean
```












{% endraw %}