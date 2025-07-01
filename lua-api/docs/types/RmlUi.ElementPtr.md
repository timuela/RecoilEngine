---
layout: default
title: ElementPtr
parent: Lua API
permalink: lua-api/types/ElementPtr
---

{% raw %}

# class ElementPtr



- namespace: RmlUi



Represents an owned element. This type is mainly used to modify the DOM tree by passing the object into other elements. For example `RmlUi.Element:AppendChild()`.
A current limitation in the Lua plugin is that Element member properties and functions cannot be used directly on this type.

[<a href="https://github.com/beyond-all-reason/RecoilEngine/blob/b4d0041e4c68c34dace9abf492f9193d28ef5d7e/rts/Rml/SolLua/bind/Global.cpp#L105-L109" target="_blank">source</a>]







---




{% endraw %}