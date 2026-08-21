local sandbox = require("sandbox")
local ecs = require("ecs")
local imgui = require("imgui")
local ffi = require("ffi")

local _g_draw_int = ffi.new("int[1]")
local _g_draw_float = ffi.new("float[1]")
local _g_draw_float2 = ffi.new("float[2]")
local _g_draw_float3 = ffi.new("float[3]")
local _g_draw_float4 = ffi.new("float[4]")
local _g_draw_bool = ffi.new("bool[1]")
local _g_draw_bool2 = ffi.new("bool[2]")
local _g_draw_char = ffi.new("char[2048]")
local _g_draw_combo = ffi.new("const char*[2048]")
local spectre = require("spectre")
local resources_module = require("interface.pages.modules.resources")

local history = require("utilities.history")
local pages = require("utilities.pages")
local search = require("utilities.search")
require("utilities.actions.write_file")

local renderer_page = Page.new()

local config_props = nil
local config_path = "project://configs/renderer.json"
_g_draw_float4[0] = 245.0 / 255.0; _g_draw_float4[1] = 245.0 / 255.0; _g_draw_float4[2] = 245.0 / 255.0; _g_draw_float4[3] = 1.0
	local bg_color = _g_draw_float4

-- ==========================================
-- Inline Functions
-- ==========================================

local function reset_to_defaults()
	bg_color[0] = 245.0 / 255.0
	bg_color[1] = 245.0 / 255.0
	bg_color[2] = 245.0 / 255.0
	bg_color[3] = 1.0
end

local function load_configuration_from_string(world, content)
	if config_props then
		config_props:destroy()
	end
	config_props = sandbox.Properties.new()
	config_props:load(content, 0)

	bg_color[0] = (config_props:get_double("background_color/r") or 245.0) / 255.0
	bg_color[1] = (config_props:get_double("background_color/g") or 245.0) / 255.0
	bg_color[2] = (config_props:get_double("background_color/b") or 245.0) / 255.0
	bg_color[3] = (config_props:get_double("background_color/a") or 255.0) / 255.0
end

local function load_configuration(world)
	if not sandbox.filesystem.exists(world, config_path) then
		reset_to_defaults()
	else
		local out_data = ffi.new("uint8_t*[1]")
		local out_size = ffi.new("size_t[1]")
		if sandbox.filesystem.read_all_bytes(world, config_path, out_data, out_size) then
			if tonumber(out_size[0]) > 0 and out_data[0] ~= nil then
				local content = ffi.string(out_data[0], tonumber(out_size[0]))
				load_configuration_from_string(world, content)
			end
			if out_data[0] ~= nil then
				sandbox.filesystem.free_bytes(world, out_data[0])
			end
		end
	end
end

local function draw_color(props, base_path, name)
	local col_r = (props:get_double(base_path .. "/" .. name .. "/r") or 255.0) / 255.0
	local col_g = (props:get_double(base_path .. "/" .. name .. "/g") or 255.0) / 255.0
	local col_b = (props:get_double(base_path .. "/" .. name .. "/b") or 255.0) / 255.0
	local col_a = (props:get_double(base_path .. "/" .. name .. "/a") or 255.0) / 255.0
	_g_draw_float4[0] = col_r; _g_draw_float4[1] = col_g; _g_draw_float4[2] = col_b; _g_draw_float4[3] = col_a
	local cbuf = _g_draw_float4
	if imgui.ColorEdit4(name, cbuf) then
		props:set_double(base_path .. "/" .. name .. "/r", cbuf[0] * 255.0)
		props:set_double(base_path .. "/" .. name .. "/g", cbuf[1] * 255.0)
		props:set_double(base_path .. "/" .. name .. "/b", cbuf[2] * 255.0)
		props:set_double(base_path .. "/" .. name .. "/a", cbuf[3] * 255.0)
		return true
	end
	return false
end

-- ==========================================
-- Action Functions
-- ==========================================

