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

_G.modules["Prefabs"] = _G.modules["Prefabs"] or { Drawers = {} }
local Drawers = _G.modules["Prefabs"].Drawers

Drawers["Transform2D"] = function(props, path)
    local p = path .. "/components/Transform2D"
    local modified = false
    
    local px = props:get_double(p .. "/position/x") or 0.0
    local py = props:get_double(p .. "/position/y") or 0.0
    local pbuf = ffi.new("float[2]", px, py)
    if imgui.InputFloat2("position", pbuf) then
        props:set_double(p .. "/position/x", pbuf[0])
        props:set_double(p .. "/position/y", pbuf[1])
        modified = true
    end

    local sx = props:get_double(p .. "/scale/x") or 1.0
    local sy = props:get_double(p .. "/scale/y") or 1.0
    local sbuf = ffi.new("float[2]", sx, sy)
    if imgui.InputFloat2("scale", sbuf) then
        props:set_double(p .. "/scale/x", sbuf[0])
        props:set_double(p .. "/scale/y", sbuf[1])
        modified = true
    end

    local rot = props:get_double(p .. "/rotation") or 0.0
    local rbuf = ffi.new("float[1]", rot)
    if imgui.InputFloat("rotation", rbuf) then
        props:set_double(p .. "/rotation", rbuf[0])
        modified = true
    end
    
    return modified
end

Drawers["TextureRenderable"] = function(props, path)
    local p = path .. "/components/TextureRenderable"
    local modified = false
    local tex = props:read_string(p .. "/texture_path") or ""
    local buf = ffi.new("char[256]")
    ffi.copy(buf, tex)
    if imgui.InputText("texture_path", buf, 256) then
        props:set_string(p .. "/texture_path", ffi.string(buf))
        modified = true
    end
    return modified
end

Drawers["TextRenderable"] = function(props, path)
    local p = path .. "/components/TextRenderable"
    local modified = false
    local txt = props:read_string(p .. "/text") or ""
    local buf = ffi.new("char[256]")
    ffi.copy(buf, txt)
    if imgui.InputText("text", buf, 256) then
        props:set_string(p .. "/text", ffi.string(buf))
        modified = true
    end
    local font = props:read_string(p .. "/font_path") or ""
    local fbuf = ffi.new("char[256]")
    ffi.copy(fbuf, font)
    if imgui.InputText("font_path", fbuf, 256) then
        props:set_string(p .. "/font_path", ffi.string(fbuf))
        modified = true
    end
    return modified
end

Drawers["Material"] = function(props, path)
    local p = path .. "/components/Material"
    local modified = false
    local col_r = props:get_double(p .. "/color/r") or 1.0
    local col_g = props:get_double(p .. "/color/g") or 1.0
    local col_b = props:get_double(p .. "/color/b") or 1.0
    local col_a = props:get_double(p .. "/color/a") or 1.0
    local cbuf = ffi.new("float[4]", col_r, col_g, col_b, col_a)
    if imgui.ColorEdit4("color", cbuf) then
        props:set_double(p .. "/color/r", cbuf[0])
        props:set_double(p .. "/color/g", cbuf[1])
        props:set_double(p .. "/color/b", cbuf[2])
        props:set_double(p .. "/color/a", cbuf[3])
        modified = true
    end
    return modified
end

return {}
