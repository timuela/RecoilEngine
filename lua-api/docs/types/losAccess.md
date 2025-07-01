---
layout: default
title: losAccess
parent: Lua API
permalink: lua-api/types/losAccess
---

{% raw %}

# class losAccess





Parameters for los access


If one condition is fulfilled all beneath it are too (e.g. if an unit is in
LOS it can read params with `inradar=true` even if the param has
`inlos=false`) All GameRulesParam are public, TeamRulesParams can just be
`private`,`allied` and/or `public` You can read RulesParams from any Lua
environments! With those losAccess policies you can limit their access.

All GameRulesParam are public, TeamRulesParams can just be `private`,`allied` and/or `public`
You can read RulesParams from any Lua environments! With those losAccess policies you can limit their access.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaSyncedCtrl.cpp#L1362-L1382" target="_blank">source</a>]







---



## fields
---

### losAccess.private
---
```lua
losAccess.private : boolean?
```



only readable by the ally (default)








### losAccess.allied
---
```lua
losAccess.allied : boolean?
```



readable by ally + ingame allied








### losAccess.inlos
---
```lua
losAccess.inlos : boolean?
```



readable if the unit is in LOS








### losAccess.inradar
---
```lua
losAccess.inradar : boolean?
```



readable if the unit is in AirLOS








### losAccess.public
---
```lua
losAccess.public : boolean?
```



readable by all










{% endraw %}