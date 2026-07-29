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
local type_buffer = ffi.new("char[256]")
local selected_resource = nil

function Resources.on_enter() end

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
		-- Logic to add resource
		sandbox.logs.info(world, "Add Resource clicked")
	end
	
	imgui.Separator()
	
	local resources_list = spectre.resources.list_resources(world)
	local search_str = ffi.string(search_buffer)
	
	for _, res_id in ipairs(resources_list) do
		local name_ptr = ffi.C.ecs_get_name((type(world) == "table" and world.ptr) and world.ptr or world, res_id)
		local res_name = name_ptr ~= nil and ffi.string(name_ptr) or tostring(res_id)
		
		if search_str == "" or string.find(res_name:lower(), search_str:lower(), 1, true) then
			if imgui.Selectable(res_name, selected_resource == res_id) then
				selected_resource = res_id
				-- Here we should ideally fetch the resource configuration to populate vpath_buffer and type_buffer
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
		local name_ptr = ffi.C.ecs_get_name((type(world) == "table" and world.ptr) and world.ptr or world, selected_resource)
		local res_name = name_ptr ~= nil and ffi.string(name_ptr) or tostring(selected_resource)
		
		imgui.Text("Configuration for: " .. res_name)
		imgui.Separator()
		
		imgui.Text("Virtual Path:")
		imgui.InputText("##VirtualPath", vpath_buffer, 256)
		
		local vpath_str = ffi.string(vpath_buffer)
		if vpath_str ~= "" and not sandbox.filesystem.exists(world, vpath_str) then
			imgui.TextColored(ffi.new("ImVec4", 1.0, 0.0, 0.0, 1.0), "Error: Virtual path does not exist!")
		end
		
		imgui.Text("Type:")
		imgui.InputText("##Type", type_buffer, 256)
		
		local type_str = ffi.string(type_buffer)
		if type_str == "texture" then
			imgui.Text("Texture Options:")
			-- Add texture specific options here
		end
	else
		imgui.Text("Select a resource to view configuration.")
	end
	imgui.EndChild()
	
	imgui.End()
end

function Resources.on_exit() end

return {}
