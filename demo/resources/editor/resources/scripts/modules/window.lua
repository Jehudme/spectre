local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local world = ecs.from_ptr(g_world)

Window = {}
_G.modules = _G.modules or {}
_G.modules["Window"] = Window

function Window.on_enter() end
function Window.on_update() end
function Window.on_exit() end

return {}
