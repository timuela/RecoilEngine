---
layout: default
title: CallAsTeam
parent: Lua API
permalink: lua-api/globals/CallAsTeam
---

{% raw %}

# global CallAsTeam

---

```lua
function CallAsTeam(
  teamID: integer,
  func: fun(...),
  ...: any
) -> The any ...
```
@param `teamID` - Team ID.

@param `func` - The function to call.

@param `...` - Arguments to pass to the function.


@return `The` - return values of the function.






Calls a function from given team's PoV. In particular this makes callouts obey that team's visibility rules.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2526-L2533" target="_blank">source</a>]


{% endraw %}