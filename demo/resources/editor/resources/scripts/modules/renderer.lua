local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local world = ecs.from_ptr(g_world)

Renderer = {}
_G.modules = _G.modules or {}
_G.modules["Renderer"] = Renderer

function Renderer.on_enter() end
function Renderer.on_update() end
function Renderer.on_exit() end

return {}
