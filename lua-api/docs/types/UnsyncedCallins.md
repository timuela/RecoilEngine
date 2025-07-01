---
layout: default
title: UnsyncedCallins
parent: Lua API
permalink: lua-api/types/UnsyncedCallins
---

{% raw %}

# class UnsyncedCallins





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L178-L181" target="_blank">source</a>]

Functions called by the Engine (Unsynced).







---

## methods
---

### UnsyncedCallins.RecvFromSynced
---
```lua
function UnsyncedCallins.RecvFromSynced(...: any)
```





Receives data sent via `SendToUnsynced` callout.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L189-L193" target="_blank">source</a>]








### UnsyncedCallins.DrawUnit
---
```lua
function UnsyncedCallins.DrawUnit(
  unitID: integer,
  drawMode: number
) -> suppressEngineDraw boolean
```





For custom rendering of units

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L220-L227" target="_blank">source</a>]



@deprecated Deprecated






### UnsyncedCallins.DrawFeature
---
```lua
function UnsyncedCallins.DrawFeature(
  featureID: integer,
  drawMode: number
) -> suppressEngineDraw boolean
```





For custom rendering of features

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L257-L264" target="_blank">source</a>]



@deprecated Deprecated






### UnsyncedCallins.DrawShield
---
```lua
function UnsyncedCallins.DrawShield(
  featureID: integer,
  weaponID: integer,
  drawMode: number
) -> suppressEngineDraw boolean
```





For custom rendering of shields.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L293-L301" target="_blank">source</a>]



@deprecated Deprecated






### UnsyncedCallins.DrawProjectile
---
```lua
function UnsyncedCallins.DrawProjectile(
  projectileID: integer,
  drawMode: number
) -> suppressEngineDraw boolean
```





For custom rendering of weapon (& other) projectiles

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L332-L339" target="_blank">source</a>]



@deprecated Deprecated






### UnsyncedCallins.DrawMaterial
---
```lua
function UnsyncedCallins.DrawMaterial(
  uuid: integer,
  drawMode: number
) -> suppressEngineDraw boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L370-L377" target="_blank">source</a>]



@deprecated Deprecated










{% endraw %}