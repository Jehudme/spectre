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
local selected_resource = nil
local show_add_popup = false

-- Resource loaders (types)
local resource_types = {}
local resource_type_names = {}
local type_current_idx = ffi.new("int[1]", 0)

-- Configuration properties
local config_props = nil
local config_path = "app://configs/resources.json"

local texture_filtering_modes = { "point", "bilinear", "trilinear" }
local texture_filtering_idx = ffi.new("int[1]", 0)
local texture_wrap_modes = { "repeat", "clamp", "mirror_repeat", "mirror_clamp" }
local texture_wrap_idx = ffi.new("int[1]", 0)
local font_size_val = ffi.new("int[1]", 32)

local function write_all_bytes(w, path, data)
	local handle = sandbox.filesystem.open_write(w, path, false, true)
	if handle and handle.token ~= 0 then
		-- Cast the string to void* to pass to write
		local c_str = ffi.cast("const void*", data)
		sandbox.filesystem.write(w, handle, c_str, #data)
		sandbox.filesystem.close_handle(w, handle)
	end
end

local function save_configuration()
	if config_props then
		local dumped = config_props:dump(0) -- SANDBOX_FORMAT_JSON
		if dumped then
			write_all_bytes(world, config_path, dumped)
		end
	end
end

local function load_configuration()
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
			config_props:load(content, 0)
			sandbox.filesystem.free_bytes(world, out_data[0])
		end
	end
end

function Resources.on_enter()
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
	
	load_configuration()
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
	local resources_list = config_props and config_props:keys("") or {}
	
	for _, res_name in ipairs(resources_list) do
		if search_str == "" or string.find(res_name:lower(), search_str:lower(), 1, true) then
			if imgui.Selectable(res_name, selected_resource == res_name) then
				selected_resource = res_name
				local vp = config_props:read_string(res_name .. ".path")
				if vp then
					ffi.copy(vpath_buffer, vp)
				else
					vpath_buffer[0] = 0
				end
			end
			
			if imgui.BeginPopupContextItem() then
				if imgui.MenuItem("Rename") then
					sandbox.logs.info(world, "Rename clicked on " .. res_name)
				end
				if imgui.MenuItem("Duplicate") then
					sandbox.logs.info(world, "Duplicate clicked on " .. res_name)
				end
				if imgui.MenuItem("Delete") then
					sandbox.logs.info(world, "Delete clicked on " .. res_name)
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
		
		imgui.Text("Virtual Path:")
		if imgui.InputText("##VirtualPath", vpath_buffer, 256) then
			config_props:set_string(res_name .. ".path", ffi.string(vpath_buffer))
			save_configuration()
		end
		
		local vpath_str = ffi.string(vpath_buffer)
		if vpath_str ~= "" and not sandbox.filesystem.exists(world, vpath_str) then
			imgui.TextColored(ffi.new("ImVec4", 1.0, 0.0, 0.0, 1.0), "Error: Virtual path does not exist!")
		end
		
		-- Fetch resource type from config
		local current_type = config_props:read_string(res_name .. ".type")
		if not current_type then current_type = "" end
		
		-- Match current_type to index
		local match_idx = 0
		for i, v in ipairs(resource_type_names) do
			if v == current_type then match_idx = i - 1 end
		end
		type_current_idx[0] = match_idx
		
		imgui.Text("Type:")
		-- Convert Lua table to array of C strings for ImGui Combo
		local c_type_names = ffi.new("const char*[?]", #resource_type_names)
		for i, v in ipairs(resource_type_names) do
			c_type_names[i-1] = v
		end
		
		if imgui.Combo("##TypeCombo", type_current_idx, c_type_names, #resource_type_names) then
			local new_type = resource_type_names[type_current_idx[0] + 1]
			config_props:set_string(res_name .. ".type", new_type)
			save_configuration()
		end
		
		current_type = config_props:read_string(res_name .. ".type")
		
		if current_type == "texture" then
			imgui.Separator()
			imgui.Text("Texture Options:")
			
			local current_filtering = config_props:read_string(res_name .. ".properties.filtering") or "point"
			for i, v in ipairs(texture_filtering_modes) do if v == current_filtering then texture_filtering_idx[0] = i - 1 end end
			
			local c_filt = ffi.new("const char*[?]", #texture_filtering_modes)
			for i, v in ipairs(texture_filtering_modes) do c_filt[i-1] = v end
			
			if imgui.Combo("Filtering", texture_filtering_idx, c_filt, #texture_filtering_modes) then
				config_props:set_string(res_name .. ".properties.filtering", texture_filtering_modes[texture_filtering_idx[0] + 1])
				save_configuration()
			end
			
			local current_wrap = config_props:read_string(res_name .. ".properties.wrap_mode") or "repeat"
			for i, v in ipairs(texture_wrap_modes) do if v == current_wrap then texture_wrap_idx[0] = i - 1 end end
			
			local c_wrap = ffi.new("const char*[?]", #texture_wrap_modes)
			for i, v in ipairs(texture_wrap_modes) do c_wrap[i-1] = v end
			
			if imgui.Combo("Wrap Mode", texture_wrap_idx, c_wrap, #texture_wrap_modes) then
				config_props:set_string(res_name .. ".properties.wrap_mode", texture_wrap_modes[texture_wrap_idx[0] + 1])
				save_configuration()
			end
			
		elseif current_type == "font" then
			imgui.Separator()
			imgui.Text("Font Options:")
			
			font_size_val[0] = config_props:get_int64(res_name .. ".properties.font_size") or 32
			if imgui.InputInt("Font Size", font_size_val) then
				config_props:set_int64(res_name .. ".properties.font_size", font_size_val[0])
				save_configuration()
			end
			
			local current_filtering = config_props:read_string(res_name .. ".properties.filtering") or "point"
			for i, v in ipairs(texture_filtering_modes) do if v == current_filtering then texture_filtering_idx[0] = i - 1 end end
			
			local c_filt = ffi.new("const char*[?]", #texture_filtering_modes)
			for i, v in ipairs(texture_filtering_modes) do c_filt[i-1] = v end
			
			if imgui.Combo("Filtering", texture_filtering_idx, c_filt, #texture_filtering_modes) then
				config_props:set_string(res_name .. ".properties.filtering", texture_filtering_modes[texture_filtering_idx[0] + 1])
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
			if new_name ~= "" then
				-- We add it to the configuration (it might not be an entity immediately until runtime reads it, or we can create the entity)
				config_props:set_string(new_name .. ".type", "texture")
				config_props:set_string(new_name .. ".path", "")
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
	
	imgui.End()
end

function Resources.on_exit()
	if config_props then
		config_props:destroy()
		config_props = nil
	end
end

return {}
