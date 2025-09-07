---@meta

--------------------------------------------------------------------------------
-- Vectors
--------------------------------------------------------------------------------

---Cartesian double (XY)
---
---@class xy
---@helper
---@field [1] number x
---@field [2] number y

---@alias float2 xy

---Cartesian triple (XYZ)
---
---@class xyz
---@helper
---@field [1] number x
---@field [2] number y
---@field [3] number z

---@alias float3 xyz

---Cartesian quadruple (XYZW)
---
---@class xyzw
---@helper
---@field [1] number x
---@field [2] number y
---@field [3] number z
---@field [4] number w

---@alias float4 xyzw

--------------------------------------------------------------------------------
-- Color
--------------------------------------------------------------------------------

---Color triple (RGB)
---
---@class rgb
---@helper
---@field [1] number Red value.
---@field [2] number Green value.
---@field [3] number Blue value.

---Color quadruple (RGBA)
---
---@class rgba
---@helper
---@field [1] number Red value.
---@field [2] number Green value.
---@field [3] number Blue value.
---@field [4] number Alpha value.

---Indicator bytes representing color code operations during font rendering
---
---@class TextColorCode
---@helper
---@field Color string Indicates that the following bytes contain color code information
---@field ColorAndOutline string Indicates that the following bytes contain color code and outline information
---@field Reset string Indicates reset of the current color

--------------------------------------------------------------------------------
-- Camera
--------------------------------------------------------------------------------

---@alias CameraMode
---| 0 # fps
---| 1 # ta
---| 2 # spring
---| 3 # rot
---| 4 # free
---| 5 # ov
---| 6 # dummy

---@alias CameraName
---| "ta"
---| "spring"
---| "rot"
---| "ov"
---| "free"
---| "fps"
---| "dummy"

---Parameters for camera state.
---
---Highly dependent on the type of the current camera controller.
---
---@class CameraState
---@helper
---@field name CameraName The camera name.
---@field mode CameraMode The camera mode.
---@field fov number?
---@field px number? Position X of the ground point in screen center.
---@field py number? Position Y of the ground point in screen center.
---@field pz number? Position Z of the ground point in screen center.
---@field dx number? Camera direction vector X.
---@field dy number? Camera direction vector Y.
---@field dz number? Camera direction vector Z.
---@field rx number? Camera rotation angle on X axis. (spring)
---@field ry number? Camera rotation angle on Y axis. (spring)
---@field rz number? Camera rotation angle on Z axis. (spring)
---@field angle number? Camera rotation angle on X axis (aka tilt/pitch). (ta)
---@field flipped number? `-1` for when south is down, `1` for when north is down. (ta)
---@field dist number? Camera distance from the ground. (spring)
---@field height number? Camera distance from the ground. (ta)
---@field oldHeight number? Camera distance from the ground, cannot be changed. (rot)

---Contains modoption information
---
---The files `ModOptions.lua` and `EngineOptions.lua` must return a table of type `ModOption[]`
---
---@class ModOption
---@field key string The key that will be returned as the index for the option via `Spring.GetModOptions()`. The actual value is lowercased, recommended to use lowercased keys.
---@field name string? (Default: `key`)
---@field desc string? (Default: `name`)
---@field section string? (Default: `""`)
---@field style string? (Default: `""`)
---@field scope string (Default: `"scope"`)
---@field type "bool"|"string"|"number"|"list"|"section"
---@field def any The default value. When not passed: boolean: `false`, string: `""`, number: `0`, list: `items[1]`
---@field min number? The minimum value allowed, only valid when type: "number". (Default: `-1.0e30f`)
---@field max number? The maximum value allowed, only valid when type: "number". (Default: `1.0e30f`)
---@field maxlen number? The maximum number of characters allowed, only valid when type: "string". (Default: `0`)
---@field items {key: string, name: string?, desc: string?}[] The list of items. Only valid when type: "list". Name defaults to key and desc defaults to name.

--------------------------------------------------------------------------------
-- Resources
--------------------------------------------------------------------------------

---@alias ResourceName "metal"|"energy"|"m"|"e"

---@alias StorageName "metalStorage"|"energyStorage"|"ms"|"es"

---@alias ResourceUsage table<ResourceName, number>
