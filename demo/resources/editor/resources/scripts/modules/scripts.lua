local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local scripts = require("spectre").scripts
local world = ecs.from_ptr(g_world)

Scripts = {}
_G.modules = _G.modules or {}
_G.modules["Scripts"] = Scripts

function Scripts.on_enter() end
function Scripts.on_update() end
function Scripts.on_exit() end

_G.modules["Prefabs"] = _G.modules["Prefabs"] or { Drawers = {} }
local Drawers = _G.modules["Prefabs"].Drawers

Drawers["Script"] = function(props, path)
    local p = path .. "/components/Script"
    local modified = false
    local module = props:read_string(p .. "/module") or ""
    local buf = ffi.new("char[256]")
    ffi.copy(buf, module)
    if imgui.InputText("module", buf, 256) then
        props:set_string(p .. "/module", ffi.string(buf))
        modified = true
    end
    return modified
end

return {}
