---
layout: default
title: Callins
parent: Lua API
permalink: lua-api/types/Callins
---

{% raw %}

# class Callins





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L82-L113" target="_blank">source</a>]

Functions called by the Engine.

To use these callins in a widget, prepend `widget:` and, for a gadget,
prepend `gadget:`. For example:

```lua
function widget:UnitCreated(unitID, unitDefID, unitTeam, builderID)
  -- ...
end
```

Some functions may differ between (synced) gadget and widgets. This is
because all information should be available to synced (game logic
controlling) gadgets, but restricted to unsynced gadget/widget. e.g.
information about an enemy unit only detected via radar and not yet in LOS.

In such cases the full (synced) param list is documented.

**Attention:** Some callins will only work on the unsynced portion of the gadget.
Due to the type-unsafe nature of lua parsing, those callins not firing up
might be hard to trace.







---

## methods
---

### Callins.Initialize
---
```lua
function Callins.Initialize()
```





Called when the addon is (re)loaded.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L521-L524" target="_blank">source</a>]








### Callins.LoadCode
---
```lua
function Callins.LoadCode()
```





Called when the game is (re)loaded.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L526-L529" target="_blank">source</a>]








### Callins.Shutdown
---
```lua
function Callins.Shutdown() ->  nil
```





Called when the addon or the game is shutdown.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L581-L585" target="_blank">source</a>]








### Callins.GotChatMsg
---
```lua
function Callins.GotChatMsg(
  msg: string,
  playerID: integer
)
```





Called when a player issues a UI command e.g. types /foo or /luarules foo.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L603-L608" target="_blank">source</a>]








### Callins.Load
---
```lua
function Callins.Load(zipReader: table)
```





Called after `GamePreload` and before `GameStart`. See Lua_SaveLoad.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L635-L639" target="_blank">source</a>]








### Callins.GamePreload
---
```lua
function Callins.GamePreload()
```





Called before the 0 gameframe.

Is not called when a saved game is loaded.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L702-L707" target="_blank">source</a>]








### Callins.GameStart
---
```lua
function Callins.GameStart()
```





Called upon the start of the game.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L725-L730" target="_blank">source</a>]

Is not called when a saved game is loaded.








### Callins.GameOver
---
```lua
function Callins.GameOver(winningAllyTeams: number[])
```
@param `winningAllyTeams` - list of winning allyTeams, if empty the game result was undecided (like when dropping from an host).






Called when the game ends

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L748-L752" target="_blank">source</a>]








### Callins.GamePaused
---
```lua
function Callins.GamePaused(
  playerID: integer,
  paused: boolean
)
```





Called when the game is paused.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L775-L780" target="_blank">source</a>]








### Callins.GameFrame
---
```lua
function Callins.GameFrame(frame: number)
```
@param `frame` - Starts at frame 1






Called for every game simulation frame (30 per second).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L825-L829" target="_blank">source</a>]








### Callins.GameFramePost
---
```lua
function Callins.GameFramePost(frame: number)
```
@param `frame` - Starts at frame 1






Called at the end of every game simulation frame

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L859-L863" target="_blank">source</a>]








### Callins.GameID
---
```lua
function Callins.GameID(gameID: string)
```
@param `gameID` - encoded in hex.






Called once to deliver the gameID

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L883-L887" target="_blank">source</a>]








### Callins.TeamDied
---
```lua
function Callins.TeamDied(teamID: integer)
```





Called when a team dies (see `Spring.KillTeam`).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L914-L918" target="_blank">source</a>]








