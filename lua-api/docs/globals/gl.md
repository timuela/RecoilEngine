---
layout: default
title: gl
parent: Lua API
permalink: lua-api/globals/gl
---

{% raw %}

# global gl


Callouts for OpenGL API

Only setters and getters for OpenGL usage in Recoil, see `GL` for constants.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L86-L92" target="_blank">source</a>]





@see GL



---

## methods
---

### gl.HasExtension
---
```lua
function gl.HasExtension(ext: string) ->  boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1148-L1152" target="_blank">source</a>]








### gl.GetNumber
---
```lua
function gl.GetNumber(
  pname: GL,
  count: integer?
) ->  number ...
```
@param `count` - (Default: `1`) Number of values to return, in range [1, 64].






Get the value or values of a selected parameter.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1160-L1167" target="_blank">source</a>]








### gl.GetString
---
```lua
function gl.GetString(pname: GL)
```





Get a string describing the current OpenGL connection.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1185-L1189" target="_blank">source</a>]








### gl.GetScreenViewTrans
---
```lua
function gl.GetScreenViewTrans()
 -> x number
 -> y number
 -> z number

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1205-L1210" target="_blank">source</a>]








### gl.GetViewSizes
---
```lua
function gl.GetViewSizes()
 -> x number
 -> y number

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1220-L1224" target="_blank">source</a>]








### gl.GetViewRange
---
```lua
function gl.GetViewRange()
 -> nearPlaneDist number
 -> farPlaneDist number
 -> minViewRange number
 -> maxViewRange number

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1233-L1239" target="_blank">source</a>]








