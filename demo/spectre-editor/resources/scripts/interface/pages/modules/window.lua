local sandbox = require("sandbox")
local ecs = require("ecs")
local imgui = require("imgui")
local ffi = require("ffi")
local spectre = require("spectre")
local history = require("utilities.history")
local search = require("utilities.search")
require("utilities.actions.write_file")

local window_page = Page.new()

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

-- ==========================================
-- Inline Functions
-- ==========================================

local function reset_to_defaults()
	w_state.width[0] = 800
	w_state.height[0] = 600
	ffi.copy(w_state.title, "Spectre Engine")
	w_state.vsync[0] = false
	w_state.fullscreen[0] = false
	w_state.borderless[0] = false
	w_state.resizable[0] = true
	w_state.always_on_top[0] = false
	w_state.min_width[0] = 0
	w_state.min_height[0] = 0
	w_state.max_width[0] = 0
	w_state.max_height[0] = 0
	w_state.position_x[0] = -1
	w_state.position_y[0] = -1
	w_state.minimized[0] = false
	w_state.maximized[0] = false
	w_state.visible[0] = true
	w_state.cursor_visible[0] = true
	w_state.cursor_locked[0] = false
end

local function load_configuration_from_string(world, content)
	if config_props then
		config_props:destroy()
	end
	config_props = sandbox.Properties.new()
	config_props:load(content, 0)

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
				sandbox.filesystem.free_bytes(world, out_data[0])
			end
		end
	end
end

-- ==========================================
-- Actions
-- ==========================================

-- ==========================================
-- Action Functions
-- ==========================================

local function apply_window_settings(world)
	local temp_props = sandbox.Properties.new()
	temp_props:set_int64("width", w_state.width[0])
	temp_props:set_int64("height", w_state.height[0])
	temp_props:set_string("title", ffi.string(w_state.title))
	temp_props:set_string("vsync", w_state.vsync[0] and "true" or "false")
	temp_props:set_string("fullscreen", w_state.fullscreen[0] and "true" or "false")
	temp_props:set_string("borderless", w_state.borderless[0] and "true" or "false")
	temp_props:set_string("resizable", w_state.resizable[0] and "true" or "false")
	temp_props:set_string("always_on_top", w_state.always_on_top[0] and "true" or "false")
	temp_props:set_int64("min_width", w_state.min_width[0])
	temp_props:set_int64("min_height", w_state.min_height[0])
	temp_props:set_int64("max_width", w_state.max_width[0])
	temp_props:set_int64("max_height", w_state.max_height[0])
	temp_props:set_int64("position_x", w_state.position_x[0])
	temp_props:set_int64("position_y", w_state.position_y[0])
	temp_props:set_string("minimized", w_state.minimized[0] and "true" or "false")
	temp_props:set_string("maximized", w_state.maximized[0] and "true" or "false")
	temp_props:set_string("visible", w_state.visible[0] and "true" or "false")
	temp_props:set_string("cursor_visible", w_state.cursor_visible[0] and "true" or "false")
	temp_props:set_string("cursor_locked", w_state.cursor_locked[0] and "true" or "false")

	local new_dumped = temp_props:dump(0)
	temp_props:destroy()

	local old_dumped = nil
	if sandbox.filesystem.exists(world, config_path) then
		old_dumped = sandbox.filesystem.read_file_string(world, config_path)
	end

	local redo_fn = function()
		local w = ecs.from_ptr(g_world)
		local parent = config_path:match("(.*)/[^/]+$")
		if parent and not sandbox.filesystem.exists(w, parent) then
			sandbox.filesystem.create_directory(w, parent, true)
		end
		sandbox.filesystem.write_file_string(w, config_path, new_dumped)
		load_configuration_from_string(w, new_dumped)
	end

	local undo_fn = function()
		local w = ecs.from_ptr(g_world)
		if old_dumped then
			sandbox.filesystem.write_file_string(w, config_path, old_dumped)
			load_configuration_from_string(w, old_dumped)
		else
			sandbox.filesystem.remove_file(w, config_path)
			reset_to_defaults()
		end
	end

	local action = Action.new(redo_fn, undo_fn, true, "Update Window Settings")
	history.execute(action)
end

-- ==========================================
-- Page Code
-- ==========================================

function window_page:on_enter()
	local world = ecs.from_ptr(g_world)
	load_configuration(world)
end