### Callins.TeamChanged
---
```lua
function Callins.TeamChanged(teamID: integer)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L938-L941" target="_blank">source</a>]








### Callins.PlayerChanged
---
```lua
function Callins.PlayerChanged(playerID: integer)
```





Called whenever a player's status changes e.g. becoming a spectator.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L961-L965" target="_blank">source</a>]








### Callins.PlayerAdded
---
```lua
function Callins.PlayerAdded(playerID: integer)
```





Called whenever a new player joins the game.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L985-L989" target="_blank">source</a>]








### Callins.PlayerRemoved
---
```lua
function Callins.PlayerRemoved(
  playerID: integer,
  reason: string
)
```





Called whenever a player is removed from the game.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1009-L1014" target="_blank">source</a>]








### Callins.UnitCreated
---
```lua
function Callins.UnitCreated(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  builderID: integer?
)
```





Called at the moment the unit is created.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1059-L1066" target="_blank">source</a>]








### Callins.UnitFinished
---
```lua
function Callins.UnitFinished(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





Called at the moment the unit is completed.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1090-L1096" target="_blank">source</a>]








### Callins.UnitFromFactory
---
```lua
function Callins.UnitFromFactory(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  factID: integer,
  factDefID: integer,
  userOrders: boolean
)
```





Called when a factory finishes construction of a unit.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1104-L1113" target="_blank">source</a>]








### Callins.UnitReverseBuilt
---
```lua
function Callins.UnitReverseBuilt(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





Called when a living unit becomes a nanoframe again.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1138-L1144" target="_blank">source</a>]








### Callins.UnitConstructionDecayed
---
```lua
function Callins.UnitConstructionDecayed(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  timeSinceLastBuild: number,
  iterationPeriod: number,
  part: number
)
```





Called when a unit being built starts decaying.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1153-L1162" target="_blank">source</a>]








### Callins.UnitDestroyed
---
```lua
function Callins.UnitDestroyed(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  attackerID: integer,
  attackerDefID: integer,
  attackerTeam: number,
  weaponDefID: integer
)
```





Called when a unit is destroyed.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1186-L1196" target="_blank">source</a>]








### Callins.UnitTaken
---
```lua
function Callins.UnitTaken(
  unitID: integer,
  unitDefID: integer,
  oldTeam: number,
  newTeam: number
)
```





Called when a unit is transferred between teams. This is called before `UnitGiven` and in that moment unit is still assigned to the oldTeam.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1224-L1231" target="_blank">source</a>]








### Callins.UnitGiven
---
```lua
function Callins.UnitGiven(
  unitID: integer,
  unitDefID: integer,
  newTeam: number,
  oldTeam: number
)
```





Called when a unit is transferred between teams. This is called after `UnitTaken` and in that moment unit is assigned to the newTeam.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1253-L1260" target="_blank">source</a>]








### Callins.UnitIdle
---
```lua
function Callins.UnitIdle(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





Called when a unit is idle (empty command queue).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1282-L1288" target="_blank">source</a>]








### Callins.UnitCommand
---
```lua
function Callins.UnitCommand(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  cmdID: integer,
  cmdParams: table,
  options: CommandOptions,
  cmdTag: number
)
```





Called after when a unit accepts a command, after `AllowCommand` returns true.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1296-L1306" target="_blank">source</a>]








### Callins.UnitCmdDone
---
```lua
function Callins.UnitCmdDone(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  cmdID: integer,
  cmdParams: table,
  options: CommandOptions,
  cmdTag: number
)
```





Called when a unit completes a command.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1330-L1340" target="_blank">source</a>]








### Callins.UnitDamaged
---
```lua
function Callins.UnitDamaged(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  damage: number,
  paralyzer: number,
  weaponDefID: integer,
  projectileID: integer,
  attackerID: integer,
  attackerDefID: integer,
  attackerTeam: number
)
```





Called when a unit is damaged (after UnitPreDamaged).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1360-L1373" target="_blank">source</a>]








### Callins.UnitStunned
---
```lua
function Callins.UnitStunned(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  stunned: boolean
)
```





Called when a unit changes its stun status.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1408-L1415" target="_blank">source</a>]








### Callins.UnitExperience
---
```lua
function Callins.UnitExperience(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  experience: number,
  oldExperience: number
)
```





Called when a unit gains experience greater or equal to the minimum limit set by calling `Spring.SetExperienceGrade`.

Should be called more reliably with small values of experience grade.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1439-L1451" target="_blank">source</a>]








### Callins.UnitHarvestStorageFull
---
```lua
function Callins.UnitHarvestStorageFull(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





Called when a unit's harvestStorage is full (according to its unitDef's entry).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1475-L1481" target="_blank">source</a>]








### Callins.UnitSeismicPing
---
```lua
function Callins.UnitSeismicPing(
  x: number,
  y: number,
  z: number,
  strength: number,
  allyTeam: integer,
  unitID: integer,
  unitDefID: integer
)
```





Called when a unit emits a seismic ping.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1491-L1504" target="_blank">source</a>]

See `seismicSignature`.








### Callins.UnitEnteredRadar
---
```lua
function Callins.UnitEnteredRadar(
  unitID: integer,
  unitTeam: integer,
  allyTeam: integer,
  unitDefID: integer
)
```





Called when a unit enters radar of an allyteam.

Also called when a unit enters LOS without any radar coverage.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1558-L1568" target="_blank">source</a>]








### Callins.UnitEnteredLos
---
```lua
function Callins.UnitEnteredLos(
  unitID: integer,
  unitTeam: integer,
  allyTeam: integer,
  unitDefID: integer
)
```
@param `allyTeam` - who's LOS the unit entered.






Called when a unit enters LOS of an allyteam.

Its called after the unit is in LOS, so you can query that unit.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1577-L1587" target="_blank">source</a>]








### Callins.UnitLeftRadar
---
```lua
function Callins.UnitLeftRadar(
  unitID: integer,
  unitTeam: integer,
  allyTeam: integer,
  unitDefID: integer
)
```





Called when a unit leaves radar of an allyteam.

Also called when a unit leaves LOS without any radar coverage.
For widgets, this is called just after a unit leaves radar coverage, so
widgets cannot get the position of units that left their radar.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1595-L1607" target="_blank">source</a>]








### Callins.UnitLeftLos
---
```lua
function Callins.UnitLeftLos(
  unitID: integer,
  unitTeam: integer,
  allyTeam: integer,
  unitDefID: integer
)
```





Called when a unit leaves LOS of an allyteam.

For widgets, this one is called just before the unit leaves los, so you can still get the position of a unit that left los.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1616-L1626" target="_blank">source</a>]








### Callins.UnitLoaded
---
```lua
function Callins.UnitLoaded(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  transportID: integer,
  transportTeam: integer
)
```





Called when a unit is loaded by a transport.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1640-L1648" target="_blank">source</a>]








### Callins.UnitUnloaded
---
```lua
function Callins.UnitUnloaded(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  transportID: integer,
  transportTeam: integer
)
```





Called when a unit is unloaded by a transport.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1672-L1680" target="_blank">source</a>]








### Callins.UnitEnteredUnderwater
---
```lua
function Callins.UnitEnteredUnderwater(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1710-L1716" target="_blank">source</a>]








### Callins.UnitEnteredWater
---
```lua
function Callins.UnitEnteredWater(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1724-L1730" target="_blank">source</a>]








### Callins.UnitLeftAir
---
```lua
function Callins.UnitLeftAir(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1738-L1745" target="_blank">source</a>]








### Callins.UnitLeftUnderwater
---
```lua
function Callins.UnitLeftUnderwater(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1753-L1760" target="_blank">source</a>]








### Callins.UnitLeftWater
---
```lua
function Callins.UnitLeftWater(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1767-L1774" target="_blank">source</a>]








### Callins.UnitEnteredAir
---
```lua
function Callins.UnitEnteredAir(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1782-L1789" target="_blank">source</a>]








### Callins.UnitCloaked
---
```lua
function Callins.UnitCloaked(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





Called when a unit cloaks.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1797-L1804" target="_blank">source</a>]








### Callins.UnitDecloaked
---
```lua
function Callins.UnitDecloaked(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





Called when a unit decloaks.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1812-L1819" target="_blank">source</a>]








### Callins.UnitUnitCollision
---
```lua
function Callins.UnitUnitCollision(
  colliderID: integer,
  collideeID: integer
)
```





Called when two units collide.

Both units must be registered with `Script.SetWatchUnit`.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1827-L1834" target="_blank">source</a>]








### Callins.UnitFeatureCollision
---
```lua
function Callins.UnitFeatureCollision(
  colliderID: integer,
  collideeID: integer
)
```





Called when a unit collides with a feature.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1878-L1886" target="_blank">source</a>]

The unit must be registered with `Script.SetWatchUnit` and the feature registered with `Script.SetWatchFeature`.








### Callins.UnitMoveFailed
---
```lua
function Callins.UnitMoveFailed(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1931-L1938" target="_blank">source</a>]








### Callins.UnitArrivedAtGoal
---
```lua
function Callins.UnitArrivedAtGoal(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1953-L1960" target="_blank">source</a>]








### Callins.RenderUnitDestroyed
---
```lua
function Callins.RenderUnitDestroyed(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer
)
```





Called just before a unit is invalid, after it finishes its death animation.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L1970-L1977" target="_blank">source</a>]








### Callins.FeatureCreated
---
```lua
function Callins.FeatureCreated(
  featureID: integer,
  allyTeamID: integer
)
```





Called when a feature is created.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2008-L2014" target="_blank">source</a>]








### Callins.FeatureDestroyed
---
```lua
function Callins.FeatureDestroyed(
  featureID: integer,
  allyTeamID: integer
)
```





Called when a feature is destroyed.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2035-L2041" target="_blank">source</a>]








### Callins.FeatureDamaged
---
```lua
function Callins.FeatureDamaged(
  featureID: integer,
  featureDefID: integer,
  featureTeam: number,
  damage: number,
  weaponDefID: integer,
  projectileID: integer,
  attackerID: integer,
  attackerDefID: integer,
  attackerTeam: number
)
```





Called when a feature is damaged.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2062-L2075" target="_blank">source</a>]








### Callins.ProjectileCreated
---
```lua
function Callins.ProjectileCreated(
  proID: integer,
  proOwnerID: integer,
  weaponDefID: integer
)
```





Called when the projectile is created.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2117-L2129" target="_blank">source</a>]

Note that weaponDefID is missing if the projectile is spawned as part of a burst, but `Spring.GetProjectileDefID` and `Spring.GetProjectileName` still work in callin scope using proID.





@see Script.SetWatchWeapon




### Callins.ProjectileDestroyed
---
```lua
function Callins.ProjectileDestroyed(
  proID: integer,
  ownerID: integer,
  proWeaponDefID: integer
)
```





Called when the projectile is destroyed.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2169-L2178" target="_blank">source</a>]





@see Script.SetWatchWeapon




### Callins.Explosion
---
```lua
function Callins.Explosion(
  weaponDefID: integer,
  px: number,
  py: number,
  pz: number,
  attackerID: integer,
  projectileID: integer
) -> noGfx boolean
```

@return `noGfx` - if then no graphical effects are drawn by the engine for this explosion.





Called when an explosion occurs.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2235-L2252" target="_blank">source</a>]

Only called for weaponDefIDs registered via Script.SetWatchExplosion or Script.SetWatchWeapon.





@see Script.SetWatchWeapon




### Callins.StockpileChanged
---
```lua
function Callins.StockpileChanged(
  unitID: integer,
  unitDefID: integer,
  unitTeam: integer,
  weaponNum: integer,
  oldCount: integer,
  newCount: integer
)
```





Called when a units stockpile of weapons increases or decreases.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2301-L2311" target="_blank">source</a>]








### Callins.RecvLuaMsg
---
```lua
function Callins.RecvLuaMsg(
  msg: string,
  playerID: integer
)
```





Receives messages from unsynced sent via `Spring.SendLuaRulesMsg` or `Spring.SendLuaUIMsg`.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2336-L2341" target="_blank">source</a>]








### Callins.Save
---
```lua
function Callins.Save(zip: table)
```
@param `zip` - a userdatum representing the savegame zip file. See Lua_SaveLoad.






Called when a chat command '/save' or '/savegame' is received.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2472-L2476" target="_blank">source</a>]








### Callins.UnsyncedHeightMapUpdate
---
```lua
function Callins.UnsyncedHeightMapUpdate()
 -> x1 number
 -> z1 number
 -> x2 number
 -> z2 number

```





Called when the unsynced copy of the height-map is altered.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2498-L2505" target="_blank">source</a>]








### Callins.Update
---
```lua
function Callins.Update(dt: number)
```
@param `dt` - the time since the last update.






Called for every draw frame (including when the game is paused) and at least once per sim frame except when catching up.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2525-L2529" target="_blank">source</a>]








### Callins.ViewResize
---
```lua
function Callins.ViewResize(
  viewSizeX: number,
  viewSizeY: number
)
```





Called whenever the window is resized.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2544-L2549" target="_blank">source</a>]








### Callins.FontsChanged
---
```lua
function Callins.FontsChanged()
```





Called whenever fonts are updated. Signals the game display lists
and other caches should be discarded.

Gets called before other Update and Draw callins.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2583-L2589" target="_blank">source</a>]








### Callins.SunChanged
---
```lua
function Callins.SunChanged()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2604-L2606" target="_blank">source</a>]








### Callins.DefaultCommand
---
```lua
function Callins.DefaultCommand(
  type: ("unit"|"feature"),
  id: integer
)
```
@param `type` - The type of the object pointed at.

@param `id` - The `unitID` or `featureID`.






Used to set the default command when a unit is selected.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2620-L2625" target="_blank">source</a>]








### Callins.DrawGenesis
---
```lua
function Callins.DrawGenesis()
```





Use this callin to update textures, shaders, etc.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2712-L2718" target="_blank">source</a>]

Doesn't render to screen!
Also available to LuaMenu.








### Callins.DrawWorld
---
```lua
function Callins.DrawWorld()
```





Spring draws command queues, 'map stuff', and map marks.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2721-L2724" target="_blank">source</a>]








### Callins.DrawWorldPreUnit
---
```lua
function Callins.DrawWorldPreUnit()
```





Spring draws units, features, some water types, cloaked units, and the sun.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2727-L2730" target="_blank">source</a>]








### Callins.DrawPreDecals
---
```lua
function Callins.DrawPreDecals()
```





Called before decals are drawn

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2733-L2736" target="_blank">source</a>]








### Callins.DrawWaterPost
---
```lua
function Callins.DrawWaterPost()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2739-L2741" target="_blank">source</a>]








### Callins.DrawShadowPassTransparent
---
```lua
function Callins.DrawShadowPassTransparent()
```





Invoked after semi-transparent shadows pass is about to conclude

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2744-L2748" target="_blank">source</a>]

This callin has depth and color buffer of shadowmap bound via FBO as well as the FFP state to do "semi-transparent" shadows pass (traditionally only used to draw shadows of shadow casting semi-transparent particles). Can be used to draw nice colored shadows.








### Callins.DrawWorldShadow
---
```lua
function Callins.DrawWorldShadow()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2751-L2753" target="_blank">source</a>]








### Callins.DrawWorldReflection
---
```lua
function Callins.DrawWorldReflection()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2756-L2758" target="_blank">source</a>]








### Callins.DrawWorldRefraction
---
```lua
function Callins.DrawWorldRefraction()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2761-L2763" target="_blank">source</a>]








### Callins.DrawGroundPreForward
---
```lua
function Callins.DrawGroundPreForward()
```





Runs at the start of the forward pass when a custom map shader has been assigned via `Spring.SetMapShader` (convenient for setting uniforms).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2766-L2769" target="_blank">source</a>]








### Callins.DrawGroundPostForward
---
```lua
function Callins.DrawGroundPostForward()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2772-L2774" target="_blank">source</a>]








### Callins.DrawGroundPreDeferred
---
```lua
function Callins.DrawGroundPreDeferred()
```





Runs at the start of the deferred pass when a custom map shader has been assigned via `Spring.SetMapShader` (convenient for setting uniforms).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2777-L2780" target="_blank">source</a>]








### Callins.DrawGroundDeferred
---
```lua
function Callins.DrawGroundDeferred()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2783-L2785" target="_blank">source</a>]








### Callins.DrawGroundPostDeferred
---
```lua
function Callins.DrawGroundPostDeferred()
```





This runs at the end of its respective deferred pass.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2788-L2793" target="_blank">source</a>]

Allows proper frame compositing (with ground flashes/decals/foliage/etc, which are drawn between it and `DrawWorldPreUnit`) via `gl.CopyToTexture`.








### Callins.DrawUnitsPostDeferred
---
```lua
function Callins.DrawUnitsPostDeferred()
```





Runs at the end of the unit deferred pass.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2796-L2801" target="_blank">source</a>]

Informs Lua code it should make use of the $model_gbuffer_* textures before another pass overwrites them (and to allow proper blending with e.g. cloaked objects which are drawn between these events and DrawWorld via gl.CopyToTexture). N.B. The *PostDeferred events are only sent (and only have a real purpose) if forward drawing is disabled.








### Callins.DrawFeaturesPostDeferred
---
```lua
function Callins.DrawFeaturesPostDeferred()
```





Runs at the end of the feature deferred pass to inform Lua code it should make use of the $model_gbuffer_* textures before another pass overwrites them (and to allow proper blending with e.g. cloaked objects which are drawn between these events and DrawWorld via gl.CopyToTexture). N.B. The *PostDeferred events are only sent (and only have a real purpose) if forward drawing is disabled.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2804-L2807" target="_blank">source</a>]








### Callins.DrawShadowUnitsLua
---
```lua
function Callins.DrawShadowUnitsLua()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2810-L2812" target="_blank">source</a>]








### Callins.DrawShadowFeaturesLua
---
```lua
function Callins.DrawShadowFeaturesLua()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2815-L2817" target="_blank">source</a>]








### Callins.DrawWorldPreParticles
---
```lua
function Callins.DrawWorldPreParticles(
  drawAboveWater: boolean,
  drawBelowWater: boolean,
  drawReflection: boolean,
  drawRefraction: boolean
)
```





DrawWorldPreParticles is called multiples times per draw frame.
Each call has a different permutation of values for drawAboveWater, drawBelowWater, drawReflection, and drawRefraction.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2820-L2829" target="_blank">source</a>]








### Callins.DrawScreen
---
```lua
function Callins.DrawScreen(
  viewSizeX: number,
  viewSizeY: number
)
```





Also available to LuaMenu.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2868-L2873" target="_blank">source</a>]








### Callins.DrawScreenEffects
---
```lua
function Callins.DrawScreenEffects(
  viewSizeX: number,
  viewSizeY: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2885-L2889" target="_blank">source</a>]








### Callins.DrawScreenPost
---
```lua
function Callins.DrawScreenPost(
  viewSizeX: number,
  viewSizeY: number
)
```





Similar to DrawScreenEffects, this can be used to alter the contents of a frame after it has been completely rendered (i.e. World, MiniMap, Menu, UI).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2901-L2909" target="_blank">source</a>]

Note: This callin is invoked after the software rendered cursor (configuration variable HardwareCursor=0) is drawn.








### Callins.DrawInMiniMap
---
```lua
function Callins.DrawInMiniMap(
  sx: number,
  sy: number
)
```
@param `sx` - relative to the minimap's position and scale.

@param `sy` - relative to the minimap's position and scale.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2921-L2926" target="_blank">source</a>]








### Callins.DrawInMiniMapBackground
---
```lua
function Callins.DrawInMiniMapBackground(
  sx: number,
  sy: number
)
```
@param `sx` - relative to the minimap's position and scale.

@param `sy` - relative to the minimap's position and scale.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L2949-L2954" target="_blank">source</a>]








### Callins.GameProgress
---
```lua
function Callins.GameProgress(serverFrameNum: integer)
```





Called every 60 frames, calculating delta between `GameFrame` and `GameProgress`.

Can give an ETA about catching up with simulation for mid-game join players.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3024-L3030" target="_blank">source</a>]








### Callins.KeyMapChanged
---
```lua
function Callins.KeyMapChanged()
```





Called when the keymap changes

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3068-L3073" target="_blank">source</a>]

Can be caused due to a change in language or keyboard








### Callins.KeyPress
---
```lua
function Callins.KeyPress(
  keyCode: number,
  mods: KeyModifiers,
  isRepeat: boolean,
  label: boolean,
  utf32char: number,
  scanCode: number,
  actionList: table
) -> halt boolean
```
@param `isRepeat` - If you want an action to occur only once check for isRepeat == false.

@param `label` - the name of the key

@param `utf32char` - (deprecated) always 0

@param `actionList` - the list of actions for this keypress


@return `halt` - whether to halt the chain for consumers of the keypress





Called repeatedly when a key is pressed down.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3109-L3123" target="_blank">source</a>]

Return true if you don't want other callins or the engine to also receive this keypress. A list of key codes can be seen at the SDL wiki.








### Callins.KeyRelease
---
```lua
function Callins.KeyRelease(
  keyCode: number,
  mods: KeyModifiers,
  label: boolean,
  utf32char: number,
  scanCode: number,
  actionList: table
) ->  boolean
```
@param `label` - the name of the key

@param `utf32char` - (deprecated) always 0

@param `actionList` - the list of actions for this keyrelease






Called when the key is released.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3177-L3189" target="_blank">source</a>]








### Callins.TextInput
---
```lua
function Callins.TextInput(utf8char: string)
```





Called whenever a key press results in text input.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3238-L3243" target="_blank">source</a>]








### Callins.TextEditing
---
```lua
function Callins.TextEditing(
  utf8: string,
  start: number,
  length: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3266-L3273" target="_blank">source</a>]








### Callins.MousePress
---
```lua
function Callins.MousePress(
  x: number,
  y: number,
  button: number
) -> becomeMouseOwner boolean
```





Called when a mouse button is pressed.

The button parameter supports up to 7 buttons. Must return true for `MouseRelease` and other functions to be called.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3296-L3305" target="_blank">source</a>]








### Callins.MouseRelease
---
```lua
function Callins.MouseRelease(
  x: number,
  y: number,
  button: number
) -> becomeMouseOwner boolean
```





Called when a mouse button is released.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3329-L3339" target="_blank">source</a>]

Please note that in order to have Spring call `Spring.MouseRelease`, you need to have a `Spring.MousePress` call-in in the same addon that returns true.








### Callins.MouseMove
---
```lua
function Callins.MouseMove(
  x: number,
  y: number,
  dx: number,
  dy: number,
  button: number
)
```
@param `x` - final x position

@param `y` - final y position

@param `dx` - distance travelled in x

@param `dy` - distance travelled in y






Called when the mouse is moved.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3358-L3367" target="_blank">source</a>]








### Callins.MouseWheel
---
```lua
function Callins.MouseWheel(
  up: boolean,
  value: number
)
```
@param `up` - the direction

@param `value` - the amount travelled






Called when the mouse wheel is moved.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3393-L3399" target="_blank">source</a>]








### Callins.IsAbove
---
```lua
function Callins.IsAbove(
  x: number,
  y: number
) -> isAbove boolean
```





Called every `Update`.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3420-L3429" target="_blank">source</a>]

Must return true for `Mouse*` events and `Spring.GetToolTip` to be called.








### Callins.GetTooltip
---
```lua
function Callins.GetTooltip(
  x: number,
  y: number
) -> tooltip string
```





Called when `Spring.IsAbove` returns true.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3451-L3457" target="_blank">source</a>]








### Callins.ActiveCommandChanged
---
```lua
function Callins.ActiveCommandChanged(
  cmdId: integer?,
  cmdType: integer?
)
```





Called when a command is issued.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3479-L3484" target="_blank">source</a>]








### Callins.CameraRotationChanged
---
```lua
function Callins.CameraRotationChanged(
  rotX: number,
  rotY: number,
  rotZ: number
)
```
@param `rotX` - Camera rotation around the x axis in radians.

@param `rotY` - Camera rotation around the y axis in radians.

@param `rotZ` - Camera rotation around the z axis in radians.






Called whenever the camera rotation changes

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3505-L3511" target="_blank">source</a>]








### Callins.CameraPositionChanged
---
```lua
function Callins.CameraPositionChanged(
  posX: number,
  posY: number,
  posZ: number
)
```
@param `posX` - Camera position x in world coordinates

@param `posY` - Camera position y in world coordinates

@param `posZ` - Camera position z in world coordinates






Called whenever the camera position changes

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3528-L3534" target="_blank">source</a>]








### Callins.CommandNotify
---
```lua
function Callins.CommandNotify(
  cmdID: integer,
  cmdParams: table,
  options: CommandOptions
) -> Returning boolean
```

@return `Returning` - true deletes the command and does not send it through the network.





Called when a command is issued.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3552-L3559" target="_blank">source</a>]








### Callins.AddConsoleLine
---
```lua
function Callins.AddConsoleLine(
  msg: string,
  priority: integer
)
```





Called when text is entered into the console (e.g. `Spring.Echo`).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3588-L3593" target="_blank">source</a>]








### Callins.GroupChanged
---
```lua
function Callins.GroupChanged(groupID: integer)
```





Called when a unit is added to or removed from a control group.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3611-L3615" target="_blank">source</a>]








### Callins.WorldTooltip
---
```lua
function Callins.WorldTooltip(
  type: "unit",
  unitId: integer
) -> tooltip string
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3631-L3636" target="_blank">source</a>]








### Callins.WorldTooltip
---
```lua
function Callins.WorldTooltip(
  type: "feature",
  featureId: integer
) -> tooltip string
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3637-L3642" target="_blank">source</a>]








### Callins.WorldTooltip
---
```lua
function Callins.WorldTooltip(
  type: "ground",
  posX: number,
  posY: number,
  posZ: number
) -> tooltip string
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3643-L3650" target="_blank">source</a>]








### Callins.WorldTooltip
---
```lua
function Callins.WorldTooltip(type: "selection") -> tooltip string
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3651-L3655" target="_blank">source</a>]








### Callins.MapDrawCmd
---
```lua
function Callins.MapDrawCmd(
  playerID: integer,
  type: "point",
  posX: number,
  posY: number,
  posZ: number,
  label: string
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3699-L3707" target="_blank">source</a>]








### Callins.MapDrawCmd
---
```lua
function Callins.MapDrawCmd(
  playerID: integer,
  type: "line",
  pos1X: number,
  pos1Y: number,
  pos1Z: number,
  pos2X: number,
  pos2Y: number,
  pos2Z: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3708-L3718" target="_blank">source</a>]








### Callins.MapDrawCmd
---
```lua
function Callins.MapDrawCmd(
  playerID: integer,
  type: "erase",
  posX: number,
  posY: number,
  posZ: number,
  radius: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3719-L3727" target="_blank">source</a>]








### Callins.GameSetup
---
```lua
function Callins.GameSetup(
  state: string,
  ready: boolean,
  playerStates: table
)
 -> success boolean
 -> newReady boolean

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3787-L3795" target="_blank">source</a>]








### Callins.RecvSkirmishAIMessage
---
```lua
function Callins.RecvSkirmishAIMessage(
  aiTeam: integer,
  dataStr: string
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3838-L3842" target="_blank">source</a>]








### Callins.DownloadQueued
---
```lua
function Callins.DownloadQueued(
  id: integer,
  name: string,
  type: string
)
```





Called when a Pr-downloader download is queued

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3883-L3889" target="_blank">source</a>]








### Callins.DownloadStarted
---
```lua
function Callins.DownloadStarted(id: integer)
```





Called when a Pr-downloader download is started via VFS.DownloadArchive.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3911-L3915" target="_blank">source</a>]








### Callins.DownloadFinished
---
```lua
function Callins.DownloadFinished(id: integer)
```





Called when a Pr-downloader download finishes successfully.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3934-L3938" target="_blank">source</a>]








### Callins.DownloadFailed
---
```lua
function Callins.DownloadFailed(
  id: integer,
  errorID: integer
)
```





Called when a Pr-downloader download fails to complete.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3957-L3962" target="_blank">source</a>]








### Callins.DownloadProgress
---
```lua
function Callins.DownloadProgress(
  id: integer,
  downloaded: integer,
  total: integer
)
```





Called incrementally during a Pr-downloader download.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaHandle.cpp#L3982-L3988" target="_blank">source</a>]












{% endraw %}