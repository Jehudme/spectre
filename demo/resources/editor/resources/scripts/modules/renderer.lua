local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local ffi = require("ffi")
local world = ecs.from_ptr(g_world)

Renderer = {}
_G.modules = _G.modules or {}
_G.modules["Renderer"] = Renderer

local config_props = nil
local config_path = "project://configs/renderer.json"
local bg_color = ffi.new("float[4]", 245.0/255.0, 245.0/255.0, 245.0/255.0, 1.0)

local function save_configuration()
	if config_props then
		config_props:set_double("background_color/r", bg_color[0] * 255.0)
		config_props:set_double("background_color/g", bg_color[1] * 255.0)
		config_props:set_double("background_color/b", bg_color[2] * 255.0)
		config_props:set_double("background_color/a", bg_color[3] * 255.0)

		local dumped = config_props:dump(0)
		if dumped then
			local c_str = ffi.cast("const void*", dumped)
			sandbox.filesystem.write_all_bytes(world, config_path, c_str, #dumped)
		end
	end
end

local function load_configuration()
	if config_props then
		config_props:destroy()
	end
	config_props = sandbox.Properties.new()
	
	if not sandbox.filesystem.exists(world, config_path) then
		save_configuration()
	else
		local out_data = ffi.new("uint8_t*[1]")
		local out_size = ffi.new("size_t[1]")
		if sandbox.filesystem.read_all_bytes(world, config_path, out_data, out_size) then
			if tonumber(out_size[0]) > 0 and out_data[0] ~= nil then
				local content = ffi.string(out_data[0], tonumber(out_size[0]))
				config_props:load(content, 0)
				sandbox.filesystem.free_bytes(world, out_data[0])

				bg_color[0] = (config_props:get_double("background_color/r") or 245.0) / 255.0
				bg_color[1] = (config_props:get_double("background_color/g") or 245.0) / 255.0
				bg_color[2] = (config_props:get_double("background_color/b") or 245.0) / 255.0
				bg_color[3] = (config_props:get_double("background_color/a") or 255.0) / 255.0
			end
		end
	end
end

function Renderer.on_enter()
	load_configuration()
end

function Renderer.on_update()
	local screen_w = spectre.window.get_width(world)
	local screen_h = spectre.window.get_height(world)
	
	imgui.SetNextWindowPos(ffi.new("ImVec2", 0, 20), 1)
	imgui.SetNextWindowSize(ffi.new("ImVec2", screen_w, screen_h - 20), 1)
	
	local window_flags = bit.bor(1, 32, 2, 4, 8192, 524288)
	imgui.Begin("Renderer Configuration##Main", nil, window_flags)
	
	imgui.Text("Renderer Settings")
	imgui.Separator()

	if imgui.ColorEdit4("Background Color", bg_color) then
		save_configuration()
	end

	imgui.End()
end

function Renderer.on_exit()
	if config_props then
		config_props:destroy()
		config_props = nil
	end
end

_G.modules["Prefabs"] = _G.modules["Prefabs"] or { Drawers = {} }
local Drawers = _G.modules["Prefabs"].Drawers

Drawers["Transform2D"] = function(props, path)
	local p = path .. "/components/Transform2D"
	local modified = false

	-- We can use InputFloat3 for position, InputFloat2 for scale and origin.
	local px = props:get_double(p .. "/position_x") or 0.0
	local py = props:get_double(p .. "/position_y") or 0.0
	local pz = props:get_double(p .. "/position_z") or 0.0
	local pbuf = ffi.new("float[3]", px, py, pz)
	if imgui.InputFloat3("position", pbuf) then
		props:set_double(p .. "/position_x", pbuf[0])
		props:set_double(p .. "/position_y", pbuf[1])
		props:set_double(p .. "/position_z", pbuf[2])
		modified = true
	end

	local sx = props:get_double(p .. "/scale_x") or 1.0
	local sy = props:get_double(p .. "/scale_y") or 1.0
	local sbuf = ffi.new("float[2]", sx, sy)
	if imgui.InputFloat2("scale", sbuf) then
		props:set_double(p .. "/scale_x", sbuf[0])
		props:set_double(p .. "/scale_y", sbuf[1])
		modified = true
	end

	local ox = props:get_double(p .. "/origin_x") or 0.0
	local oy = props:get_double(p .. "/origin_y") or 0.0
	local obuf = ffi.new("float[2]", ox, oy)
	if imgui.InputFloat2("origin", obuf) then
		props:set_double(p .. "/origin_x", obuf[0])
		props:set_double(p .. "/origin_y", obuf[1])
		modified = true
	end

	local rot = props:get_double(p .. "/rotation") or 0.0
	local rbuf = ffi.new("float[1]", rot)
	if imgui.InputFloat("rotation", rbuf) then
		props:set_double(p .. "/rotation", rbuf[0])
		modified = true
	end

	-- Cleanup legacy fields if they exist
	if props:has(p .. "/position/x") then
		props:clear(p .. "/position")
		modified = true
	end
	if props:has(p .. "/scale/x") then
		props:clear(p .. "/scale")
		modified = true
	end

	return modified
end

Drawers["TextureRenderable"] = function(props, path)
	local p = path .. "/components/TextureRenderable"
	local modified = false

	local name = props:read_string(p .. "/name") or ""
	local textures = _G.modules["Resources"].get_resources_by_type("texture")

	-- Insert an empty option or find current index
	local current_idx = 0
	local c_textures = ffi.new("const char*[?]", #textures + 1)
	c_textures[0] = "None"
	for i, tex in ipairs(textures) do
		c_textures[i] = tex
		if tex == name then
			current_idx = i
		end
	end

	local idx_buf = ffi.new("int[1]", current_idx)
	if imgui.Combo("Texture##TexRen", idx_buf, c_textures, #textures + 1) then
		local new_name = idx_buf[0] > 0 and textures[idx_buf[0]] or ""
		props:set_string(p .. "/name", new_name)
		modified = true
	end

	local w = props:get_double(p .. "/width") or 0.0
	local h = props:get_double(p .. "/height") or 0.0
	local size_buf = ffi.new("float[2]", w, h)
	if imgui.InputFloat2("size##TexRen", size_buf) then
		props:set_double(p .. "/width", size_buf[0])
		props:set_double(p .. "/height", size_buf[1])
		modified = true
	end

	local sx = props:get_double(p .. "/source_x") or 0.0
	local sy = props:get_double(p .. "/source_y") or 0.0
	local src_pos = ffi.new("float[2]", sx, sy)
	if imgui.InputFloat2("source pos##TexRen", src_pos) then
		props:set_double(p .. "/source_x", src_pos[0])
		props:set_double(p .. "/source_y", src_pos[1])
		modified = true
	end

	local sw = props:get_double(p .. "/source_width") or 0.0
	local sh = props:get_double(p .. "/source_height") or 0.0
	local src_size = ffi.new("float[2]", sw, sh)
	if imgui.InputFloat2("source size##TexRen", src_size) then
		props:set_double(p .. "/source_width", src_size[0])
		props:set_double(p .. "/source_height", src_size[1])
		modified = true
	end

	local fx = props:read_string(p .. "/flip_x") == "true"
	local fy = props:read_string(p .. "/flip_y") == "true"
	local f_buf = ffi.new("bool[2]", fx, fy)
	if imgui.Checkbox("flip x##TexRen", f_buf) then
		props:set_string(p .. "/flip_x", f_buf[0] and "true" or "false")
		modified = true
	end
	imgui.SameLine()
	if imgui.Checkbox("flip y##TexRen", ffi.cast("bool*", ffi.cast("char*", f_buf) + 1)) then
		props:set_string(p .. "/flip_y", f_buf[1] and "true" or "false")
		modified = true
	end

	local tr = props:get_double(p .. "/tint/r") or 255.0
	local tg = props:get_double(p .. "/tint/g") or 255.0
	local tb = props:get_double(p .. "/tint/b") or 255.0
	local ta = props:get_double(p .. "/tint/a") or 255.0
	local tbuf = ffi.new("float[4]", tr / 255.0, tg / 255.0, tb / 255.0, ta / 255.0)
	if imgui.ColorEdit4("tint##TexRen", tbuf) then
		props:set_double(p .. "/tint/r", tbuf[0] * 255.0)
		props:set_double(p .. "/tint/g", tbuf[1] * 255.0)
		props:set_double(p .. "/tint/b", tbuf[2] * 255.0)
		props:set_double(p .. "/tint/a", tbuf[3] * 255.0)
		modified = true
	end

	return modified
end

Drawers["TextRenderable"] = function(props, path)
	local p = path .. "/components/TextRenderable"
	local modified = false

	local name = props:read_string(p .. "/name") or ""
	local fonts = _G.modules["Resources"].get_resources_by_type("font")

	local current_idx = 0
	local c_fonts = ffi.new("const char*[?]", #fonts + 1)
	c_fonts[0] = "None"
	for i, font in ipairs(fonts) do
		c_fonts[i] = font
		if font == name then
			current_idx = i
		end
	end

	local idx_buf = ffi.new("int[1]", current_idx)
	if imgui.Combo("Font##TxtRen", idx_buf, c_fonts, #fonts + 1) then
		local new_name = idx_buf[0] > 0 and fonts[idx_buf[0]] or ""
		props:set_string(p .. "/name", new_name)
		modified = true
	end

	local txt = props:read_string(p .. "/content") or ""
	local buf = ffi.new("char[2048]")
	ffi.copy(buf, txt)
	if imgui.InputTextMultiline("content##TxtRen", buf, 2048, ffi.new("ImVec2", 0, 50)) then
		props:set_string(p .. "/content", ffi.string(buf))
		modified = true
	end

	local fs = props:get_double(p .. "/font_size") or 20.0
	local fs_buf = ffi.new("float[1]", fs)
	if imgui.InputFloat("font size##TxtRen", fs_buf) then
		props:set_double(p .. "/font_size", fs_buf[0])
		modified = true
	end

	local sp = props:get_double(p .. "/spacing") or 1.0
	local sp_buf = ffi.new("float[1]", sp)
	if imgui.InputFloat("spacing##TxtRen", sp_buf) then
		props:set_double(p .. "/spacing", sp_buf[0])
		modified = true
	end

	local tr = props:get_double(p .. "/tint/r") or 255.0
	local tg = props:get_double(p .. "/tint/g") or 255.0
	local tb = props:get_double(p .. "/tint/b") or 255.0
	local ta = props:get_double(p .. "/tint/a") or 255.0
	local tbuf = ffi.new("float[4]", tr / 255.0, tg / 255.0, tb / 255.0, ta / 255.0)
	if imgui.ColorEdit4("tint##TxtRen", tbuf) then
		props:set_double(p .. "/tint/r", tbuf[0] * 255.0)
		props:set_double(p .. "/tint/g", tbuf[1] * 255.0)
		props:set_double(p .. "/tint/b", tbuf[2] * 255.0)
		props:set_double(p .. "/tint/a", tbuf[3] * 255.0)
		modified = true
	end

	local b = props:read_string(p .. "/bold") == "true"
	local i = props:read_string(p .. "/italic") == "true"
	local b_buf = ffi.new("bool[2]", b, i)
	if imgui.Checkbox("bold##TxtRen", b_buf) then
		props:set_string(p .. "/bold", b_buf[0] and "true" or "false")
		modified = true
	end
	imgui.SameLine()
	if imgui.Checkbox("italic##TxtRen", ffi.cast("bool*", ffi.cast("char*", b_buf) + 1)) then
		props:set_string(p .. "/italic", b_buf[1] and "true" or "false")
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

local function draw_color(props, base_path, name)
	local col_r = props:get_double(base_path .. "/" .. name .. "/r") or 1.0
	local col_g = props:get_double(base_path .. "/" .. name .. "/g") or 1.0
	local col_b = props:get_double(base_path .. "/" .. name .. "/b") or 1.0
	local col_a = props:get_double(base_path .. "/" .. name .. "/a") or 1.0
	local cbuf = ffi.new("float[4]", col_r, col_g, col_b, col_a)
	if imgui.ColorEdit4(name, cbuf) then
		props:set_double(base_path .. "/" .. name .. "/r", cbuf[0])
		props:set_double(base_path .. "/" .. name .. "/g", cbuf[1])
		props:set_double(base_path .. "/" .. name .. "/b", cbuf[2])
		props:set_double(base_path .. "/" .. name .. "/a", cbuf[3])
		return true
	end
	return false
end

Drawers["RectangleRenderable"] = function(props, path)
	local p = path .. "/components/RectangleRenderable"
	local modified = false

	local w = props:get_double(p .. "/width") or 10.0
	local h = props:get_double(p .. "/height") or 10.0
	local sbuf = ffi.new("float[2]", w, h)
	if imgui.InputFloat2("size", sbuf) then
		props:set_double(p .. "/width", sbuf[0])
		props:set_double(p .. "/height", sbuf[1])
		modified = true
	end

	if draw_color(props, p, "fill_color") then
		modified = true
	end
	if draw_color(props, p, "outline_color") then
		modified = true
	end

	local thick = props:get_double(p .. "/outline_thickness") or 0.0
	local tbuf = ffi.new("float[1]", thick)
	if imgui.InputFloat("outline_thickness", tbuf) then
		props:set_double(p .. "/outline_thickness", tbuf[0])
		modified = true
	end

	return modified
end

Drawers["CircleRenderable"] = function(props, path)
	local p = path .. "/components/CircleRenderable"
	local modified = false

	local r = props:get_double(p .. "/radius") or 10.0
	local rbuf = ffi.new("float[1]", r)
	if imgui.InputFloat("radius", rbuf) then
		props:set_double(p .. "/radius", rbuf[0])
		modified = true
	end

	if draw_color(props, p, "fill_color") then
		modified = true
	end
	if draw_color(props, p, "outline_color") then
		modified = true
	end

	local thick = props:get_double(p .. "/outline_thickness") or 0.0
	local tbuf = ffi.new("float[1]", thick)
	if imgui.InputFloat("outline_thickness", tbuf) then
		props:set_double(p .. "/outline_thickness", tbuf[0])
		modified = true
	end

	return modified
end

Drawers["PolygoneRenderable"] = function(props, path)
	local p = path .. "/components/PolygoneRenderable"
	local modified = false

	local r = props:get_double(p .. "/radius") or 10.0
	local rbuf = ffi.new("float[1]", r)
	if imgui.InputFloat("radius", rbuf) then
		props:set_double(p .. "/radius", rbuf[0])
		modified = true
	end

	local pt = props:get_int64(p .. "/point_count") or 3
	local ptbuf = ffi.new("int[1]", pt)
	if imgui.InputInt("point_count", ptbuf) then
		props:set_int64(p .. "/point_count", ptbuf[0])
		modified = true
	end

	if draw_color(props, p, "fill_color") then
		modified = true
	end
	if draw_color(props, p, "outline_color") then
		modified = true
	end

	local thick = props:get_double(p .. "/outline_thickness") or 0.0
	local tbuf = ffi.new("float[1]", thick)
	if imgui.InputFloat("outline_thickness", tbuf) then
		props:set_double(p .. "/outline_thickness", tbuf[0])
		modified = true
	end

	return modified
end

Drawers["LigneRenderable"] = function(props, path)
	local p = path .. "/components/LigneRenderable"
	local modified = false

	local x1 = props:get_double(p .. "/position_x1") or 0.0
	local y1 = props:get_double(p .. "/position_y1") or 0.0
	local p1buf = ffi.new("float[2]", x1, y1)
	if imgui.InputFloat2("point 1", p1buf) then
		props:set_double(p .. "/position_x1", p1buf[0])
		props:set_double(p .. "/position_y1", p1buf[1])
		modified = true
	end

	local x2 = props:get_double(p .. "/position_x2") or 10.0
	local y2 = props:get_double(p .. "/position_y2") or 10.0
	local p2buf = ffi.new("float[2]", x2, y2)
	if imgui.InputFloat2("point 2", p2buf) then
		props:set_double(p .. "/position_x2", p2buf[0])
		props:set_double(p .. "/position_y2", p2buf[1])
		modified = true
	end

	if draw_color(props, p, "color") then
		modified = true
	end

	local thick = props:get_double(p .. "/thickness") or 1.0
	local tbuf = ffi.new("float[1]", thick)
	if imgui.InputFloat("thickness", tbuf) then
		props:set_double(p .. "/thickness", tbuf[0])
		modified = true
	end

	return modified
end

Drawers["Renderable"] = function(props, path)
	return false
end
Drawers["CustomPolygoneRenderable"] = function(props, path)
	return false
end

return {}