function window_page:on_render()
	local world = ecs.from_ptr(g_world)

	imgui.Text("Window Settings")
	imgui.TextDisabled("Configure the project window properties")
	imgui.Separator()
	imgui.Spacing()

	-- Size & title
	imgui.Text("General")
	imgui.Spacing()
	imgui.InputInt("Width", w_state.width)
	imgui.InputInt("Height", w_state.height)
	imgui.InputText("Title", w_state.title, 256)

	imgui.Spacing()
	imgui.Separator()
	imgui.Spacing()

	-- Display options
	imgui.Text("Display")
	imgui.Spacing()
	imgui.Checkbox("VSync", w_state.vsync)
	imgui.SameLine()
	imgui.Checkbox("Fullscreen", w_state.fullscreen)
	imgui.SameLine()
	imgui.Checkbox("Borderless", w_state.borderless)
	imgui.Checkbox("Resizable", w_state.resizable)
	imgui.SameLine()
	imgui.Checkbox("Always on Top", w_state.always_on_top)

	imgui.Spacing()
	imgui.Separator()
	imgui.Spacing()

	-- Constraints & positioning
	imgui.Text("Constraints & Position")
	imgui.TextDisabled("0 = no constraint  |  -1 = auto")
	imgui.Spacing()
	imgui.InputInt("Min Width", w_state.min_width)
	imgui.InputInt("Min Height", w_state.min_height)
	imgui.InputInt("Max Width", w_state.max_width)
	imgui.InputInt("Max Height", w_state.max_height)
	imgui.InputInt("Position X", w_state.position_x)
	imgui.InputInt("Position Y", w_state.position_y)

	imgui.Spacing()
	imgui.Separator()
	imgui.Spacing()

	-- State flags
	imgui.Text("State Flags")
	imgui.Spacing()
	imgui.Checkbox("Minimized", w_state.minimized)
	imgui.SameLine()
	imgui.Checkbox("Maximized", w_state.maximized)
	imgui.Checkbox("Visible", w_state.visible)
	imgui.SameLine()
	imgui.Checkbox("Cursor Visible", w_state.cursor_visible)
	imgui.SameLine()
	imgui.Checkbox("Cursor Locked", w_state.cursor_locked)

	imgui.Spacing()
	imgui.Separator()
	imgui.Spacing()

	if imgui.Button("Apply Changes") then
		apply_window_settings(world)
	end
	imgui.SameLine()
	if imgui.Button("Reset to Defaults") then
		reset_to_defaults()
		apply_window_settings(world)
	end
end

function window_page:on_exit()
	if config_props then
		config_props:destroy()
		config_props = nil
	end
end

-- ==========================================
-- Tests
-- ==========================================

function window_page.run_test()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[Window Page Test] ===== STARTING ALL TESTS =====")

	local orig_path = config_path
	config_path = "save://configs/window.json"
	local test_config = config_path
	if not sandbox.filesystem.exists(world, "save://configs") then
		sandbox.filesystem.create_directory(world, "save://configs", true)
	end

	local backup = nil
	if sandbox.filesystem.exists(world, test_config) then
		backup = sandbox.filesystem.read_file_string(world, test_config)
	end

	history.clear()

	-- Set values & apply
	w_state.width[0] = 1024
	w_state.height[0] = 768
	ffi.copy(w_state.title, "Test Title")
	apply_window_settings(world)

	if sandbox.filesystem.exists(world, test_config) then
		local content = sandbox.filesystem.read_file_string(world, test_config)
		if content:match("1024") and content:match("Test Title") then
			sandbox.logs.info(world, "[Window Page Test] Passed apply settings")
		else
			sandbox.logs.error(world, "[Window Page Test] Failed apply settings")
		end
	else
		sandbox.logs.error(world, "[Window Page Test] Failed apply settings (no file)")
	end

	-- Undo
	history.undo()
	if w_state.width[0] ~= 1024 then
		sandbox.logs.info(world, "[Window Page Test] Passed undo settings")
	else
		sandbox.logs.error(world, "[Window Page Test] Failed undo settings")
	end

	-- Redo
	history.redo()
	if w_state.width[0] == 1024 and w_state.height[0] == 768 then
		sandbox.logs.info(world, "[Window Page Test] Passed redo settings")
	else
		sandbox.logs.error(world, "[Window Page Test] Failed redo settings")
	end

	if backup then
		sandbox.filesystem.write_file_string(world, test_config, backup)
	else
		sandbox.filesystem.remove_file(world, test_config)
	end

	config_path = orig_path

	sandbox.logs.info(world, "[Window Page Test] ===== ALL TESTS FINISHED =====")
	return true
end

return window_page
