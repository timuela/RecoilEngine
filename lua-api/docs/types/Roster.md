---
layout: default
title: Roster
parent: Lua API
permalink: lua-api/types/Roster
---

{% raw %}

# class Roster





Roster

Contains data about a player

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaUnsyncedRead.cpp#L4236-L4248" target="_blank">source</a>]







---



## fields
---

### Roster.name
---
```lua
Roster.name : string
```










### Roster.playerID
---
```lua
Roster.playerID : integer
```










### Roster.teamID
---
```lua
Roster.teamID : integer
```










### Roster.allyTeamID
---
```lua
Roster.allyTeamID : integer
```










### Roster.spectator
---
```lua
Roster.spectator : boolean
```










### Roster.cpuUsage
---
```lua
Roster.cpuUsage : number
```



in order to find the progress, use: cpuUsage&0x1 if it's PC or BO, cpuUsage& 0xFE to get path res, (cpuUsage>>8)*1000 for the progress








### Roster.pingTime
---
```lua
Roster.pingTime : number
```



if -1, the player is pathfinding










{% endraw %}