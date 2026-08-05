local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local ffi = require("ffi")
local world = ecs.from_ptr(g_world)

Resources = {}

_G.modules = _G.modules or {}
_G.modules["Resources"] = Resources

local search_buffer = ffi.new("char[256]")
local vpath_buffer = ffi.new("char[256]")
local add_name_buffer = ffi.new("char[256]")
local rename_name_buffer = ffi.new("char[256]")
local selected_resource = nil
local show_add_popup = false
local show_rename_popup = false
local rename_target = ""

-- Resource loaders (types)
local resource_types = {}
local resource_type_names = {}
local type_current_idx = ffi.new("int[1]", 0)

-- Configuration properties
local config_props = nil
local config_path = "project://configs/resources.json"
local cached_resources_list = {}

local function update_cached_resources()
	if config_props then
		cached_resources_list = config_props:keys("") or {}
	end
end

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

local function write_all_bytes(w, path, data)
	local c_str = ffi.cast("const void*", data)
	if sandbox.filesystem.write_all_bytes(w, path, c_str, #data) then
		sandbox.logs.info(w, "[Resources] successfully wrote to " .. path)
	else
		sandbox.logs.error(w, "[Resources] Failed to write bytes to: " .. path)
	end
end

local function save_configuration()
	if config_props then
		local dumped = config_props:dump(0) -- SANDBOX_FORMAT_JSON
		if dumped then
			sandbox.logs.info(
				world,
				"[Resources] Exporting configuration to " .. config_path .. "\nDumped content: " .. dumped
			)
			write_all_bytes(world, config_path, dumped)
		end
		update_cached_resources()
	end
end

local function load_configuration()
	sandbox.logs.info(world, "[Resources] Loading configuration from " .. config_path)
	if config_props then
		config_props:destroy()
	end
	config_props = sandbox.Properties.new()

	if not sandbox.filesystem.exists(world, config_path) then
		write_all_bytes(world, config_path, "{}")
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

local function rename_resource(old_name, new_name)
	local dumped_sub = config_props:sub(old_name):dump(0)
	if dumped_sub then
		local new_json = string.format('{"%s": %s}', new_name, dumped_sub)
		config_props:load(new_json, 0)
		config_props:clear(old_name)
		if selected_resource == old_name then
			selected_resource = new_name
		end
		save_configuration()
	end
end

local function duplicate_resource(old_name)
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
		save_configuration()
	end
end

function Resources.on_enter()
	selected_resource = nil
	-- Fetch available resource types
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

	load_configuration()
end

function Resources.get_resources_by_type(target_type)
	if not config_props then
		load_configuration()
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

function Resources.on_update()
	local screen_w = spectre.window.get_width(world)
	local screen_h = spectre.window.get_height(world)

	imgui.SetNextWindowPos(ffi.new("ImVec2", 0, 20), 1)
	imgui.SetNextWindowSize(ffi.new("ImVec2", screen_w, screen_h - 20), 1)

	local window_flags = bit.bor(1, 32, 2, 4, 8192, 524288)
	imgui.Begin("Resources Manager##Main", nil, window_flags)

	-- Right/Left Panel for list of resources (width 300)
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

	for _, res_name in ipairs(cached_resources_list) do
		if search_str == "" or string.find(res_name:lower(), search_str:lower(), 1, true) then
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
					duplicate_resource(res_name)
				end
				if imgui.MenuItem("Delete") then
					sandbox.logs.info(world, "Delete clicked on " .. res_name)
					config_props:clear(res_name)
					if selected_resource == res_name then
						selected_resource = nil
					end
					save_configuration()
				end
				imgui.EndPopup()
			end
		end
	end

	imgui.EndChild()

	imgui.SameLine()

	-- Center Panel for configuration
	imgui.BeginChild("ResourceConfig", ffi.new("ImVec2", 0, 0), true)
	if selected_resource then
		local res_name = selected_resource

		imgui.Text("Configuration for: " .. res_name)
		imgui.Separator()
		local res_sub = config_props:sub(res_name)
		local res_config = res_sub:sub("configurations")

		imgui.Text("Virtual Path:")
		if imgui.InputText("##VirtualPath", vpath_buffer, 256) then
			current_resource_state.path = ffi.string(vpath_buffer)
			res_sub:set_string("path", current_resource_state.path)
			save_configuration()
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
			res_sub:set_string("type", new_type)
			save_configuration()
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
				res_config:set_string("filtering", new_filt)
				save_configuration()
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
				res_config:set_string("wrap_mode", new_wrap)
				save_configuration()
			end
		elseif current_type == "font" then
			imgui.Separator()
			imgui.Text("Font Options:")

			font_size_val[0] = current_resource_state.font_size
			if imgui.InputInt("Font Size", font_size_val) then
				current_resource_state.font_size = font_size_val[0]
				res_config:set_int64("font_size", font_size_val[0])
				save_configuration()
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
				res_config:set_string("filtering", new_filt)
				save_configuration()
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
				local new_json =
					string.format('{"%s": {"type": "texture", "path": "", "configurations": {}}}', new_name)
				config_props:load(new_json, 0)
				save_configuration()
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
				rename_resource(rename_target, new_name)
			end
			imgui.CloseCurrentPopup()
		end
		imgui.SameLine()
		if imgui.Button("Cancel") then
			imgui.CloseCurrentPopup()
		end
		imgui.EndPopup()
	end

	imgui.End()
end

function Resources.on_exit()
	if config_props then
		config_props:destroy()
		config_props = nil
	end
end

_G.modules["Prefabs"] = _G.modules["Prefabs"] or { Drawers = {} }
local Drawers = _G.modules["Prefabs"].Drawers

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

return {}
