local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local world = ecs.from_ptr(g_world)

Scenes = {}
_G.modules = _G.modules or {}
_G.modules["Scenes"] = Scenes

function Scenes.on_enter() end
function Scenes.on_update() end
function Scenes.on_exit() end

return {}
