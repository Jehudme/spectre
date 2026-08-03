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

return {}
