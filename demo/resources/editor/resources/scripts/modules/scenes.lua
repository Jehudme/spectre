local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local ffi = require("ffi")
local world = ecs.from_ptr(g_world)

Scenes = {}
_G.modules = _G.modules or {}
_G.modules["Scenes"] = Scenes

-- TODO: Implement an box int the left with an list of all state and an other list of all scenes. with an search bar at the top
-- TODO: Implement right click for the option to create, delete, rename, duplicate, duplicate states and scenes.
-- TODO: Implement the state editor mode. in the right side, an table with the scene in the correct order. right click on the table to add, remove, move up, move down, scene in the selected state.
-- TODO: Implement the scene editor mode. in the top right side reuse the code in modules/prefabs.lua to make the hierachy box. but instead the top "entity is an scene". if needed make the function inside modules/prefabs.lua to be more generic so that it can be used for scene and state. like the function Drawers["Camera2D"] should be moved to a more generic place so that it can be used in the inspector box for the scene and state.
-- TODO: Implement the bottom right box. reuse the cide in modules/prefabs.lua to make the inspector box. to  configure the "scenes" root entity and the state. if needed make the function inside modules/prefabs.lua to be more generic so that it can be used for scene and state. like the function Drawers["Camera2D"] should be moved to a more generic place so that it can be used in the inspector box for the scene and state.
-- TODO: Refactor scene c++ code so that it can really pass and en entity. like it should be created as en entity. i thing his children is store with hirachy name when it suppose to be write as children like other prfabs entity i thing think like that. I want you to make this refactor first, then make/update unitest. test it. commit then make the editor lua code.
-- TODO: basicly an scene should be an normal like an normal prefabeble entity with but with the scene tags.

function Scenes.on_enter() end
function Scenes.on_update() end
function Scenes.on_exit() end

_G.modules["Prefabs"] = _G.modules["Prefabs"] or { Drawers = {} }
local Drawers = _G.modules["Prefabs"].Drawers

Drawers["Camera2D"] = function(props, path)
	local p = path .. "/components/Camera2D"
	local modified = false

	local z = props:get_double(p .. "/zoom") or 1.0
	local zbuf = ffi.new("float[1]", z)
	if imgui.InputFloat("zoom", zbuf) then
		props:set_double(p .. "/zoom", zbuf[0])
		modified = true
	end

	local active = props:get_bool(p .. "/is_active") or false
	local abuf = ffi.new("bool[1]", active)
	if imgui.Checkbox("is_active", abuf) then
		props:set_bool(p .. "/is_active", abuf[0])
		modified = true
	end

	return modified
end

Drawers["Scene"] = function(props, path)
	return false
end
Drawers["State"] = function(props, path)
	return false
end
Drawers["StateUseSceneRelation"] = function(props, path)
	return false
end
Drawers["StateContext"] = function(props, path)
	return false
end
Drawers["SceneContext"] = function(props, path)
	return false
end
Drawers["DisableRendering"] = function(props, path)
	return false
end

return {}
