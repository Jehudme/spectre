local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local world = ecs.from_ptr(g_world)

Prefabs = {}
_G.modules = _G.modules or {}
_G.modules["Prefabs"] = Prefabs

function Prefabs.on_enter() end
function Prefabs.on_update() end
function Prefabs.on_exit() end

return {}
