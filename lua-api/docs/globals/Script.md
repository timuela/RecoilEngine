---
layout: default
title: Script
parent: Lua API
permalink: lua-api/globals/Script
---

{% raw %}

# global Script








---

## methods
---

### Script.GetWatchUnit
---
```lua
function Script.GetWatchUnit(unitDefID: integer) -> watched boolean
```

@return `watched` - Watch status.





Query whether any callins are registered for a unitDefID.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2175-L2183" target="_blank">source</a>]





@see Script.SetWatchUnit




### Script.GetWatchFeature
---
```lua
function Script.GetWatchFeature(featureDefID: integer) -> watched boolean
```

@return `watched` - `true` if callins are registered, otherwise `false`.





Query whether any callins are registered for a featureDefID.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2188-L2196" target="_blank">source</a>]





@see Script.SetWatchFeature




### Script.GetWatchWeapon
---
```lua
function Script.GetWatchWeapon(weaponDefID: integer) -> watched boolean
```

@return `watched` - True if watch is enabled for any weaponDefID callins.





Query whether any callins are registered for a weaponDefID.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2201-L2214" target="_blank">source</a>]

Same as calling:
```lua
Script.GetWatchExplosion(weaponDefID) or Script.GetWatchProjectile(weaponDefID) or Script.GetWatchAllowTarget(weaponDefID)
```





@see Script.SetWatchWeapon




### Script.GetWatchExplosion
---
```lua
function Script.GetWatchExplosion(weaponDefID: integer) -> watched boolean
```

@return `watched` - `true` if callins are registered, otherwise `false`.





Query whether explosion callins are registered for a weaponDefID.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2216-L2224" target="_blank">source</a>]





@see Script.SetWatchExplosion




### Script.GetWatchProjectile
---
```lua
function Script.GetWatchProjectile(weaponDefID: integer) -> watched boolean
```

@return `watched` - `true` if callins are registered, otherwise `false`.





Query whether projectile callins are registered for a weaponDefID.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2230-L2238" target="_blank">source</a>]





@see Script.SetWatchProjectile




### Script.GetWatchAllowTarget
---
```lua
function Script.GetWatchAllowTarget(weaponDefID: integer) -> watched boolean
```

@return `watched` - `true` if callins are registered, otherwise `false`.





Query whether weapon targeting callins are registered for a weaponDefID.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2243-L2251" target="_blank">source</a>]





@see Script.SetWatchAllowTarget




### Script.SetWatchUnit
---
```lua
function Script.SetWatchUnit(
  unitDefID: integer,
  watch: boolean
)
```
@param `watch` - Whether to register or deregister.






Register or deregister unitDefID for expensive callins.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2256-L2267" target="_blank">source</a>]





@see Callins:UnitMoveFailed




### Script.SetWatchFeature
---
```lua
function Script.SetWatchFeature(
  featureDefID: integer,
  watch: boolean
)
```
@param `watch` - Whether to register or deregister.






Register or deregister featureDefID for expensive callins.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2272-L2281" target="_blank">source</a>]





@see Callins:UnitFeatureCollision




### Script.SetWatchWeapon
---
```lua
function Script.SetWatchWeapon(
  weaponDefID: integer,
  watch: boolean
)
```
@param `watch` - Whether to register or deregister.






Register or deregister weaponDefID for all expensive callins.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2286-L2307" target="_blank">source</a>]

Equivalent to calling:

```lua
Script.SetWatchExplosion(weaponDefID)
Script.SetWatchProjectile(weaponDefID)
Script.SetWatchAllowTarget(weaponDefID)
```

Generally it's better to use those methods to avoid registering uneeded callins.





@see Script.SetWatchAllowTarget




### Script.SetWatchExplosion
---
```lua
function Script.SetWatchExplosion(
  weaponDefID: integer,
  watch: boolean
)
```
@param `watch` - Whether to register or deregister.






Register or deregister weaponDefID for explosion callins.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2309-L2318" target="_blank">source</a>]





@see Callins:Explosion




### Script.SetWatchProjectile
---
```lua
function Script.SetWatchProjectile(
  weaponDefID: integer,
  watch: boolean
)
```
@param `weaponDefID` - weaponDefID for weapons or -1 to watch for debris.

@param `watch` - Whether to register or deregister.






Register or deregister weaponDefID for expensive projectile callins.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2324-L2334" target="_blank">source</a>]





@see Callins:ProjectileDestroyed




### Script.SetWatchAllowTarget
---
```lua
function Script.SetWatchAllowTarget(
  weaponDefID: integer,
  watch: boolean
)
```
@param `watch` - Whether to register or deregister.






Register or deregister weaponDefID for weapon targeting callins.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandleSynced.cpp#L2339-L2350" target="_blank">source</a>]





@see SyncedCallins:AllowWeaponInterceptTarget




### Script.Kill
---
```lua
function Script.Kill(killMessage: string?)
```
@param `killMessage` - Kill message.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L227-L230" target="_blank">source</a>]








### Script.UpdateCallin
---
```lua
function Script.UpdateCallin(name: string)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L683-L686" target="_blank">source</a>]








### Script.GetName
---
```lua
function Script.GetName() -> name string
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4129-L4132" target="_blank">source</a>]








### Script.GetSynced
---
```lua
function Script.GetSynced() -> synced boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4140-L4143" target="_blank">source</a>]








### Script.GetFullCtrl
---
```lua
function Script.GetFullCtrl() -> fullCtrl boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4151-L4154" target="_blank">source</a>]








### Script.GetFullRead
---
```lua
function Script.GetFullRead() -> fullRead boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4162-L4165" target="_blank">source</a>]








### Script.GetCtrlTeam
---
```lua
function Script.GetCtrlTeam() -> teamID integer
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4173-L4176" target="_blank">source</a>]








### Script.GetReadTeam
---
```lua
function Script.GetReadTeam() -> teamID integer
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4184-L4187" target="_blank">source</a>]








### Script.GetReadAllyTeam
---
```lua
function Script.GetReadAllyTeam() -> allyTeamID integer
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4195-L4198" target="_blank">source</a>]








### Script.GetSelectTeam
---
```lua
function Script.GetSelectTeam() -> teamID integer
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4206-L4209" target="_blank">source</a>]








### Script.GetGlobal
---
```lua
function Script.GetGlobal() -> global integer?
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4217-L4220" target="_blank">source</a>]








### Script.GetRegistry
---
```lua
function Script.GetRegistry() -> registry integer?
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4231-L4234" target="_blank">source</a>]








### Script.DelayByFrames
---
```lua
function Script.DelayByFrames(
  frameDelay: integer,
  fun: unknown
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4251-L4255" target="_blank">source</a>]








### Script.IsEngineMinVersion
---
```lua
function Script.IsEngineMinVersion(
  minMajorVer: integer,
  minMinorVer: integer?,
  minCommits: integer?
) -> satisfiesMin boolean
```
@param `minMinorVer` - (Default: `0`)

@param `minCommits` - (Default: `0`)


@return `satisfiesMin` - `true` if the engine version is greater or equal to the specified version, otherwise `false`.





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaUtils.cpp#L527-L533" target="_blank">source</a>]











## fields
---

### Script.NO_ACCESS_TEAM
---
```lua
Script.NO_ACCESS_TEAM: integer = -1
```



[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4106-L4106" target="_blank">source</a>]








### Script.ALL_ACCESS_TEAM
---
```lua
Script.ALL_ACCESS_TEAM: integer = -2
```



[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L4108-L4108" target="_blank">source</a>]










{% endraw %}