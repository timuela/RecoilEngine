---
layout: default
title: SendToUnsynced
parent: Lua API
permalink: lua-api/globals/SendToUnsynced
---

{% raw %}

# global SendToUnsynced

---

```lua
function SendToUnsynced(...: (boolean|number|string|table)?)
```
@param `...` - Arguments. Typically the first argument is the name of a function to call.

Argument tables will be recursively copied and stripped of unsupported types and metatables.







Invoke `UnsyncedCallins:RecvFromSynced` callin with the given arguments.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2003-L2013" target="_blank">source</a>]


{% endraw %}