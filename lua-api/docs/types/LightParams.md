---
layout: default
title: LightParams
parent: Lua API
permalink: lua-api/types/LightParams
---

{% raw %}

# class LightParams





Parameters for lighting

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaUnsyncedCtrl.cpp#L1590-L1616" target="_blank">source</a>]







---



## fields
---

### LightParams.position
---
```lua
LightParams.position : { px: number, py: number, pz: number }
```










### LightParams.direction
---
```lua
LightParams.direction : { dx: number, dy: number, dz: number }
```










### LightParams.ambientColor
---
```lua
LightParams.ambientColor : { blue: number, green: number, red: number }
```










### LightParams.diffuseColor
---
```lua
LightParams.diffuseColor : { blue: number, green: number, red: number }
```










### LightParams.specularColor
---
```lua
LightParams.specularColor : { blue: number, green: number, red: number }
```










### LightParams.intensityWeight
---
```lua
LightParams.intensityWeight : { ambientWeight: number, diffuseWeight: number, specularWeight: number }
```










### LightParams.ambientDecayRate
---
```lua
LightParams.ambientDecayRate : { ambientBlueDecay: number, ambientGreenDecay: number, ambientRedDecay: number }
```



Per-frame decay of `ambientColor` (spread over TTL frames)








### LightParams.diffuseDecayRate
---
```lua
LightParams.diffuseDecayRate : { diffuseBlueDecay: number, diffuseGreenDecay: number, diffuseRedDecay: number }
```



Per-frame decay of `diffuseColor` (spread over TTL frames)








### LightParams.specularDecayRate
---
```lua
LightParams.specularDecayRate : { specularBlueDecay: number, specularGreenDecay: number, specularRedDecay: number }
```



Per-frame decay of `specularColor` (spread over TTL frames)








### LightParams.decayFunctionType
---
```lua
LightParams.decayFunctionType : { ambientDecayType: number, diffuseDecayType: number, specularDecayType: number }
```



If value is `0.0` then the `*DecayRate` values will be interpreted as linear, otherwise exponential.








### LightParams.radius
---
```lua
LightParams.radius : number
```










### LightParams.fov
---
```lua
LightParams.fov : number
```










### LightParams.ttl
---
```lua
LightParams.ttl : number
```










### LightParams.priority
---
```lua
LightParams.priority : number
```










### LightParams.ignoreLOS
---
```lua
LightParams.ignoreLOS : boolean
```












{% endraw %}