local function apply_renderer_settings(world)
	local temp_props = sandbox.Properties.new()
	temp_props:set_double("background_color/r", bg_color[0] * 255.0)
	temp_props:set_double("background_color/g", bg_color[1] * 255.0)
	temp_props:set_double("background_color/b", bg_color[2] * 255.0)
	temp_props:set_double("background_color/a", bg_color[3] * 255.0)

	local new_dumped = temp_props:dump(0)
	temp_props:destroy()

	local old_dumped = nil
	if sandbox.filesystem.exists(world, config_path) then
		old_dumped = sandbox.filesystem.read_file_string(world, config_path)
	end
	
	local current_path = config_path

	local redo_fn = function()
		local w = ecs.from_ptr(g_world)
		local parent = current_path:match("(.*)/[^/]+$")
		if parent and not sandbox.filesystem.exists(w, parent) then
			sandbox.filesystem.create_directory(w, parent, true)
		end
		sandbox.filesystem.write_file_string(w, current_path, new_dumped)
		load_configuration_from_string(w, new_dumped)
	end

	local undo_fn = function()
		local w = ecs.from_ptr(g_world)
		if old_dumped then
			sandbox.filesystem.write_file_string(w, current_path, old_dumped)
			load_configuration_from_string(w, old_dumped)
		else
			sandbox.filesystem.remove_file(w, current_path)
			reset_to_defaults()
		end
	end

	local action = _G.Action.new(redo_fn, undo_fn, true, "Update Renderer Settings")
	history.execute(action)
end

-- ==========================================
-- Page Code
-- ==========================================

function renderer_page:on_enter()
	local world = ecs.from_ptr(g_world)
	load_configuration(world)
end

function renderer_page:on_render()
	local world = ecs.from_ptr(g_world)
	imgui.Text("Renderer Settings")
	imgui.Separator()

	if imgui.ColorEdit4("Background Color", bg_color) then
		apply_renderer_settings(world)
	end
end

function renderer_page:on_exit()
	if config_props then
		config_props:destroy()
		config_props = nil
	end
end

-- Transform2D Drawer
local transform2d_drawer = Page.new()
function transform2d_drawer:on_render(props, path)
	local p = path .. "/components/Transform2D"
	local modified = false

	local px = props:get_double(p .. "/position_x") or 0.0
	local py = props:get_double(p .. "/position_y") or 0.0
	local pz = props:get_double(p .. "/position_z") or 0.0
	_g_draw_float3[0] = px; _g_draw_float3[1] = py; _g_draw_float3[2] = pz
	local pbuf = _g_draw_float3
	if imgui.InputFloat3("position", pbuf) then
		props:set_double(p .. "/position_x", pbuf[0])
		props:set_double(p .. "/position_y", pbuf[1])
		props:set_double(p .. "/position_z", pbuf[2])
		modified = true
	end

	local sx = props:get_double(p .. "/scale_x") or 1.0
	local sy = props:get_double(p .. "/scale_y") or 1.0
	_g_draw_float2[0] = sx; _g_draw_float2[1] = sy
	local sbuf = _g_draw_float2
	if imgui.InputFloat2("scale", sbuf) then
		props:set_double(p .. "/scale_x", sbuf[0])
		props:set_double(p .. "/scale_y", sbuf[1])
		modified = true
	end

	local ox = props:get_double(p .. "/origin_x") or 0.0
	local oy = props:get_double(p .. "/origin_y") or 0.0
	_g_draw_float2[0] = ox; _g_draw_float2[1] = oy
	local obuf = _g_draw_float2
	if imgui.InputFloat2("origin", obuf) then
		props:set_double(p .. "/origin_x", obuf[0])
		props:set_double(p .. "/origin_y", obuf[1])
		modified = true
	end

	local rot = props:get_double(p .. "/rotation") or 0.0
	_g_draw_float[0] = rot
	local rbuf = _g_draw_float
	if imgui.InputFloat("rotation", rbuf) then
		props:set_double(p .. "/rotation", rbuf[0])
		modified = true
	end

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
pages.register("drawer", "Transform2D", transform2d_drawer)

