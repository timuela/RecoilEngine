---
layout: default
title: ExplosionParams
parent: Lua API
permalink: lua-api/types/ExplosionParams
---

{% raw %}

# class ExplosionParams





Parameters for explosion.

Please note the explosion defaults to 1 damage regardless of what it's defined in the weaponDef.
The weapondefID is only used for visuals and for passing into callins like UnitDamaged.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaSyncedCtrl.cpp#L7142-L7161" target="_blank">source</a>]







---



## fields
---

### ExplosionParams.weaponDef
---
```lua
ExplosionParams.weaponDef : number
```










### ExplosionParams.owner
---
```lua
ExplosionParams.owner : number
```










### ExplosionParams.hitUnit
---
```lua
ExplosionParams.hitUnit : number
```










### ExplosionParams.hitFeature
---
```lua
ExplosionParams.hitFeature : number
```










### ExplosionParams.craterAreaOfEffect
---
```lua
ExplosionParams.craterAreaOfEffect : number
```










### ExplosionParams.damageAreaOfEffect
---
```lua
ExplosionParams.damageAreaOfEffect : number
```










### ExplosionParams.edgeEffectiveness
---
```lua
ExplosionParams.edgeEffectiveness : number
```










### ExplosionParams.explosionSpeed
---
```lua
ExplosionParams.explosionSpeed : number
```










### ExplosionParams.gfxMod
---
```lua
ExplosionParams.gfxMod : number
```










### ExplosionParams.impactOnly
---
```lua
ExplosionParams.impactOnly : boolean
```










### ExplosionParams.ignoreOwner
---
```lua
ExplosionParams.ignoreOwner : boolean
```










### ExplosionParams.damageGround
---
```lua
ExplosionParams.damageGround : boolean
```












{% endraw %}