### gl.SlaveMiniMap
---
```lua
function gl.SlaveMiniMap(newMode: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1255-L1258" target="_blank">source</a>]








### gl.ConfigMiniMap
---
```lua
function gl.ConfigMiniMap(
  px: integer,
  py: integer,
  sx: integer,
  sy: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1270-L1276" target="_blank">source</a>]








### gl.DrawMiniMap
---
```lua
function gl.DrawMiniMap(defaultTransform: boolean?)
```
@param `defaultTransform` - (Default: `true`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1292-L1295" target="_blank">source</a>]








### gl.BeginText
---
```lua
function gl.BeginText(userDefinedBlending: boolean?)
```
@param `userDefinedBlending` - When `true` doesn't set the gl.BlendFunc automatically. Defaults to `false`.






Begin a block of text commands.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1332-L1346" target="_blank">source</a>]

Text can be drawn without Start/End, but when doing several operations it's more optimal
if done inside a block.

Also allows disabling automatic setting of the blend mode. Otherwise the font will always print
with `BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)`.





@see gl.BlendFuncSeparate




### gl.EndText
---
```lua
function gl.EndText()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1356-L1358" target="_blank">source</a>]








### gl.Text
---
```lua
function gl.Text(
  text: string,
  x: number,
  y: number,
  size: number,
  options: string?
) ->  nil
```
@param `options` - concatenated string of option characters.

- horizontal alignment:
- 'c' = center
- 'r' = right
- vertical alignment:
- 'a' = ascender
- 't' = top
- 'v' = vertical center
- 'x' = baseline
- 'b' = bottom
- 'd' = descender
- decorations:
- 'o' = black outline
- 'O' = white outline
- 's' = shadow
- other:
- 'n' = don't round vertex coords to nearest integer (font may get blurry)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1366-L1391" target="_blank">source</a>]








### gl.GetTextWidth
---
```lua
function gl.GetTextWidth(text: string) -> width number
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1447-L1451" target="_blank">source</a>]








### gl.GetTextHeight
---
```lua
function gl.GetTextHeight(text: string)
 -> height number
 -> descender number
 -> lines integer

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1462-L1468" target="_blank">source</a>]








### gl.Unit
---
```lua
function gl.Unit(
  unitID: integer,
  doRawDraw: boolean?,
  useLuaMat: integer?,
  noLuaCall: boolean?,
  fullModel: boolean?
)
```
@param `doRawDraw` - (Default: `false`)

@param `noLuaCall` - (Default: `false`) Skip the `DrawUnit` callin.

@param `fullModel` - (Default: `true`)






Draw the unit, applying transform.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1663-L1672" target="_blank">source</a>]








### gl.UnitRaw
---
```lua
function gl.UnitRaw(
  unitID: integer,
  doRawDraw: boolean?,
  useLuaMat: integer?,
  noLuaCall: boolean?,
  fullModel: boolean?
)
```
@param `doRawDraw` - (Default: `false`)

@param `noLuaCall` - (Default: `true`) Skip the `DrawUnit` callin.

@param `fullModel` - (Default: `true`)






Draw the unit without applying transform.

Also skips the `DrawUnit` callin by default so any
recursion is blocked.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1675-L1687" target="_blank">source</a>]








### gl.UnitTextures
---
```lua
function gl.UnitTextures(
  unitID: integer,
  push: boolean
)
```
@param `push` - If `true`, push the render state; if `false`, pop it.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1690-L1694" target="_blank">source</a>]








### gl.UnitShape
---
```lua
function gl.UnitShape(
  unitDefID: integer,
  teamID: integer,
  rawState: boolean?,
  toScreen: boolean?,
  opaque: boolean?
)
```
@param `rawState` - (Default: `true`)

@param `toScreen` - (Default: `false`)

@param `opaque` - (Default: `true`) If `true`, draw opaque; if `false`, draw alpha.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1702-L1709" target="_blank">source</a>]








### gl.UnitShapeTextures
---
```lua
function gl.UnitShapeTextures(
  unitDefID: integer,
  push: boolean
)
```
@param `push` - If `true`, push the render state; if `false`, pop it.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1717-L1721" target="_blank">source</a>]








### gl.UnitMultMatrix
---
```lua
function gl.UnitMultMatrix(unitID: integer)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1730-L1733" target="_blank">source</a>]








### gl.UnitPiece
---
```lua
function gl.UnitPiece(
  unitID: integer,
  pieceID: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1748-L1752" target="_blank">source</a>]








### gl.UnitPieceMatrix
---
```lua
function gl.UnitPieceMatrix(
  unitID: integer,
  pieceID: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1759-L1763" target="_blank">source</a>]








### gl.UnitPieceMultMatrix
---
```lua
function gl.UnitPieceMultMatrix(
  unitID: integer,
  pieceID: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1766-L1770" target="_blank">source</a>]








### gl.Feature
---
```lua
function gl.Feature(
  featureID: integer,
  doRawDraw: boolean?,
  useLuaMat: integer?,
  noLuaCall: boolean?
)
```
@param `doRawDraw` - (Default: `false`)

@param `noLuaCall` - (Default: `false`) Skip the `DrawFeature` callin.






Draw the feature, applying transform.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1831-L1839" target="_blank">source</a>]








### gl.FeatureRaw
---
```lua
function gl.FeatureRaw(
  featureID: integer,
  doRawDraw: boolean?,
  useLuaMat: integer?,
  noLuaCall: boolean?
)
```
@param `doRawDraw` - (Default: `false`)

@param `noLuaCall` - (Default: `true`) Skip the `DrawFeature` callin.






Draw the unit without applying transform.

Also skips the `DrawFeature` callin by default so any
recursion is blocked.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1842-L1852" target="_blank">source</a>]








### gl.FeatureTextures
---
```lua
function gl.FeatureTextures(
  featureID: integer,
  push: boolean
)
```
@param `push` - If `true`, push the render state; if `false`, pop it.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1855-L1859" target="_blank">source</a>]








### gl.FeatureShape
---
```lua
function gl.FeatureShape(
  featureDefID: integer,
  teamID: integer,
  rawState: boolean?,
  toScreen: boolean?,
  opaque: boolean?
)
```
@param `rawState` - (Default: `true`)

@param `toScreen` - (Default: `false`)

@param `opaque` - (Default: `true`) If `true`, draw opaque; if `false`, draw alpha.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1867-L1874" target="_blank">source</a>]








### gl.FeatureShapeTextures
---
```lua
function gl.FeatureShapeTextures(
  featureDefID: integer,
  push: boolean
)
```
@param `push` - If `true`, push the render state; if `false`, pop it.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1882-L1886" target="_blank">source</a>]








### gl.FeatureMultMatrix
---
```lua
function gl.FeatureMultMatrix(featureID: integer)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1895-L1898" target="_blank">source</a>]








### gl.FeaturePiece
---
```lua
function gl.FeaturePiece(
  featureID: integer,
  pieceID: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1913-L1917" target="_blank">source</a>]








### gl.FeaturePieceMatrix
---
```lua
function gl.FeaturePieceMatrix(
  featureID: integer,
  pieceID: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1925-L1929" target="_blank">source</a>]








### gl.FeaturePieceMultMatrix
---
```lua
function gl.FeaturePieceMultMatrix(
  featureID: integer,
  pieceID: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1933-L1937" target="_blank">source</a>]








### gl.DrawListAtUnit
---
```lua
function gl.DrawListAtUnit(
  unitID: integer,
  listIndex: integer,
  useMidPos: boolean?,
  scaleX: number?,
  scaleY: number?,
  scaleZ: number?,
  degrees: number?,
  rotX: number?,
  rotY: number?,
  rotZ: number?
)
```
@param `useMidPos` - (Default: `true`)

@param `scaleX` - (Default: `1.0`)

@param `scaleY` - (Default: `1.0`)

@param `scaleZ` - (Default: `1.0`)

@param `degrees` - (Default: `0.0`)

@param `rotX` - (Default: `0.0`)

@param `rotY` - (Default: `1.0`)

@param `rotZ` - (Default: `0.0`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L1950-L1962" target="_blank">source</a>]








### gl.DrawFuncAtUnit
---
```lua
function gl.DrawFuncAtUnit(
  unitID: integer,
  useMidPos: boolean?,
  fun: unknown,
  ...: any
)
```
@param `useMidPos` - (Default: `true`)

@param `...` - Arguments passed to function.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2007-L2013" target="_blank">source</a>]








### gl.DrawGroundCircle
---
```lua
function gl.DrawGroundCircle(
  posX: number,
  posY: number,
  posZ: number,
  radius: number,
  resolution: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2050-L2057" target="_blank">source</a>]








### gl.DrawGroundCircle
---
```lua
function gl.DrawGroundCircle(
  posX: number,
  posY: number,
  posZ: number,
  radius: number,
  resolution: integer,
  slope: number,
  gravity: number,
  weaponDefID: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2058-L2068" target="_blank">source</a>]








### gl.DrawGroundCircle
---
```lua
function gl.DrawGroundCircle(
  x0: number,
  z0: number,
  x1: number,
  z1: number,
  useNorm: nil,
  useTxcd: boolean?
)
```
@param `useNorm` - No longer used.

@param `useTxcd` - (Default: `false`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2109-L2117" target="_blank">source</a>]








### gl.DrawGroundCircle
---
```lua
function gl.DrawGroundCircle(
  x0: number,
  z0: number,
  x1: number,
  z1: number,
  useNorm: nil,
  tu0: number,
  tv0: number,
  tu1: number,
  tv1: number
)
```
@param `useNorm` - No longer used.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2118-L2129" target="_blank">source</a>]








### gl.Shape
---
```lua
function gl.Shape(
  type: GL,
  vertices: VertexData[]
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2318-L2322" target="_blank">source</a>]








### gl.BeginEnd
---
```lua
function gl.BeginEnd(
  primMode: GL,
  fun: unknown,
  ...: any
)
```
@param `...` - Arguments passed to function.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2364-L2369" target="_blank">source</a>]








### gl.Vertex
---
```lua
function gl.Vertex(v: xy)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2401-L2404" target="_blank">source</a>]








### gl.Vertex
---
```lua
function gl.Vertex(v: xyz)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2405-L2408" target="_blank">source</a>]








### gl.Vertex
---
```lua
function gl.Vertex(v: xyzw)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2409-L2412" target="_blank">source</a>]








### gl.Vertex
---
```lua
function gl.Vertex(
  x: number,
  y: number,
  z: number?,
  w: number?
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2413-L2419" target="_blank">source</a>]








### gl.Normal
---
```lua
function gl.Normal(v: xyz)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2483-L2486" target="_blank">source</a>]








### gl.Normal
---
```lua
function gl.Normal(
  x: number,
  y: number,
  z: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2487-L2492" target="_blank">source</a>]








### gl.TexCoord
---
```lua
function gl.TexCoord(coord: (number))
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2531-L2534" target="_blank">source</a>]








### gl.TexCoord
---
```lua
function gl.TexCoord(coord: xy)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2535-L2538" target="_blank">source</a>]








### gl.TexCoord
---
```lua
function gl.TexCoord(coord: xyz)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2539-L2542" target="_blank">source</a>]








### gl.TexCoord
---
```lua
function gl.TexCoord(coord: xyzw)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2543-L2546" target="_blank">source</a>]








### gl.TexCoord
---
```lua
function gl.TexCoord(
  s: number,
  t: number?,
  r: number?,
  q: number?
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2547-L2553" target="_blank">source</a>]








### gl.MultiTexCoord
---
```lua
function gl.MultiTexCoord(
  texNum: integer,
  coord: (number)
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2622-L2626" target="_blank">source</a>]








### gl.MultiTexCoord
---
```lua
function gl.MultiTexCoord(
  texNum: integer,
  coord: xy
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2627-L2631" target="_blank">source</a>]








### gl.MultiTexCoord
---
```lua
function gl.MultiTexCoord(
  texNum: integer,
  coord: xyz
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2632-L2636" target="_blank">source</a>]








### gl.MultiTexCoord
---
```lua
function gl.MultiTexCoord(
  texNum: integer,
  coord: xyzw
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2637-L2641" target="_blank">source</a>]








### gl.MultiTexCoord
---
```lua
function gl.MultiTexCoord(
  texNum: integer,
  s: number,
  t: number?,
  r: number?,
  q: number?
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2642-L2649" target="_blank">source</a>]








### gl.SecondaryColor
---
```lua
function gl.SecondaryColor(color: rgb)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2724-L2727" target="_blank">source</a>]








### gl.SecondaryColor
---
```lua
function gl.SecondaryColor(
  r: number,
  g: number,
  b: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2728-L2733" target="_blank">source</a>]








### gl.FogCoord
---
```lua
function gl.FogCoord(coord: number)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2772-L2775" target="_blank">source</a>]








### gl.EdgeFlag
---
```lua
function gl.EdgeFlag(flag: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2787-L2790" target="_blank">source</a>]








### gl.Rect
---
```lua
function gl.Rect(
  x1: number,
  y1: number,
  x2: number,
  y2: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2805-L2811" target="_blank">source</a>]








### gl.Rect
---
```lua
function gl.Rect(
  x1: number,
  y1: number,
  x2: number,
  y2: number,
  flipSCoords: boolean?,
  flipTCoords: boolean?
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2824-L2832" target="_blank">source</a>]








### gl.Rect
---
```lua
function gl.Rect(
  x1: number,
  y1: number,
  x2: number,
  y2: number,
  s1: number,
  t1: number,
  s2: number,
  t2: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2833-L2843" target="_blank">source</a>]








### gl.DispatchCompute
---
```lua
function gl.DispatchCompute(
  numGroupX: integer,
  numGroupY: integer,
  numGroupZ: integer,
  barriers: integer?
)
```
@param `barriers` - (Default: `4`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2899-L2905" target="_blank">source</a>]








### gl.MemoryBarrier
---
```lua
function gl.MemoryBarrier(barriers: integer?)
```
@param `barriers` - (Default: `4`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2938-L2941" target="_blank">source</a>]








### gl.Color
---
```lua
function gl.Color(
  r: number,
  g: number,
  b: number,
  a: number?
)
```
@param `r` - Red.

@param `g` - Green.

@param `b` - Blue.

@param `a` - (Default: `1.0`) Alpha.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2959-L2965" target="_blank">source</a>]








### gl.Color
---
```lua
function gl.Color(color: rgba)
```
@param `color` - Color with alpha.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2966-L2969" target="_blank">source</a>]








### gl.Color
---
```lua
function gl.Color(color: rgb)
```
@param `color` - Color.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L2970-L2973" target="_blank">source</a>]








### gl.Material
---
```lua
function gl.Material(material: Material)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3021-L3024" target="_blank">source</a>]








### gl.ResetState
---
```lua
function gl.ResetState()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3094-L3096" target="_blank">source</a>]








### gl.ResetMatrices
---
```lua
function gl.ResetMatrices()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3105-L3107" target="_blank">source</a>]








### gl.Lighting
---
```lua
function gl.Lighting(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3123-L3126" target="_blank">source</a>]








### gl.ShadeModel
---
```lua
function gl.ShadeModel(model: GL)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3140-L3143" target="_blank">source</a>]








### gl.Scissor
---
```lua
function gl.Scissor(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3152-L3155" target="_blank">source</a>]








### gl.Scissor
---
```lua
function gl.Scissor(
  x: integer,
  y: integer,
  w: integer,
  h: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3156-L3162" target="_blank">source</a>]








### gl.Viewport
---
```lua
function gl.Viewport(
  x: integer,
  y: integer,
  w: integer,
  h: integer
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3193-L3199" target="_blank">source</a>]








### gl.ColorMask
---
```lua
function gl.ColorMask(rgba: boolean)
```





Enable or disable writing of frame buffer color components.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3216-L3220" target="_blank">source</a>]








### gl.ColorMask
---
```lua
function gl.ColorMask(
  red: boolean,
  green: boolean,
  blue: boolean,
  alpha: boolean
)
```





Enable or disable writing of frame buffer color components.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3221-L3228" target="_blank">source</a>]








### gl.DepthMask
---
```lua
function gl.DepthMask(enable: boolean)
```





Enable or disable writing into the depth buffer.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3252-L3256" target="_blank">source</a>]








### gl.DepthTest
---
```lua
function gl.DepthTest(enable: boolean)
```





Enable or disable depth test.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3269-L3273" target="_blank">source</a>]








### gl.DepthTest
---
```lua
function gl.DepthTest(depthFunction: GL)
```
@param `depthFunction` - Symbolic constants `GL.NEVER`, `GL.LESS`, `GL.EQUAL`, `GL.LEQUAL`,
`GL.GREATER`, `GL.NOTEQUAL`, `GL.GEQUAL`, and `GL.ALWAYS` are accepted.
The initial value is `GL.LESS`.






Enable depth test and specify the depth comparison function.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3274-L3283" target="_blank">source</a>]








### gl.DepthClamp
---
```lua
function gl.DepthClamp(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3311-L3314" target="_blank">source</a>]








### gl.Culling
---
```lua
function gl.Culling(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3328-L3331" target="_blank">source</a>]








### gl.Culling
---
```lua
function gl.Culling(mode: GL)
```
@param `mode` - Specifies whether front- or back-facing facets are candidates for culling.
Symbolic constants `GL.FRONT`, `GL.BACK`, and `GL.FRONT_AND_BACK` are accepted. The
initial value is `GL.BACK`.






Enable culling and set culling mode.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3332-L3340" target="_blank">source</a>]








### gl.LogicOp
---
```lua
function gl.LogicOp(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3369-L3372" target="_blank">source</a>]








### gl.LogicOp
---
```lua
function gl.LogicOp(opCode: GL)
```
@param `opCode` - Specifies a symbolic constant that selects a logical operation. The following
symbols are accepted: `GL.CLEAR`, `GL.SET`, `GL.COPY`, `GL.COPY_INVERTED`,
`GL.NOOP`, `GL.INVERT`, `GL.AND`, `GL.NAND`, `GL.OR`, `GL.NOR`, `GL.XOR`,
`GL.EQUIV`, `GL.AND_REVERSE`, `GL.AND_INVERTED`, `GL.OR_REVERSE`, and
`GL.OR_INVERTED`.  The initial value is `GL.COPY`.






Specify a logical pixel operation for rendering.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3373-L3385" target="_blank">source</a>]








### gl.Fog
---
```lua
function gl.Fog(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3413-L3416" target="_blank">source</a>]








### gl.Blending
---
```lua
function gl.Blending(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3431-L3434" target="_blank">source</a>]








### gl.Blending
---
```lua
function gl.Blending(mode: ("add"|"alpha_add"|"alpha"|"reset"|"color"|"modulate"...))
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3435-L3438" target="_blank">source</a>]








### gl.Blending
---
```lua
function gl.Blending(
  src: GL,
  dst: GL
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3439-L3443" target="_blank">source</a>]








### gl.BlendEquation
---
```lua
function gl.BlendEquation(mode: GL)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3506-L3509" target="_blank">source</a>]








### gl.BlendFunc
---
```lua
function gl.BlendFunc(
  src: GL,
  dst: GL
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3519-L3523" target="_blank">source</a>]








### gl.BlendEquationSeparate
---
```lua
function gl.BlendEquationSeparate(
  modeRGB: GL,
  modeAlpha: GL
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3534-L3538" target="_blank">source</a>]








### gl.BlendFuncSeparate
---
```lua
function gl.BlendFuncSeparate(
  srcRGB: GL,
  dstRGB: GL,
  srcAlpha: GL,
  dstAlpha: GL
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3549-L3555" target="_blank">source</a>]








### gl.AlphaTest
---
```lua
function gl.AlphaTest(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3568-L3571" target="_blank">source</a>]








### gl.AlphaTest
---
```lua
function gl.AlphaTest(
  func: GL,
  ref: number
)
```
@param `func` - Specifies the alpha comparison function. Symbolic constants `GL.NEVER`,
`GL.LESS`, `GL.EQUAL`, `GL.LEQUAL`, `GL.GREATER`, `GL.NOTEQUAL`, `GL.GEQUAL`,
and `GL.ALWAYS` are accepted. The initial value is `GL.ALWAYS`.

@param `ref` - Specifies the reference value that incoming alpha values are compared to.
This value is clamped to the range `[0, 1]`, where `0` represents the lowest
possible alpha value and `1` the highest possible value. The initial reference
value is `0`.






Specify the alpha test function.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3572-L3585" target="_blank">source</a>]








### gl.AlphaToCoverage
---
```lua
function gl.AlphaToCoverage(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3609-L3612" target="_blank">source</a>]








### gl.PolygonMode
---
```lua
function gl.PolygonMode(
  face: GL,
  mode: GL
)
```
@param `face` - Specifies the polygons that mode applies to. Must be `GL.FRONT` for
front-facing polygons, `GL.BACK` for back-facing polygons, or `GL.FRONT_AND_BACK`
for front- and back-facing polygons.

@param `mode` - Specifies how polygons will be rasterized. Accepted values are `GL.POINT`,
`GL.LINE`, and `GL.FILL`. The initial value is `GL.FILL` for both front- and
back-facing polygons.






Select polygon rasterization mode.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3630-L3646" target="_blank">source</a>]








### gl.PolygonOffset
---
```lua
function gl.PolygonOffset(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3657-L3660" target="_blank">source</a>]








### gl.PolygonOffset
---
```lua
function gl.PolygonOffset(
  factor: number,
  units: number
)
```
@param `factor` - Specifies a scale factor that is used to create a variable depth offset for
each polygon. The initial value is `0`.

@param `units` - Is multiplied by an implementation-specific value to create a constant depth
offset. The initial value is `0`.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3661-L3672" target="_blank">source</a>]








### gl.StencilTest
---
```lua
function gl.StencilTest(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3704-L3707" target="_blank">source</a>]








### gl.StencilMask
---
```lua
function gl.StencilMask(mask: integer)
```
@param `mask` - Specifies a bit mask to enable and disable writing of individual bits in the stencil planes. Initially, the mask is all `1`'s.






Control the front and back writing of individual bits in the stencil planes.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3721-L3725" target="_blank">source</a>]








### gl.StencilFunc
---
```lua
function gl.StencilFunc(
  func: GL,
  ref: integer,
  mask: integer
)
```
@param `func` - Specifies the test function. Eight symbolic constants are valid: `GL.NEVER`, `GL.LESS`, `GL.EQUAL`, `GL.LEQUAL`, `GL.GREATER`, `GL.NOTEQUAL`, `GL.GEQUAL`, and `GL.ALWAYS`. The initial value is `GL.ALWAYS`.

@param `ref` - Specifies the reference value for the stencil test. `ref` is clamped to the range `[0, 2^n - 1]`, where `n` is the number of bitplanes in the stencil buffer. The initial value is `0`.

@param `mask` - Specifies a mask that is ANDed with both the reference value and the stored stencil value when the test is done. The initial value is all `1`'s.






Set front and back function and reference value for stencil testing.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3735-L3741" target="_blank">source</a>]








### gl.StencilOp
---
```lua
function gl.StencilOp(
  fail: GL,
  zfail: GL,
  zpass: GL
)
```
@param `fail` - Specifies the action to take when the stencil test fails. Eight symbolic constants are valid: `GL.KEEP`, `GL.ZERO`, `GL.REPLACE`, `GL.INCR`, `GL.INCR_WRAP`, `GL.DECR`, `GL.DECR_WRAP`, and `GL.INVERT`. The initial value is `GL.KEEP`.

@param `zfail` - Specifies the stencil action when the stencil test passes, but the depth test fails. The initial value is `GL.KEEP`.

@param `zpass` - Specifies the stencil action when both the stencil test and the depth test pass, or when the stencil test passes and either there is no depth buffer or depth testing is not enabled. The initial value is `GL.KEEP`.






Set front and back stencil test actions.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3753-L3759" target="_blank">source</a>]








### gl.StencilMaskSeparate
---
```lua
function gl.StencilMaskSeparate(
  face: GL,
  mask: integer
)
```
@param `face` - Specifies whether the front and/or back stencil writemask is updated. Three symbolic constants are accepted: `GL.FRONT`, `GL.BACK`, and `GL.FRONT_AND_BACK`. The initial value is `GL.FRONT_AND_BACK`.

@param `mask` - Specifies a bit mask to enable and disable writing of individual bits in the stencil planes. Initially, the mask is all `1`'s.






Control the front and back writing of individual bits in the stencil planes.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3771-L3776" target="_blank">source</a>]








### gl.StencilFuncSeparate
---
```lua
function gl.StencilFuncSeparate(
  face: GL,
  func: GL,
  ref: integer,
  mask: integer
)
```
@param `face` - Specifies whether front and/or back stencil state is updated. Three symbolic constants are accepted: `GL.FRONT`, `GL.BACK`, and `GL.FRONT_AND_BACK`. The initial value is `GL.FRONT_AND_BACK`.

@param `func` - Specifies the test function. Eight symbolic constants are valid: `GL.NEVER`, `GL.LESS`, `GL.EQUAL`, `GL.LEQUAL`, `GL.GREATER`, `GL.NOTEQUAL`, `GL.GEQUAL`, and `GL.ALWAYS`. The initial value is `GL.ALWAYS`.

@param `ref` - Specifies the reference value for the stencil test. `ref` is clamped to the range `[0, 2^n - 1]`, where `n` is the number of bitplanes in the stencil buffer. The initial value is `0`.

@param `mask` - Specifies a mask that is ANDed with both the reference value and the stored stencil value when the test is done. The initial value is all `1`'s.






Set front and/or back function and reference value for stencil testing.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3787-L3794" target="_blank">source</a>]








### gl.StencilOpSeparate
---
```lua
function gl.StencilOpSeparate(
  face: GL,
  fail: GL,
  zfail: GL,
  zpass: GL
)
```
@param `face` - Specifies whether front and/or back stencil state is updated. Three symbolic constants are accepted: `GL.FRONT`, `GL.BACK`, and `GL.FRONT_AND_BACK`. The initial value is `GL.FRONT_AND_BACK`.

@param `fail` - Specifies the action to take when the stencil test fails. Eight symbolic constants are valid: `GL.KEEP`, `GL.ZERO`, `GL.REPLACE`, `GL.INCR`, `GL.INCR_WRAP`, `GL.DECR`, `GL.DECR_WRAP`, and `GL.INVERT`. The initial value is `GL.KEEP`.

@param `zfail` - Specifies the stencil action when the stencil test passes, but the depth test fails. The initial value is `GL.KEEP`.

@param `zpass` - Specifies the stencil action when both the stencil test and the depth test pass, or when the stencil test passes and either there is no depth buffer or depth testing is not enabled. The initial value is `GL.KEEP`.






Set front and/or back stencil test actions.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3807-L3814" target="_blank">source</a>]








### gl.LineStipple
---
```lua
function gl.LineStipple(enable: boolean)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3829-L3832" target="_blank">source</a>]








### gl.LineStipple
---
```lua
function gl.LineStipple(ignoredString: string)
```
@param `ignoredString` - The value of this string is ignored, but it still does something.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3833-L3836" target="_blank">source</a>]








### gl.LineStipple
---
```lua
function gl.LineStipple(
  factor: integer,
  pattern: integer,
  shift: integer?
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3837-L3842" target="_blank">source</a>]








### gl.LineWidth
---
```lua
function gl.LineWidth(width: number)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3892-L3895" target="_blank">source</a>]








### gl.PointSize
---
```lua
function gl.PointSize(size: number)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3906-L3909" target="_blank">source</a>]








### gl.PointSprite
---
```lua
function gl.PointSprite(
  enable: boolean,
  enableCoordReplace: boolean?,
  coordOrigin: boolean?
)
```
@param `coordOrigin` - `true` for upper left, `false` for lower left, otherwise no change.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3920-L3925" target="_blank">source</a>]








### gl.PointParameter
---
```lua
function gl.PointParameter(
  atten0: number,
  atten1: number,
  atten2: number,
  sizeMin: number?,
  sizeMax: number?,
  sizeFade: number?
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3954-L3962" target="_blank">source</a>]








### gl.Texture
---
```lua
function gl.Texture(
  texNum: integer,
  enable: boolean?
) ->  boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3990-L3995" target="_blank">source</a>]








### gl.Texture
---
```lua
function gl.Texture(enable: boolean) ->  boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L3996-L4000" target="_blank">source</a>]








### gl.Texture
---
```lua
function gl.Texture(
  texNum: integer,
  image: string
) ->  boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4001-L4006" target="_blank">source</a>]








### gl.Texture
---
```lua
function gl.Texture(image: string) ->  boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4007-L4011" target="_blank">source</a>]








### gl.CreateTexture
---
```lua
function gl.CreateTexture(
  xsize: integer,
  ysize: integer,
  texture: Texture
) -> texName string?
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4140-L4146" target="_blank">source</a>]








### gl.CreateTexture
---
```lua
function gl.CreateTexture(
  xsize: integer,
  ysize: integer,
  zsize: integer,
  texture: Texture
) -> texName string?
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4147-L4154" target="_blank">source</a>]








### gl.ChangeTextureParams
---
```lua
function gl.ChangeTextureParams(
  texName: string,
  params: Texture
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4222-L4226" target="_blank">source</a>]








### gl.DeleteTexture
---
```lua
function gl.DeleteTexture(texName: string) ->  boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4253-L4257" target="_blank">source</a>]








### gl.DeleteTextureFBO
---
```lua
function gl.DeleteTextureFBO(texName: string) ->  boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4274-L4278" target="_blank">source</a>]








### gl.TextureInfo
---
```lua
function gl.TextureInfo(texName: string) -> textureInfo TextureInfo
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4299-L4303" target="_blank">source</a>]








### gl.CopyToTexture
---
```lua
function gl.CopyToTexture(
  texName: string,
  xoff: integer,
  yoff: integer,
  x: integer,
  y: integer,
  w: integer,
  h: integer,
  target: GL?,
  level: GL?
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4325-L4336" target="_blank">source</a>]








### gl.RenderToTexture
---
```lua
function gl.RenderToTexture(
  texName: string,
  fun: unknown,
  ...: any
)
```
@param `...` - Arguments to the function.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4373-L4378" target="_blank">source</a>]








### gl.GenerateMipmap
---
```lua
function gl.GenerateMipmap(texName: string)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4427-L4430" target="_blank">source</a>]








### gl.ActiveTexture
---
```lua
function gl.ActiveTexture(
  texNum: integer,
  func: function,
  ...: any
)
```
@param `...` - Arguments to the function.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4452-L4457" target="_blank">source</a>]








### gl.TextEnv
---
```lua
function gl.TextEnv(
  target: GL,
  pname: GL,
  value: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4486-L4491" target="_blank">source</a>]








### gl.TextEnv
---
```lua
function gl.TextEnv(
  target: GL,
  pname: GL,
  r: number?,
  g: number?,
  b: number?,
  a: number?
)
```
@param `r` - (Default: `0.0`)

@param `g` - (Default: `0.0`)

@param `b` - (Default: `0.0`)

@param `a` - (Default: `0.0`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4492-L4500" target="_blank">source</a>]








### gl.MultiTexEnv
---
```lua
function gl.MultiTexEnv(
  texNum: integer,
  target: GL,
  pname: GL,
  value: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4530-L4536" target="_blank">source</a>]








### gl.MultiTexEnv
---
```lua
function gl.MultiTexEnv(
  texNum: integer,
  target: GL,
  pname: GL,
  r: number?,
  g: number?,
  b: number?,
  a: number?
)
```
@param `r` - (Default: `0.0`)

@param `g` - (Default: `0.0`)

@param `b` - (Default: `0.0`)

@param `a` - (Default: `0.0`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4537-L4546" target="_blank">source</a>]








### gl.TexGen
---
```lua
function gl.TexGen(
  target: GL,
  state: boolean
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4597-L4601" target="_blank">source</a>]








### gl.TexGen
---
```lua
function gl.TexGen(
  target: GL,
  pname: GL,
  value: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4602-L4607" target="_blank">source</a>]








### gl.TexGen
---
```lua
function gl.TexGen(
  target: GL,
  pname: GL,
  r: number?,
  g: number?,
  b: number?,
  a: number?
)
```
@param `r` - (Default: `0.0`)

@param `g` - (Default: `0.0`)

@param `b` - (Default: `0.0`)

@param `a` - (Default: `0.0`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4608-L4616" target="_blank">source</a>]








### gl.MultiTexGen
---
```lua
function gl.MultiTexGen(
  texNum: integer,
  target: GL,
  state: boolean
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4655-L4660" target="_blank">source</a>]








### gl.MultiTexGen
---
```lua
function gl.MultiTexGen(
  texNum: integer,
  target: GL,
  pname: GL,
  value: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4661-L4667" target="_blank">source</a>]








### gl.MultiTexGen
---
```lua
function gl.MultiTexGen(
  texNum: integer,
  target: GL,
  pname: GL,
  r: number?,
  g: number?,
  b: number?,
  a: number?
)
```
@param `r` - (Default: `0.0`)

@param `g` - (Default: `0.0`)

@param `b` - (Default: `0.0`)

@param `a` - (Default: `0.0`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4668-L4677" target="_blank">source</a>]








### gl.BindImageTexture
---
```lua
function gl.BindImageTexture(
  unit: integer,
  texID: string?,
  level: integer?,
  layer: integer?,
  access: (GL.READ_ONLY|GL.WRITE_ONLY|GL.READ_WRITE)?,
  format: integer
)
```
@param `texID` - (nil breaks any existing binding to the image unit)

@param `level` - (Default: 0)

@param `layer` - (nil binds the entire texture(array/cube), an integer binds a specific layer, ignored by gl if the texture does not support layered bindings)

@param `access` - (Default: `GL.READ_WRITE`)

@param `format` - (Example: GL.RGBA16F)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4726-L4752" target="_blank">source</a>]

For format parameters refer to
https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml
and
https://beyond-all-reason.github.io/RecoilEngine/lua-api/types/GL#rgba32f

Example uses
local my_texture_id = gl.CreateTexture(...)

-- bind layer 1 of my_texture_id if it supports layered bindings to image unit 0
gl.BindImageTexture(0, my_texture_id, 0, 1, GL.READ_WRITE, GL.RGBA16F)

-- bind all layers of my_texture_id if it supports layered bindings to image unit 0
gl.BindImageTexture(0, my_texture_id, 0, nil, GL.READ_WRITE, GL.RGBA16F)

-- unbind any texture attached to image unit 0
gl.BindImageTexture(0, nil, nil, nil, nil, GL.RGBA16F)








### gl.CreateTextureAtlas
---
```lua
function gl.CreateTextureAtlas(
  xsize: integer,
  ysize: integer,
  allocType: integer?
) -> texName string
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4864-L4870" target="_blank">source</a>]








### gl.FinalizeTextureAtlas
---
```lua
function gl.FinalizeTextureAtlas(texName: string) ->  boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4895-L4899" target="_blank">source</a>]








### gl.DeleteTextureAtlas
---
```lua
function gl.DeleteTextureAtlas(texName: string) ->  boolean
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4915-L4919" target="_blank">source</a>]








### gl.AddAtlasTexture
---
```lua
function gl.AddAtlasTexture(
  texName: string,
  subAtlasTexName: string
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4931-L4935" target="_blank">source</a>]








### gl.GetAtlasTexture
---
```lua
function gl.GetAtlasTexture(
  texName: string,
  subAtlasTexName: string
)
 -> x1 number
 -> x2 number
 -> y1 number
 -> y2 number

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L4982-L4990" target="_blank">source</a>]








### gl.GetEngineAtlasTextures
---
```lua
function gl.GetEngineAtlasTextures(atlasName: ("$explosions"|"$groundfx")) -> atlasTextures table<string,float4>
```

@return `atlasTextures` - Table of x1,x2,y1,y2 coordinates by texture name.





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5016-L5020" target="_blank">source</a>]








### gl.Clear
---
```lua
function gl.Clear(
  bits: GL,
  val: number
)
```
@param `bits` - `GL.DEPTH_BUFFER_BIT` or `GL.STENCIL_BUFFER_BIT`.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5070-L5074" target="_blank">source</a>]








### gl.Clear
---
```lua
function gl.Clear(
  bits: GL,
  r: number,
  g: number,
  b: number,
  a: number
)
```
@param `bits` - `GL.COLOR_BUFFER_BIT` or `GL.ACCUM_BUFFER_BIT`.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5075-L5082" target="_blank">source</a>]








### gl.SwapBuffers
---
```lua
function gl.SwapBuffers()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5121-L5123" target="_blank">source</a>]








### gl.Translate
---
```lua
function gl.Translate(
  x: number,
  y: number,
  z: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5138-L5143" target="_blank">source</a>]








### gl.Scale
---
```lua
function gl.Scale(
  x: number,
  y: number,
  z: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5156-L5161" target="_blank">source</a>]








### gl.Rotate
---
```lua
function gl.Rotate(
  r: number,
  x: number,
  y: number,
  z: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5174-L5180" target="_blank">source</a>]








### gl.Ortho
---
```lua
function gl.Ortho(
  left: number,
  right: number,
  bottom: number,
  top: number,
  near: number,
  far: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5194-L5202" target="_blank">source</a>]








### gl.Frustum
---
```lua
function gl.Frustum(
  left: number,
  right: number,
  bottom: number,
  top: number,
  near: number,
  far: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5218-L5226" target="_blank">source</a>]








### gl.Billboard
---
```lua
function gl.Billboard()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5242-L5244" target="_blank">source</a>]








### gl.Light
---
```lua
function gl.Light(
  light: integer,
  enable: boolean
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5256-L5260" target="_blank">source</a>]








### gl.Light
---
```lua
function gl.Light(
  light: integer,
  pname: GL,
  param: GL
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5261-L5266" target="_blank">source</a>]








### gl.Light
---
```lua
function gl.Light(
  light: integer,
  pname: GL,
  r: number,
  g: number,
  b: number,
  a: number?
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5267-L5275" target="_blank">source</a>]








### gl.ClipPlane
---
```lua
function gl.ClipPlane(
  plane: integer,
  enable: boolean
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5326-L5330" target="_blank">source</a>]








### gl.ClipPlane
---
```lua
function gl.ClipPlane(
  plane: integer,
  equation0: number,
  equation1: number,
  equation2: number,
  equation3: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5331-L5338" target="_blank">source</a>]








### gl.ClipDistance
---
```lua
function gl.ClipDistance(
  clipId: integer,
  enable: boolean
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5368-L5372" target="_blank">source</a>]








### gl.MatrixMode
---
```lua
function gl.MatrixMode(mode: GL)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5401-L5404" target="_blank">source</a>]








### gl.LoadIdentity
---
```lua
function gl.LoadIdentity()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5417-L5419" target="_blank">source</a>]








### gl.LoadMatrix
---
```lua
function gl.LoadMatrix(matrix: string)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5454-L5457" target="_blank">source</a>]








### gl.LoadMatrix
---
```lua
function gl.LoadMatrix(matrix: Matrix4x4)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5458-L5461" target="_blank">source</a>]








### gl.LoadMatrix
---
```lua
function gl.LoadMatrix()
 -> m11 number
 -> m12 number
 -> m13 number
 -> m14 number
 -> m21 number
 -> m22 number
 -> m23 number
 -> m24 number
 -> m31 number
 -> m32 number
 -> m33 number
 -> m34 number
 -> m41 number
 -> m42 number
 -> m43 number
 -> m44 number

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5462-L5480" target="_blank">source</a>]








### gl.MultMatrix
---
```lua
function gl.MultMatrix(matrixName: string)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5513-L5516" target="_blank">source</a>]








### gl.MultMatrix
---
```lua
function gl.MultMatrix(matrix: Matrix4x4)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5517-L5520" target="_blank">source</a>]








### gl.MultMatrix
---
```lua
function gl.MultMatrix(
  m11: number,
  m12: number,
  m13: number,
  m14: number,
  m21: number,
  m22: number,
  m23: number,
  m24: number,
  m31: number,
  m32: number,
  m33: number,
  m34: number,
  m41: number,
  m42: number,
  m43: number,
  m44: number
)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5521-L5539" target="_blank">source</a>]








### gl.PushMatrix
---
```lua
function gl.PushMatrix()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5572-L5574" target="_blank">source</a>]








### gl.PopMatrix
---
```lua
function gl.PopMatrix()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5593-L5595" target="_blank">source</a>]








### gl.PushPopMatrix
---
```lua
function gl.PushPopMatrix(
  matMode1: GL,
  func: fun(),
  ...: any
)
```
@param `...` - Arguments to the function.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5613-L5618" target="_blank">source</a>]








### gl.PushPopMatrix
---
```lua
function gl.PushPopMatrix(
  func: fun(),
  ...: any
)
```
@param `...` - Arguments to the function.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5619-L5623" target="_blank">source</a>]








### gl.GetMatrixData
---
```lua
function gl.GetMatrixData(
  type: GL,
  index: integer
) -> The number
```
@param `type` - Matrix type (`GL.PROJECTION`, `GL.MODELVIEW`, `GL.TEXTURE`).

@param `index` - Matrix index in range `[1, 16]`.


@return `The` - value.





Get value at index of matrix.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5671-L5678" target="_blank">source</a>]








### gl.GetMatrixData
---
```lua
function gl.GetMatrixData(type: GL) -> The Matrix4x4
```
@param `type` - Matrix type (`GL.PROJECTION`, `GL.MODELVIEW`, `GL.TEXTURE`).


@return `The` - matrix.





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5679-L5683" target="_blank">source</a>]








### gl.GetMatrixData
---
```lua
function gl.GetMatrixData(index: integer) -> The number
```
@param `index` - Matrix index in range `[1, 16]`.


@return `The` - value.





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5684-L5688" target="_blank">source</a>]








### gl.GetMatrixData
---
```lua
function gl.GetMatrixData(name: MatrixName) -> The Matrix4x4
```
@param `name` - The matrix name.


@return `The` - matrix.





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5689-L5693" target="_blank">source</a>]








### gl.PushAttrib
---
```lua
function gl.PushAttrib(mask: GL?)
```
@param `mask` - (Default: `GL.ALL_ATTRIB_BITS`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5756-L5759" target="_blank">source</a>]








### gl.PopAttrib
---
```lua
function gl.PopAttrib()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5773-L5775" target="_blank">source</a>]








### gl.UnsafeState
---
```lua
function gl.UnsafeState(
  state: GL,
  func: fun(),
  ...: any
)
```
@param `...` - Arguments to the function.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5784-L5789" target="_blank">source</a>]








### gl.UnsafeState
---
```lua
function gl.UnsafeState(
  state: GL,
  reverse: boolean,
  func: fun(),
  ...: any
)
```
@param `...` - Arguments to the function.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5790-L5796" target="_blank">source</a>]








### gl.GetFixedState
---
```lua
function gl.GetFixedState(
  param: string,
  toStr: boolean?
)
 -> enabled boolean
 -> values any ...

```
@param `toStr` - (Default: `false`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L5823-L5829" target="_blank">source</a>]








### gl.CreateList
---
```lua
function gl.CreateList(
  func: fun(),
  ...: any
)
```
@param `...` - Arguments to the function.






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6111-L6115" target="_blank">source</a>]








### gl.CallList
---
```lua
function gl.CallList(listIndex: integer)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6163-L6166" target="_blank">source</a>]








### gl.DeleteList
---
```lua
function gl.DeleteList(listIndex: integer)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6187-L6190" target="_blank">source</a>]








### gl.Flush
---
```lua
function gl.Flush()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6210-L6212" target="_blank">source</a>]








### gl.Finish
---
```lua
function gl.Finish()
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6221-L6223" target="_blank">source</a>]








### gl.ReadPixels
---
```lua
function gl.ReadPixels(
  x: integer,
  y: integer,
  w: 1,
  h: 1,
  format: GL?
) -> Color number ...
```
@param `format` - (Default: `GL.RGBA`)


@return `Color` - values (color size based on format).





Get single pixel.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6279-L6288" target="_blank">source</a>]








### gl.ReadPixels
---
```lua
function gl.ReadPixels(
  x: integer,
  y: integer,
  w: 1,
  h: integer,
  format: GL?
) -> Column number[][]
```
@param `format` - (Default: `GL.RGBA`)


@return `Column` - of color values (color size based on format).





Get column of pixels.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6289-L6298" target="_blank">source</a>]








### gl.ReadPixels
---
```lua
function gl.ReadPixels(
  x: integer,
  y: integer,
  w: integer,
  h: 1,
  format: GL?
) -> Row number[][]
```
@param `format` - (Default: `GL.RGBA`)


@return `Row` - of color values (color size based on format).





Get row of pixels.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6299-L6308" target="_blank">source</a>]








### gl.ReadPixels
---
```lua
function gl.ReadPixels(
  x: integer,
  y: integer,
  w: integer,
  h: integer,
  format: GL?
) -> Array number[][][]
```
@param `format` - (Default: `GL.RGBA`)


@return `Array` - of columns of color values (color size based on format).





Get row of pixels.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6309-L6318" target="_blank">source</a>]








### gl.SaveImage
---
```lua
function gl.SaveImage(
  x: integer,
  y: integer,
  width: integer,
  height: integer,
  filename: string,
  options: SaveImageOptions?
) -> success boolean?
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6398-L6407" target="_blank">source</a>]








### gl.CreateQuery
---
```lua
function gl.CreateQuery() -> query any
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6481-L6484" target="_blank">source</a>]








### gl.DeleteQuery
---
```lua
function gl.DeleteQuery(query: any)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6505-L6508" target="_blank">source</a>]








### gl.RunQuery
---
```lua
function gl.RunQuery(query: any)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6532-L6535" target="_blank">source</a>]








### gl.GetQuery
---
```lua
function gl.GetQuery(query: any) -> count integer
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6572-L6576" target="_blank">source</a>]








### gl.GetGlobalTexNames
---
```lua
function gl.GetGlobalTexNames() -> List string[]
```

@return `List` - of texture names.





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6599-L6602" target="_blank">source</a>]








### gl.GetGlobalTexCoords
---
```lua
function gl.GetGlobalTexCoords()
 -> xstart number
 -> ystart number
 -> xend number
 -> yend number

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6618-L6624" target="_blank">source</a>]








### gl.GetShadowMapParams
---
```lua
function gl.GetShadowMapParams()
 -> x number
 -> y number
 -> z number
 -> w number

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6641-L6647" target="_blank">source</a>]








### gl.GetAtmosphere
---
```lua
function gl.GetAtmosphere()
 -> lightDirX number
 -> lightDirY number
 -> lightDirZ number

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6657-L6662" target="_blank">source</a>]








### gl.GetAtmosphere
---
```lua
function gl.GetAtmosphere(param: ("fogStart"|"fogEnd"|"pos"|"fogColor"|"skyColor"|"sunColor"...)) ->  any ...
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6663-L6667" target="_blank">source</a>]








### gl.GetSun
---
```lua
function gl.GetSun()
 -> lightDirX number
 -> lightDirY number
 -> lightDirZ number

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6720-L6725" target="_blank">source</a>]








### gl.GetSun
---
```lua
function gl.GetSun(
  param: ("pos"|"dir"|"specularExponent"|"shadowDensity"|"diffuse"|"ambient"...),
  mode: ("ground"|"unit")
)
 -> data1 number?
 -> data2 number?
 -> data3 number?

```
@param `mode` - (Default: `"ground"`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6726-L6733" target="_blank">source</a>]








### gl.GetWaterRendering
---
```lua
function gl.GetWaterRendering(key: string) -> value any ...
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6799-L6803" target="_blank">source</a>]








### gl.GetMapRendering
---
```lua
function gl.GetMapRendering(key: string) -> value any ...
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L6981-L6985" target="_blank">source</a>]








### gl.ObjectLabel
---
```lua
function gl.ObjectLabel(
  objectTypeIdentifier: GL,
  objectID: integer,
  label: string
)
```
@param `objectTypeIdentifier` - Specifies the type of object being labeled.

@param `objectID` - Specifies the name or ID of the object to label.

@param `label` - A string containing the label to be assigned to the object.






Labels an object for use with debugging tools.
May be unavailable and `nil` if the platform doesn't support the feature.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L7025-L7033" target="_blank">source</a>]








### gl.PushDebugGroup
---
```lua
function gl.PushDebugGroup(
  id: integer,
  message: string,
  sourceIsThirdParty: boolean
) ->  nil
```
@param `id` - A numeric identifier for the group, can be any unique number.

@param `message` - A human-readable string describing the debug group. Will be truncated if longer than driver-specific limit

@param `sourceIsThirdParty` - Set the source tag, true for GL_DEBUG_SOURCE_THIRD_PARTY, false for GL_DEBUG_SOURCE_APPLICATION. default false






Pushes a debug marker for debugging tools such as `nVidia nSight 2024.04`,
see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPushDebugGroup.xhtml .

May be unavailable and `nil` if the platform doesn't support the feature.

Groups are basically named scopes similar to tracy's, and are pushed/popped independently
from GL attribute/matrix push/pop (though of course makes sense to put them together).

Tools are known to struggle to see the annotation for FBOs if they are raw bound.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L7062-L7078" target="_blank">source</a>]








### gl.PopDebugGroup
---
```lua
function gl.PopDebugGroup() ->  nil
```





Pops the most recent GL debug group from the stack (does NOT take the numerical ID from push).
May be unavailable and `nil` if the platform doesn't support the feature.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaOpenGL.cpp#L7100-L7107" target="_blank">source</a>]








### gl.CreateFBO
---
```lua
function gl.CreateFBO(fboDesc: FBODescription) -> fbo FBO
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFBOs.cpp#L467-L471" target="_blank">source</a>]








### gl.DeleteFBO
---
```lua
function gl.DeleteFBO(fbo: FBO)
```





This doesn't delete the attached objects!

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFBOs.cpp#L542-L547" target="_blank">source</a>]








### gl.IsValidFBO
---
```lua
function gl.IsValidFBO(
  fbo: FBO,
  target: GL?
)
 -> valid boolean
 -> status number?

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFBOs.cpp#L560-L566" target="_blank">source</a>]








### gl.ActiveFBO
---
```lua
function gl.ActiveFBO(
  fbo: FBO,
  func: fun(...),
  ...: any
)
```
@param `...` - args






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFBOs.cpp#L601-L606" target="_blank">source</a>]








### gl.ActiveFBO
---
```lua
function gl.ActiveFBO(
  fbo: FBO,
  target: GL?,
  func: fun(...),
  ...: any
)
```
@param `...` - args






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFBOs.cpp#L607-L613" target="_blank">source</a>]








### gl.RawBindFBO
---
```lua
function gl.RawBindFBO(
  fbo: nil,
  target: GL?,
  rawFboId: integer?
) ->  nil
```
@param `target` - (Default: `GL_FRAMEBUFFER_EXT`)

@param `rawFboId` - (Default: `0`)






Bind default or specified via rawFboId numeric id of FBO

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFBOs.cpp#L677-L685" target="_blank">source</a>]








### gl.RawBindFBO
---
```lua
function gl.RawBindFBO(
  fbo: FBO,
  target: GL?
) -> previouslyBoundRawFboId number
```
@param `target` - (Default: `fbo.target`)






[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFBOs.cpp#L686-L691" target="_blank">source</a>]








### gl.BlitFBO
---
```lua
function gl.BlitFBO(
  x0Src: number,
  y0Src: number,
  x1Src: number,
  y1Src: number,
  x0Dst: number,
  y0Dst: number,
  x1Dst: number,
  y1Dst: number,
  mask: number?,
  filter: number?
)
```
@param `mask` - (Default: GL_COLOR_BUFFER_BIT)

@param `filter` - (Default: GL_NEAREST)






needs `GLAD_GL_EXT_framebuffer_blit`

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFBOs.cpp#L719-L732" target="_blank">source</a>]








### gl.BlitFBO
---
```lua
function gl.BlitFBO(
  fboSrc: FBO,
  x0Src: number,
  y0Src: number,
  x1Src: number,
  y1Src: number,
  fboDst: FBO,
  x0Dst: number,
  y0Dst: number,
  x1Dst: number,
  y1Dst: number,
  mask: number?,
  filter: number?
)
```
@param `mask` - (Default: GL_COLOR_BUFFER_BIT)

@param `filter` - (Default: GL_NEAREST)






needs `GLAD_GL_EXT_framebuffer_blit`

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFBOs.cpp#L733-L748" target="_blank">source</a>]








### gl.ClearAttachmentFBO
---
```lua
function gl.ClearAttachmentFBO(
  target: number?,
  attachment: (GL|Attachment),
  clearValue0: number?,
  clearValue1: number?,
  clearValue2: number?,
  clearValue3: number?
) -> success boolean
```
@param `target` - (Default: `GL.FRAMEBUFFER`)

@param `attachment` - (e.g. `"color0"` or `GL.COLOR_ATTACHMENT0`)

@param `clearValue0` - (Default: `0`)

@param `clearValue1` - (Default: `0`)

@param `clearValue2` - (Default: `0`)

@param `clearValue3` - (Default: `0`)






needs `Platform.glVersionNum >= 30`
Clears the "attachment" of the currently bound FBO type "target" with "clearValues"

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFBOs.cpp#L818-L829" target="_blank">source</a>]








### gl.GetVAO
---
```lua
function gl.GetVAO() -> vao VAO?
```

@return `vao` - The VAO ref on success, else `nil`





Example:
```
local myVAO = gl.GetVAO()
if myVAO == nil then Spring.Echo("Failed to get VAO") end
```

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaVAO.cpp#L75-L84" target="_blank">source</a>]








### gl.GetVBO
---
```lua
function gl.GetVBO(
  bufferType: GL?,
  freqUpdated: boolean?
) -> VBO VBO?
```
@param `bufferType` - (Default: `GL.ARRAY_BUFFER`) The buffer type to use.

Accepts the following:
- `GL.ARRAY_BUFFER` for vertex data.
- `GL.ELEMENT_ARRAY_BUFFER` for vertex indices.
- `GL.UNIFORM_BUFFER`
- `GL.SHADER_STORAGE_BUFFER`

@param `freqUpdated` - (Default: `true`)

`true` to updated frequently, `false` to update only once.


@return `VBO` - The VBO ref on success, or nil if not supported or an error occurred.





Example:

```lua
local myVBO = gl.GetVBO()
if myVBO == nil then Spring.Echo("Failed to get VBO") end
```

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaVBO.cpp#L125-L152" target="_blank">source</a>]





@see GL.OpenGL_Buffer_Types




### gl.AddFallbackFont
---
```lua
function gl.AddFallbackFont(filePath: string) -> success boolean
```
@param `filePath` - VFS path to the file, for example "fonts/myfont.ttf". Uses VFS.RAW_FIRST access mode.






Adds a fallback font for the font rendering engine.

Fonts added first will have higher priority.
When a glyph isn't found when rendering a font, the fallback fonts
will be searched first, otherwise os fonts will be used.

The application should listen for the unsynced 'FontsChanged' callin so
modules can clear any already reserved display lists or other relevant
caches.

Note the callin won't be executed at the time of calling this method,
but later, on the Update cycle (before other Update and Draw callins).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFonts.cpp#L219-L235" target="_blank">source</a>]








### gl.ClearFallbackFonts
---
```lua
function gl.ClearFallbackFonts() ->  nil
```





Clears all fallback fonts.

See the note at 'AddFallbackFont' about the 'FontsChanged' callin,
it also applies when calling this method.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaFonts.cpp#L247-L254" target="_blank">source</a>]








### gl.GetShaderLog
---
```lua
function gl.GetShaderLog() -> infoLog string
```





Returns the shader compilation error log. This is empty if the shader linking failed, in that case, check your in/out blocks and ensure they match.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L199-L203" target="_blank">source</a>]








### gl.CreateShader
---
```lua
function gl.CreateShader(shaderParams: ShaderParams) -> shaderID integer
```





Create a shader.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L622-L628" target="_blank">source</a>]








### gl.DeleteShader
---
```lua
function gl.DeleteShader(shaderID: integer)
```





Deletes a shader identified by shaderID

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L790-L794" target="_blank">source</a>]








### gl.UseShader
---
```lua
function gl.UseShader(shaderID: integer) -> linked boolean
```





Binds a shader program identified by shaderID. Pass 0 to disable the shader. Returns whether the shader was successfully bound.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L807-L812" target="_blank">source</a>]








### gl.ActiveShader
---
```lua
function gl.ActiveShader(
  shaderID: integer,
  func: function,
  ...: any
)
```
@param `...` - Arguments






Binds a shader program identified by shaderID, and calls the Lua func with
the specified arguments.

Can be used in NON-drawing events (to update uniforms etc.)!

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L840-L850" target="_blank">source</a>]








### gl.GetActiveUniforms
---
```lua
function gl.GetActiveUniforms(shaderID: integer) -> activeUniforms ActiveUniform[]
```





Query the active (actually used) uniforms of a shader and identify their
names, types (float, int, uint) and sizes (float, vec4, ...).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L932-L939" target="_blank">source</a>]








### gl.GetUniformLocation
---
```lua
function gl.GetUniformLocation(
  shaderID: integer,
  name: string
) -> locationID GL
```





Returns the locationID of a shaders uniform. Needed for changing uniform
values with function `gl.Uniform`.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L967-L975" target="_blank">source</a>]








### gl.Uniform
---
```lua
function gl.Uniform(
  locationID: (GL|string),
  f1: number,
  f2: number?,
  f3: number?,
  f4: number?
)
```
@param `locationID` - uniformName






Sets the uniform float value at the locationID for the currently active
shader. Shader must be activated before setting uniforms.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L1046-L1056" target="_blank">source</a>]








### gl.UniformInt
---
```lua
function gl.UniformInt(
  locationID: (integer|string),
  int1: integer,
  int2: integer?,
  int3: integer?,
  int4: integer?
)
```
@param `locationID` - uniformName






Sets the uniform int value at the locationID for the currently active shader.
Shader must be activated before setting uniforms.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L1087-L1097" target="_blank">source</a>]








### gl.UniformArray
---
```lua
function gl.UniformArray(
  locationID: (integer|string),
  type: UniformArrayType,
  uniforms: number[]
)
```
@param `locationID` - uniformName

@param `uniforms` - Array up to 1024 elements






Sets the an array of uniform values at the locationID for the currently
active shader.

Shader must be activated before setting uniforms.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L1157-L1167" target="_blank">source</a>]








### gl.UniformMatrix
---
```lua
function gl.UniformMatrix(
  locationID: (integer|string),
  matrix: MatrixName
)
```
@param `locationID` - uniformName

@param `matrix` - Name of common matrix.






Sets the a uniform mat4 locationID for the currently active shader.

Shader must be activated before setting uniforms.

Can set one one common matrix like shadow, or by passing 16 additional
numbers for the matrix.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L1206-L1217" target="_blank">source</a>]








### gl.UniformMatrix
---
```lua
function gl.UniformMatrix(
  locationID: (integer|string),
  matrix: number[]
)
```
@param `locationID` - uniformName

@param `matrix` - A 2x2, 3x3 or 4x4 matrix.






Sets the a uniform mat4 locationID for the currently active shader.

Shader must be activated before setting uniforms.

Can set one one common matrix like shadow, or by passing 16 additional
numbers for the matrix.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L1219-L1230" target="_blank">source</a>]








### gl.GetEngineUniformBufferDef
---
```lua
function gl.GetEngineUniformBufferDef(index: number) -> glslDefinition string
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L1307-L1315" target="_blank">source</a>]

Return the GLSL compliant definition of UniformMatricesBuffer(idx=0) or UniformParamsBuffer(idx=1) structure.








### gl.GetEngineModelUniformDataDef
---
```lua
function gl.GetEngineModelUniformDataDef(index: number) -> glslDefinition string
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L1329-L1337" target="_blank">source</a>]

Return the GLSL compliant definition of ModelUniformData structure (per Unit/Feature buffer available on GPU)








### gl.GetEngineModelUniformDataSize
---
```lua
function gl.GetEngineModelUniformDataSize(index: number)
 -> sizeInElements number
 -> sizeInBytesOnCPU number

```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L1347-L1357" target="_blank">source</a>]

Return the current size values of ModelUniformData structure (per Unit/Feature buffer available on GPU)








### gl.SetGeometryShaderParameter
---
```lua
function gl.SetGeometryShaderParameter(
  shaderID: integer,
  key: number,
  value: number
) ->  nil
```





Sets the Geometry shader parameters for shaderID. Needed by geometry shader programs (check the opengl GL_ARB_geometry_shader4 extension for glProgramParameteri)

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L1370-L1377" target="_blank">source</a>]








### gl.SetTesselationShaderParameter
---
```lua
function gl.SetTesselationShaderParameter(
  param: integer,
  value: integer
) ->  nil
```





Sets the tesselation shader parameters for `shaderID`.

Needed by tesselation shader programs. (Check the opengl
`GL_ARB_tessellation_shader` extension for `glProgramParameteri`).

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaShaders.cpp#L1398-L1408" target="_blank">source</a>]








### gl.CreateRBO
---
```lua
function gl.CreateRBO(
  xsize: integer,
  ysize: integer,
  data: CreateRBOData
) ->  RBO
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaRBOs.cpp#L157-L163" target="_blank">source</a>]








### gl.DeleteRBO
---
```lua
function gl.DeleteRBO(rbo: RBO)
```





[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Lua/LuaRBOs.cpp#L233-L236" target="_blank">source</a>]












{% endraw %}