-- TextureRenderable Drawer
local texture_renderable_drawer = Page.new()
function texture_renderable_drawer:on_render(props, path)
	local p = path .. "/components/TextureRenderable"
	local modified = false

	local name = props:read_string(p .. "/name") or ""
	local textures = resources_module.get_resources_by_type("texture")

	local current_idx = 0
	local c_textures = _g_draw_combo
	c_textures[0] = "None"
	for i, tex in ipairs(textures) do
		c_textures[i] = tex
		if tex == name then
			current_idx = i
		end
	end

	_g_draw_int[0] = current_idx
	local idx_buf = _g_draw_int
	if imgui.Combo("Texture##TexRen", idx_buf, c_textures, #textures + 1) then
		local new_name = idx_buf[0] > 0 and textures[idx_buf[0]] or ""
		props:set_string(p .. "/name", new_name)
		modified = true
	end

	local w = props:get_double(p .. "/width") or 0.0
	local h = props:get_double(p .. "/height") or 0.0
	_g_draw_float2[0] = w; _g_draw_float2[1] = h
	local size_buf = _g_draw_float2
	if imgui.InputFloat2("size##TexRen", size_buf) then
		props:set_double(p .. "/width", size_buf[0])
		props:set_double(p .. "/height", size_buf[1])
		modified = true
	end

	local sx = props:get_double(p .. "/source_x") or 0.0
	local sy = props:get_double(p .. "/source_y") or 0.0
	_g_draw_float2[0] = sx; _g_draw_float2[1] = sy
	local src_pos = _g_draw_float2
	if imgui.InputFloat2("source pos##TexRen", src_pos) then
		props:set_double(p .. "/source_x", src_pos[0])
		props:set_double(p .. "/source_y", src_pos[1])
		modified = true
	end

	local sw = props:get_double(p .. "/source_width") or 0.0
	local sh = props:get_double(p .. "/source_height") or 0.0
	_g_draw_float2[0] = sw; _g_draw_float2[1] = sh
	local src_size = _g_draw_float2
	if imgui.InputFloat2("source size##TexRen", src_size) then
		props:set_double(p .. "/source_width", src_size[0])
		props:set_double(p .. "/source_height", src_size[1])
		modified = true
	end

	local fx = props:read_string(p .. "/flip_x") == "true"
	local fy = props:read_string(p .. "/flip_y") == "true"
	_g_draw_bool2[0] = fx; _g_draw_bool2[1] = fy
	local f_buf = _g_draw_bool2
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
	_g_draw_float4[0] = tr / 255.0; _g_draw_float4[1] = tg / 255.0; _g_draw_float4[2] = tb / 255.0; _g_draw_float4[3] = ta / 255.0
	local tbuf = _g_draw_float4
	if imgui.ColorEdit4("tint##TexRen", tbuf) then
		props:set_double(p .. "/tint/r", tbuf[0] * 255.0)
		props:set_double(p .. "/tint/g", tbuf[1] * 255.0)
		props:set_double(p .. "/tint/b", tbuf[2] * 255.0)
		props:set_double(p .. "/tint/a", tbuf[3] * 255.0)
		modified = true
	end

	return modified
end
pages.register("drawer", "TextureRenderable", texture_renderable_drawer)

-- TextRenderable Drawer
local text_renderable_drawer = Page.new()
function text_renderable_drawer:on_render(props, path)
	local p = path .. "/components/TextRenderable"
	local modified = false

	local name = props:read_string(p .. "/name") or ""
	local fonts = resources_module.get_resources_by_type("font")

	local current_idx = 0
	local c_fonts = _g_draw_combo
	c_fonts[0] = "None"
	for i, font in ipairs(fonts) do
		c_fonts[i] = font
		if font == name then
			current_idx = i
		end
	end

	_g_draw_int[0] = current_idx
	local idx_buf = _g_draw_int
	if imgui.Combo("Font##TxtRen", idx_buf, c_fonts, #fonts + 1) then
		local new_name = idx_buf[0] > 0 and fonts[idx_buf[0]] or ""
		props:set_string(p .. "/name", new_name)
		modified = true
	end

	local txt = props:read_string(p .. "/content") or ""
	local buf = _g_draw_char
	ffi.copy(buf, txt)
	if imgui.InputTextMultiline("content##TxtRen", buf, 2048, ffi.new("ImVec2", 0, 50)) then
		props:set_string(p .. "/content", ffi.string(buf))
		modified = true
	end

	local fs = props:get_double(p .. "/font_size") or 20.0
	_g_draw_float[0] = fs
	local fs_buf = _g_draw_float
	if imgui.InputFloat("font size##TxtRen", fs_buf) then
		props:set_double(p .. "/font_size", fs_buf[0])
		modified = true
	end

	local sp = props:get_double(p .. "/spacing") or 1.0
	_g_draw_float[0] = sp
	local sp_buf = _g_draw_float
	if imgui.InputFloat("spacing##TxtRen", sp_buf) then
		props:set_double(p .. "/spacing", sp_buf[0])
		modified = true
	end

	local tr = props:get_double(p .. "/tint/r") or 255.0
	local tg = props:get_double(p .. "/tint/g") or 255.0
	local tb = props:get_double(p .. "/tint/b") or 255.0
	local ta = props:get_double(p .. "/tint/a") or 255.0
	_g_draw_float4[0] = tr / 255.0; _g_draw_float4[1] = tg / 255.0; _g_draw_float4[2] = tb / 255.0; _g_draw_float4[3] = ta / 255.0
	local tbuf = _g_draw_float4
	if imgui.ColorEdit4("tint##TxtRen", tbuf) then
		props:set_double(p .. "/tint/r", tbuf[0] * 255.0)
		props:set_double(p .. "/tint/g", tbuf[1] * 255.0)
		props:set_double(p .. "/tint/b", tbuf[2] * 255.0)
		props:set_double(p .. "/tint/a", tbuf[3] * 255.0)
		modified = true
	end

	local b = props:read_string(p .. "/bold") == "true"
	local i = props:read_string(p .. "/italic") == "true"
	_g_draw_bool2[0] = b; _g_draw_bool2[1] = i
	local b_buf = _g_draw_bool2
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
pages.register("drawer", "TextRenderable", text_renderable_drawer)

-- Material Drawer
local material_drawer = Page.new()
function material_drawer:on_render(props, path)
	local p = path .. "/components/Material"
	local modified = false
	local col_r = props:get_double(p .. "/color/r") or 1.0
	local col_g = props:get_double(p .. "/color/g") or 1.0
	local col_b = props:get_double(p .. "/color/b") or 1.0
	local col_a = props:get_double(p .. "/color/a") or 1.0
	_g_draw_float4[0] = col_r; _g_draw_float4[1] = col_g; _g_draw_float4[2] = col_b; _g_draw_float4[3] = col_a
	local cbuf = _g_draw_float4
	if imgui.ColorEdit4("color", cbuf) then
		props:set_double(p .. "/color/r", cbuf[0])
		props:set_double(p .. "/color/g", cbuf[1])
		props:set_double(p .. "/color/b", cbuf[2])
		props:set_double(p .. "/color/a", cbuf[3])
		modified = true
	end
	return modified
end
pages.register("drawer", "Material", material_drawer)

-- RectangleRenderable Drawer
local rectangle_renderable_drawer = Page.new()
function rectangle_renderable_drawer:on_render(props, path)
	local p = path .. "/components/RectangleRenderable"
	local modified = false

	local w = props:get_double(p .. "/width") or 10.0
	local h = props:get_double(p .. "/height") or 10.0
	_g_draw_float2[0] = w; _g_draw_float2[1] = h
	local sbuf = _g_draw_float2
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
	_g_draw_float[0] = thick
	local tbuf = _g_draw_float
	if imgui.InputFloat("outline_thickness", tbuf) then
		props:set_double(p .. "/outline_thickness", tbuf[0])
		modified = true
	end

	return modified
end
pages.register("drawer", "RectangleRenderable", rectangle_renderable_drawer)

-- CircleRenderable Drawer
local circle_renderable_drawer = Page.new()
function circle_renderable_drawer:on_render(props, path)
	local p = path .. "/components/CircleRenderable"
	local modified = false

	local r = props:get_double(p .. "/radius") or 10.0
	_g_draw_float[0] = r
	local rbuf = _g_draw_float
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
	_g_draw_float[0] = thick
	local tbuf = _g_draw_float
	if imgui.InputFloat("outline_thickness", tbuf) then
		props:set_double(p .. "/outline_thickness", tbuf[0])
		modified = true
	end

	return modified
end
pages.register("drawer", "CircleRenderable", circle_renderable_drawer)

-- PolygoneRenderable Drawer
local polygone_renderable_drawer = Page.new()
function polygone_renderable_drawer:on_render(props, path)
	local p = path .. "/components/PolygoneRenderable"
	local modified = false

	local r = props:get_double(p .. "/radius") or 10.0
	_g_draw_float[0] = r
	local rbuf = _g_draw_float
	if imgui.InputFloat("radius", rbuf) then
		props:set_double(p .. "/radius", rbuf[0])
		modified = true
	end

	local pt = props:get_int64(p .. "/point_count") or 3
	_g_draw_int[0] = pt
	local ptbuf = _g_draw_int
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
	_g_draw_float[0] = thick
	local tbuf = _g_draw_float
	if imgui.InputFloat("outline_thickness", tbuf) then
		props:set_double(p .. "/outline_thickness", tbuf[0])
		modified = true
	end

	return modified
end
pages.register("drawer", "PolygoneRenderable", polygone_renderable_drawer)

-- LigneRenderable Drawer
local ligne_renderable_drawer = Page.new()
function ligne_renderable_drawer:on_render(props, path)
	local p = path .. "/components/LigneRenderable"
	local modified = false

	local x1 = props:get_double(p .. "/position_x1") or 0.0
	local y1 = props:get_double(p .. "/position_y1") or 0.0
	_g_draw_float2[0] = x1; _g_draw_float2[1] = y1
	local p1buf = _g_draw_float2
	if imgui.InputFloat2("point 1", p1buf) then
		props:set_double(p .. "/position_x1", p1buf[0])
		props:set_double(p .. "/position_y1", p1buf[1])
		modified = true
	end

	local x2 = props:get_double(p .. "/position_x2") or 10.0
	local y2 = props:get_double(p .. "/position_y2") or 10.0
	_g_draw_float2[0] = x2; _g_draw_float2[1] = y2
	local p2buf = _g_draw_float2
	if imgui.InputFloat2("point 2", p2buf) then
		props:set_double(p .. "/position_x2", p2buf[0])
		props:set_double(p .. "/position_y2", p2buf[1])
		modified = true
	end

	if draw_color(props, p, "color") then
		modified = true
	end

	local thick = props:get_double(p .. "/thickness") or 1.0
	_g_draw_float[0] = thick
	local tbuf = _g_draw_float
	if imgui.InputFloat("thickness", tbuf) then
		props:set_double(p .. "/thickness", tbuf[0])
		modified = true
	end

	return modified
end
pages.register("drawer", "LigneRenderable", ligne_renderable_drawer)

-- Renderable Drawer
local renderable_drawer = Page.new()
function renderable_drawer:on_render(props, path)
	return false
end
pages.register("drawer", "Renderable", renderable_drawer)

-- CustomPolygoneRenderable Drawer
local custom_polygone_renderable_drawer = Page.new()
function custom_polygone_renderable_drawer:on_render(props, path)
	return false
end
pages.register("drawer", "CustomPolygoneRenderable", custom_polygone_renderable_drawer)

-- ==========================================
-- Tests
-- ==========================================

function renderer_page.run_test()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[Renderer Page Test] ===== STARTING ALL TESTS =====")

	local orig_path = config_path
	config_path = "save://configs/renderer.json"
	local test_config = config_path
	if not sandbox.filesystem.exists(world, "save://configs") then
		sandbox.filesystem.create_directory(world, "save://configs", true)
	end

	local backup = nil
	if sandbox.filesystem.exists(world, test_config) then
		backup = sandbox.filesystem.read_file_string(world, test_config)
	end

	history.clear()

	bg_color[0] = 0.5
	bg_color[1] = 0.5
	bg_color[2] = 0.5
	bg_color[3] = 1.0
	apply_renderer_settings(world)

	if sandbox.filesystem.exists(world, test_config) then
		local content = sandbox.filesystem.read_file_string(world, test_config)
		if content:match("background_color") then
			sandbox.logs.info(world, "[Renderer Page Test] Passed apply settings")
		else
			sandbox.logs.error(world, "[Renderer Page Test] Failed apply settings")
		end
	else
		sandbox.logs.error(world, "[Renderer Page Test] Failed apply settings (no file)")
	end

	-- Undo
	history.undo()
	if bg_color[0] ~= 0.5 then
		sandbox.logs.info(world, "[Renderer Page Test] Passed undo settings")
	else
		sandbox.logs.error(world, "[Renderer Page Test] Failed undo settings")
	end

	-- Redo
	history.redo()
	if bg_color[0] == 0.5 then
		sandbox.logs.info(world, "[Renderer Page Test] Passed redo settings")
	else
		sandbox.logs.error(world, "[Renderer Page Test] Failed redo settings")
	end

	if backup then
		sandbox.filesystem.write_file_string(world, test_config, backup)
	else
		sandbox.filesystem.remove_file(world, test_config)
	end

	config_path = orig_path

	-- Test page lookup of drawer
	local drw = pages.find("drawer", "Transform2D")
	if drw then
		sandbox.logs.info(world, "[Renderer Page Test] Passed drawer lookup test")
	else
		sandbox.logs.error(world, "[Renderer Page Test] Failed drawer lookup test")
	end

	sandbox.logs.info(world, "[Renderer Page Test] ===== ALL TESTS FINISHED =====")
	return true
end

return renderer_page
