local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local world = ecs.from_ptr(g_world)

Components = {}
_G.modules = _G.modules or {}
_G.modules["Components"] = Components

function Components.on_enter() end
function Components.on_update() end
function Components.on_exit() end

return {}
