local sandbox = require("sandbox")
local ecs = require("ecs")
local imgui = require("imgui")
local ffi = require("ffi")
local spectre = require("spectre")
require("utilities.actions.write_file")
local history = require("utilities.history")
local search = require("utilities.search")

local config_path = "project://configs/resources.json"
local cached_resources_list = {}
local config_props = nil

local search_buffer = ffi.new("char[256]")
local vpath_buffer = ffi.new("char[256]")
local add_name_buffer = ffi.new("char[256]")
local rename_name_buffer = ffi.new("char[256]")
local selected_resource = nil
local show_add_popup = false
local show_rename_popup = false
local rename_target = ""

local resource_types = {}
local resource_type_names = {}
local type_current_idx = ffi.new("int[1]", 0)

local texture_filtering_modes = { "point", "bilinear", "trilinear" }
local texture_filtering_idx = ffi.new("int[1]", 0)
local c_filt = ffi.new("const char*[?]", #texture_filtering_modes)
for i, v in ipairs(texture_filtering_modes) do
	c_filt[i - 1] = v
end

local texture_wrap_modes = { "repeat", "clamp", "mirror_repeat", "mirror_clamp" }
local texture_wrap_idx = ffi.new("int[1]", 0)
local c_wrap = ffi.new("const char*[?]", #texture_wrap_modes)
for i, v in ipairs(texture_wrap_modes) do
	c_wrap[i - 1] = v
end

local font_size_val = ffi.new("int[1]", 32)
local c_type_names = nil
local current_resource_state = {}

-- =====================================================================
-- INLINE FUNCTIONS
-- =====================================================================

local function update_cached_resources()
	if config_props then
		cached_resources_list = config_props:keys("") or {}
	end
end

local function load_configuration(world)
	sandbox.logs.info(world, "[Resources] Loading configuration from " .. config_path)
	if config_props then
		config_props:destroy()
	end
	config_props = sandbox.Properties.new()

	if not sandbox.filesystem.exists(world, config_path) then
		local action = _G.WriteFileAction.new(config_path, "{}")
		action:execute()
	end

	local out_data = ffi.new("uint8_t*[1]")
	local out_size = ffi.new("size_t[1]")
	if sandbox.filesystem.read_all_bytes(world, config_path, out_data, out_size) then
		if tonumber(out_size[0]) > 0 and out_data[0] ~= nil then
			local content = ffi.string(out_data[0], tonumber(out_size[0]))
			sandbox.logs.info(world, "[Resources] Loaded content: " .. content)
			config_props:load(content, 0)
			sandbox.filesystem.free_bytes(world, out_data[0])
		end
	end
	update_cached_resources()
end

-- =====================================================================
-- ACTIONS
-- =====================================================================

local ResourceConfigAction = {}
ResourceConfigAction.__index = ResourceConfigAction
function ResourceConfigAction.new(world, old_content, new_content)
	local self = setmetatable({}, ResourceConfigAction)
	self.world = world
	self.old_content = old_content
	self.new_content = new_content
	self.write_action = _G.WriteFileAction.new(config_path, new_content)
	self.undo_action = _G.WriteFileAction.new(config_path, old_content)
	return self
end
function ResourceConfigAction:execute()
	self.write_action:execute()
	load_configuration(self.world)
end
function ResourceConfigAction:undo()
	self.undo_action:execute()
	load_configuration(self.world)
end

-- =====================================================================
-- ACTION FUNCTIONS
-- =====================================================================

local function execute_config_change(world, update_fn)
	local old_content = config_props:dump(0) or "{}"
	update_fn()
	local new_content = config_props:dump(0) or "{}"
	local action = ResourceConfigAction.new(world, old_content, new_content)
	history.execute(action)
end

local function action_add_resource(world, new_name)
	execute_config_change(world, function()
		local new_json = string.format('{"%s": {"type": "texture", "path": "", "configurations": {}}}', new_name)
		config_props:load(new_json, 0)
	end)
end

local function action_rename_resource(world, old_name, new_name)
	execute_config_change(world, function()
		local dumped_sub = config_props:sub(old_name):dump(0)
		if dumped_sub then
			local new_json = string.format('{"%s": %s}', new_name, dumped_sub)
			config_props:load(new_json, 0)
			config_props:clear(old_name)
			if selected_resource == old_name then
				selected_resource = new_name
			end
		end
	end)
end

local function action_duplicate_resource(world, old_name)
	execute_config_change(world, function()
		local new_name = old_name .. "_copy"
		local i = 1
		while config_props:has(new_name) do
			new_name = old_name .. "_copy" .. tostring(i)
			i = i + 1
		end
		local dumped_sub = config_props:sub(old_name):dump(0)
		if dumped_sub then
			local new_json = string.format('{"%s": %s}', new_name, dumped_sub)
			config_props:load(new_json, 0)
		end
	end)
end

local function action_delete_resource(world, res_name)
	execute_config_change(world, function()
		config_props:clear(res_name)
		if selected_resource == res_name then
			selected_resource = nil
		end
	end)
end

local function action_update_resource_property(world, res_name, key, value, is_config)
	execute_config_change(world, function()
		local res_sub = config_props:sub(res_name)
		if is_config then
			res_sub = res_sub:sub("configurations")
		end
		if type(value) == "string" then
			res_sub:set_string(key, value)
		elseif type(value) == "number" then
			res_sub:set_int64(key, value)
		end
	end)
end

-- =====================================================================
-- PAGE CODE
-- =====================================================================

local resources_page = Page.new()

_G.PrefabsDrawers = _G.PrefabsDrawers or {}
local Drawers = _G.PrefabsDrawers

function resources_page:on_enter()
	local world = ecs.from_ptr(g_world)
	selected_resource = nil

	resource_types = spectre.resources.list_resource_loaders(world)
	resource_type_names = {}
	for i, loader_id in ipairs(resource_types) do
		local name_ptr = ffi.C.ecs_get_name((type(world) == "table" and world.ptr) and world.ptr or world, loader_id)
		if name_ptr ~= nil then
			table.insert(resource_type_names, ffi.string(name_ptr))
		else
			table.insert(resource_type_names, "unknown_" .. tostring(loader_id))
		end
	end

	c_type_names = ffi.new("const char*[?]", #resource_type_names)
	for i, v in ipairs(resource_type_names) do
		c_type_names[i - 1] = v
	end

	load_configuration(world)
end

function resources_page.get_resources_by_type(target_type)
	local world = ecs.from_ptr(g_world)
	if not config_props then
		load_configuration(world)
	end
	local results = {}
	for _, res_name in ipairs(cached_resources_list) do
		local res_sub = config_props:sub(res_name)
		local t = res_sub:read_string("type") or ""
		if t == target_type or target_type == nil then
			table.insert(results, res_name)
		end
	end
	return results
end

function resources_page:on_render()
	local world = ecs.from_ptr(g_world)

	imgui.BeginChild("ResourcesList", ffi.new("ImVec2", 300, 0), true)

	imgui.InputText("##Search", search_buffer, 256)
	imgui.SameLine()
	if imgui.Button("Add") then
		show_add_popup = true
		add_name_buffer[0] = 0
		type_current_idx[0] = 0
	end

	imgui.Separator()

	local search_str = ffi.string(search_buffer)
	local filtered_resources = search.filter(cached_resources_list, search_str)

	for _, res_name in ipairs(filtered_resources) do
		if imgui.Selectable(res_name, selected_resource == res_name) then
			if selected_resource ~= res_name then
				selected_resource = res_name
				local res_sub = config_props:sub(res_name)
				local res_config = res_sub:sub("configurations")

				current_resource_state.path = res_sub:read_string("path") or ""
				current_resource_state.type = res_sub:read_string("type") or ""
				current_resource_state.filtering = res_config:read_string("filtering") or "point"
				current_resource_state.wrap_mode = res_config:read_string("wrap_mode") or "repeat"
				current_resource_state.font_size = res_config:get_int64("font_size") or 32

				ffi.copy(vpath_buffer, current_resource_state.path)
			end
		end

		if imgui.BeginPopupContextItem() then
			if imgui.MenuItem("Rename") then
				sandbox.logs.info(world, "Rename clicked on " .. res_name)
				show_rename_popup = true
				rename_target = res_name
				ffi.copy(rename_name_buffer, res_name)
			end
			if imgui.MenuItem("Duplicate") then
				sandbox.logs.info(world, "Duplicate clicked on " .. res_name)
				action_duplicate_resource(world, res_name)
			end
			if imgui.MenuItem("Delete") then
				sandbox.logs.info(world, "Delete clicked on " .. res_name)
				action_delete_resource(world, res_name)
			end
			imgui.EndPopup()
		end
	end

	imgui.EndChild()

	imgui.SameLine()

	imgui.BeginChild("ResourceConfig", ffi.new("ImVec2", 0, 0), true)
	if selected_resource then
		local res_name = selected_resource

		imgui.Text("Configuration for: " .. res_name)
		imgui.Separator()

		imgui.Text("Virtual Path:")
		if imgui.InputText("##VirtualPath", vpath_buffer, 256) then
			current_resource_state.path = ffi.string(vpath_buffer)
			action_update_resource_property(world, res_name, "path", current_resource_state.path, false)
		end

		local vpath_str = ffi.string(vpath_buffer)
		if vpath_str ~= "" and not sandbox.filesystem.exists(world, vpath_str) then
			imgui.TextColored(ffi.new("ImVec4", 1.0, 0.0, 0.0, 1.0), "Error: Virtual path does not exist!")
		end

		local current_type = current_resource_state.type

		local match_idx = 0
		for i, v in ipairs(resource_type_names) do
			if v == current_type then
				match_idx = i - 1
			end
		end
		type_current_idx[0] = match_idx

		imgui.Text("Type:")

		if not c_type_names and #resource_type_names > 0 then
			c_type_names = ffi.new("const char*[?]", #resource_type_names)
			for i, v in ipairs(resource_type_names) do
				c_type_names[i - 1] = v
			end
		end

		if c_type_names and imgui.Combo("##TypeCombo", type_current_idx, c_type_names, #resource_type_names) then
			local new_type = resource_type_names[type_current_idx[0] + 1]
			current_resource_state.type = new_type
			action_update_resource_property(world, res_name, "type", new_type, false)
		end

		if current_type == "texture" then
			imgui.Separator()
			imgui.Text("Texture Options:")

			local current_filtering = current_resource_state.filtering
			for i, v in ipairs(texture_filtering_modes) do
				if v == current_filtering then
					texture_filtering_idx[0] = i - 1
				end
			end

			if imgui.Combo("Filtering", texture_filtering_idx, c_filt, #texture_filtering_modes) then
				local new_filt = texture_filtering_modes[texture_filtering_idx[0] + 1]
				current_resource_state.filtering = new_filt
				action_update_resource_property(world, res_name, "filtering", new_filt, true)
			end

			local current_wrap = current_resource_state.wrap_mode
			for i, v in ipairs(texture_wrap_modes) do
				if v == current_wrap then
					texture_wrap_idx[0] = i - 1
				end
			end

			if imgui.Combo("Wrap Mode", texture_wrap_idx, c_wrap, #texture_wrap_modes) then
				local new_wrap = texture_wrap_modes[texture_wrap_idx[0] + 1]
				current_resource_state.wrap_mode = new_wrap
				action_update_resource_property(world, res_name, "wrap_mode", new_wrap, true)
			end
		elseif current_type == "font" then
			imgui.Separator()
			imgui.Text("Font Options:")

			font_size_val[0] = current_resource_state.font_size
			if imgui.InputInt("Font Size", font_size_val) then
				current_resource_state.font_size = font_size_val[0]
				action_update_resource_property(world, res_name, "font_size", font_size_val[0], true)
			end

			local current_filtering = current_resource_state.filtering
			for i, v in ipairs(texture_filtering_modes) do
				if v == current_filtering then
					texture_filtering_idx[0] = i - 1
				end
			end

			if imgui.Combo("Filtering", texture_filtering_idx, c_filt, #texture_filtering_modes) then
				local new_filt = texture_filtering_modes[texture_filtering_idx[0] + 1]
				current_resource_state.filtering = new_filt
				action_update_resource_property(world, res_name, "filtering", new_filt, true)
			end
		end
	else
		imgui.Text("Select a resource to view configuration.")
	end
	imgui.EndChild()

	if show_add_popup then
		imgui.OpenPopup("Add Resource")
	end

	if imgui.BeginPopupModal("Add Resource", nil, 64) then
		show_add_popup = false
		imgui.Text("Resource Name:")
		imgui.InputText("##NewResourceName", add_name_buffer, 256)

		if imgui.Button("Create") then
			local new_name = ffi.string(add_name_buffer)
			if new_name ~= "" and not config_props:has(new_name) then
				action_add_resource(world, new_name)
			end
			imgui.CloseCurrentPopup()
		end
		imgui.SameLine()
		if imgui.Button("Cancel") then
			imgui.CloseCurrentPopup()
		end
		imgui.EndPopup()
	end

	if show_rename_popup then
		imgui.OpenPopup("Rename Resource")
	end

	if imgui.BeginPopupModal("Rename Resource", nil, 64) then
		show_rename_popup = false
		imgui.Text("New Name:")
		imgui.InputText("##RenameResourceName", rename_name_buffer, 256)

		if imgui.Button("Rename") then
			local new_name = ffi.string(rename_name_buffer)
			if new_name ~= "" and new_name ~= rename_target and not config_props:has(new_name) then
				action_rename_resource(world, rename_target, new_name)
			end
			imgui.CloseCurrentPopup()
		end
		imgui.SameLine()
		if imgui.Button("Cancel") then
			imgui.CloseCurrentPopup()
		end
		imgui.EndPopup()
	end
end

function resources_page:on_exit()
	if config_props then
		config_props:destroy()
		config_props = nil
	end
end

Drawers["Resource"] = function(props, path)
	return false
end
Drawers["ResourceLoader"] = function(props, path)
	return false
end
Drawers["UseLoaderRelation"] = function(props, path)
	return false
end
Drawers["ResourceFlag"] = function(props, path)
	return false
end

-- =====================================================================
-- TESTS
-- =====================================================================

local function run_test()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[Tests] Running resources_page tests...")
	
	if not config_props then
		load_configuration(world)
	end

	local test_res_name = "test_resource_" .. tostring(os.time())
	
	action_add_resource(world, test_res_name)
	if not config_props:has(test_res_name) then
		sandbox.logs.error(world, "[Tests] Failed to add resource")
		return false
	end
	
	action_rename_resource(world, test_res_name, test_res_name .. "_renamed")
	if config_props:has(test_res_name) or not config_props:has(test_res_name .. "_renamed") then
		sandbox.logs.error(world, "[Tests] Failed to rename resource")
		return false
	end
	
	action_delete_resource(world, test_res_name .. "_renamed")
	if config_props:has(test_res_name .. "_renamed") then
		sandbox.logs.error(world, "[Tests] Failed to delete resource")
		return false
	end
	
	sandbox.logs.info(world, "[Tests] resources_page tests passed!")
	return true
end

resources_page.run_test = run_test

return resources_page
