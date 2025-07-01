---
layout: default
title: FeatureSupport
parent: Lua API
permalink: lua-api/types/FeatureSupport
---

{% raw %}

# class FeatureSupport





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaConstEngine.cpp#L17-L30" target="_blank">source</a>]







---



## fields
---

### FeatureSupport.NegativeGetUnitCurrentCommand
---
```lua
FeatureSupport.NegativeGetUnitCurrentCommand : boolean
```



Whether Spring.GetUnitCurrentCommand allows negative indices to look from the end








### FeatureSupport.hasExitOnlyYardmaps
---
```lua
FeatureSupport.hasExitOnlyYardmaps : boolean
```



Whether yardmaps accept 'e' (exit only) and 'u' (unbuildable, walkable)








### FeatureSupport.rmlUiApiVersion
---
```lua
FeatureSupport.rmlUiApiVersion : integer
```



Version of Recoil's rmlUI API








### FeatureSupport.noAutoShowMetal
---
```lua
FeatureSupport.noAutoShowMetal : boolean
```



Whether the engine switches to the metal view when selecting a "build metal extractor" command (yes if false)








### FeatureSupport.maxPiecesPerModel
---
```lua
FeatureSupport.maxPiecesPerModel : integer
```



How many pieces supported for 3d models?








### FeatureSupport.gunshipCruiseAltitudeMultiplier
---
```lua
FeatureSupport.gunshipCruiseAltitudeMultiplier : number
```



For gunships, the cruiseAltitude from the unit def is multiplied by this much








### FeatureSupport.noRefundForConstructionDecay
---
```lua
FeatureSupport.noRefundForConstructionDecay : boolean
```



Whether there is no refund for construction decay (100% metal back if false)








### FeatureSupport.noRefundForFactoryCancel
---
```lua
FeatureSupport.noRefundForFactoryCancel : boolean
```



Whether there is no refund for factory cancel (100% metal back if false)








### FeatureSupport.noOffsetForFeatureID
---
```lua
FeatureSupport.noOffsetForFeatureID : boolean
```



Whether featureID from various interfaces (targetID for Reclaim commands, ownerID from SpringGetGroundDecalOwner, etc) needs to be offset by `Game.maxUnits`








### FeatureSupport.noHandicapForReclaim
---
```lua
FeatureSupport.noHandicapForReclaim : boolean
```



Whether handicap is applied to income from reclaim








### FeatureSupport.groupAddDoesntSelect
---
```lua
FeatureSupport.groupAddDoesntSelect : boolean
```



Whether 'group add' also selects the group (does both if false)










{% endraw %}