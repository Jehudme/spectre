local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local world = ecs.from_ptr(g_world)

Window = {}
_G.modules = _G.modules or {}
_G.modules["Window"] = Window

local config_props = nil
local config_path = "project://configs/window.json"

local w_state = {
    width = ffi.new("int[1]", 800),
    height = ffi.new("int[1]", 600),
    title = ffi.new("char[256]"),
    vsync = ffi.new("bool[1]", false),
    fullscreen = ffi.new("bool[1]", false),
    borderless = ffi.new("bool[1]", false),
    resizable = ffi.new("bool[1]", true),
    always_on_top = ffi.new("bool[1]", false),
    min_width = ffi.new("int[1]", 0),
    min_height = ffi.new("int[1]", 0),
    max_width = ffi.new("int[1]", 0),
    max_height = ffi.new("int[1]", 0),
    position_x = ffi.new("int[1]", -1),
    position_y = ffi.new("int[1]", -1),
    minimized = ffi.new("bool[1]", false),
    maximized = ffi.new("bool[1]", false),
    visible = ffi.new("bool[1]", true),
    cursor_visible = ffi.new("bool[1]", true),
    cursor_locked = ffi.new("bool[1]", false)
}

local function save_configuration()
	if config_props then
		config_props:set_int64("width", w_state.width[0])
		config_props:set_int64("height", w_state.height[0])
		config_props:set_string("title", ffi.string(w_state.title))
		config_props:set_string("vsync", w_state.vsync[0] and "true" or "false")
		config_props:set_string("fullscreen", w_state.fullscreen[0] and "true" or "false")
		config_props:set_string("borderless", w_state.borderless[0] and "true" or "false")
		config_props:set_string("resizable", w_state.resizable[0] and "true" or "false")
		config_props:set_string("always_on_top", w_state.always_on_top[0] and "true" or "false")
		config_props:set_int64("min_width", w_state.min_width[0])
		config_props:set_int64("min_height", w_state.min_height[0])
		config_props:set_int64("max_width", w_state.max_width[0])
		config_props:set_int64("max_height", w_state.max_height[0])
		config_props:set_int64("position_x", w_state.position_x[0])
		config_props:set_int64("position_y", w_state.position_y[0])
		config_props:set_string("minimized", w_state.minimized[0] and "true" or "false")
		config_props:set_string("maximized", w_state.maximized[0] and "true" or "false")
		config_props:set_string("visible", w_state.visible[0] and "true" or "false")
		config_props:set_string("cursor_visible", w_state.cursor_visible[0] and "true" or "false")
		config_props:set_string("cursor_locked", w_state.cursor_locked[0] and "true" or "false")

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
		ffi.copy(w_state.title, "Spectre Engine")
		save_configuration()
	else
		local out_data = ffi.new("uint8_t*[1]")
		local out_size = ffi.new("size_t[1]")
		if sandbox.filesystem.read_all_bytes(world, config_path, out_data, out_size) then
			if tonumber(out_size[0]) > 0 and out_data[0] ~= nil then
				local content = ffi.string(out_data[0], tonumber(out_size[0]))
				config_props:load(content, 0)
				sandbox.filesystem.free_bytes(world, out_data[0])

				w_state.width[0] = config_props:get_int64("width") or 800
				w_state.height[0] = config_props:get_int64("height") or 600
				ffi.copy(w_state.title, config_props:read_string("title") or "Spectre Engine")
				w_state.vsync[0] = config_props:read_string("vsync") == "true"
				w_state.fullscreen[0] = config_props:read_string("fullscreen") == "true"
				w_state.borderless[0] = config_props:read_string("borderless") == "true"
				w_state.resizable[0] = config_props:read_string("resizable") ~= "false"
				w_state.always_on_top[0] = config_props:read_string("always_on_top") == "true"
				w_state.min_width[0] = config_props:get_int64("min_width") or 0
				w_state.min_height[0] = config_props:get_int64("min_height") or 0
				w_state.max_width[0] = config_props:get_int64("max_width") or 0
				w_state.max_height[0] = config_props:get_int64("max_height") or 0
				w_state.position_x[0] = config_props:get_int64("position_x") or -1
				w_state.position_y[0] = config_props:get_int64("position_y") or -1
				w_state.minimized[0] = config_props:read_string("minimized") == "true"
				w_state.maximized[0] = config_props:read_string("maximized") == "true"
				w_state.visible[0] = config_props:read_string("visible") ~= "false"
				w_state.cursor_visible[0] = config_props:read_string("cursor_visible") ~= "false"
				w_state.cursor_locked[0] = config_props:read_string("cursor_locked") == "true"
			end
		end
	end
end

function Window.on_enter()
	load_configuration()
end

function Window.on_update()
	local screen_w = spectre.window.get_width(world)
	local screen_h = spectre.window.get_height(world)
	
-- 	imgui.SetNextWindowPos(ffi.new("ImVec2", 0, 20), 1)
-- 	imgui.SetNextWindowSize(ffi.new("ImVec2", screen_w, screen_h - 20), 1)
	
	local window_flags = bit.bor(1, 32, 2, 4, 8192, 524288)
	imgui.Begin("Window Configuration##Main", nil, window_flags)
	
	imgui.Text("Window Settings")
	imgui.Separator()

	local changed = false

	if imgui.InputInt("Width", w_state.width) then changed = true end
	if imgui.InputInt("Height", w_state.height) then changed = true end
	if imgui.InputText("Title", w_state.title, 256) then changed = true end
	if imgui.Checkbox("VSync", w_state.vsync) then changed = true end
	if imgui.Checkbox("Fullscreen", w_state.fullscreen) then changed = true end
	if imgui.Checkbox("Borderless", w_state.borderless) then changed = true end
	if imgui.Checkbox("Resizable", w_state.resizable) then changed = true end
	if imgui.Checkbox("Always on Top", w_state.always_on_top) then changed = true end
	
	imgui.Separator()
	imgui.Text("Constraints & Positioning")
	if imgui.InputInt("Min Width", w_state.min_width) then changed = true end
	if imgui.InputInt("Min Height", w_state.min_height) then changed = true end
	if imgui.InputInt("Max Width", w_state.max_width) then changed = true end
	if imgui.InputInt("Max Height", w_state.max_height) then changed = true end
	if imgui.InputInt("Position X", w_state.position_x) then changed = true end
	if imgui.InputInt("Position Y", w_state.position_y) then changed = true end

	imgui.Separator()
	imgui.Text("State Flags")
	if imgui.Checkbox("Minimized", w_state.minimized) then changed = true end
	if imgui.Checkbox("Maximized", w_state.maximized) then changed = true end
	if imgui.Checkbox("Visible", w_state.visible) then changed = true end
	if imgui.Checkbox("Cursor Visible", w_state.cursor_visible) then changed = true end
	if imgui.Checkbox("Cursor Locked", w_state.cursor_locked) then changed = true end

	if changed then
		save_configuration()
	end

	imgui.End()
end

function Window.on_exit()
	if config_props then
		config_props:destroy()
		config_props = nil
	end
end

return {}
