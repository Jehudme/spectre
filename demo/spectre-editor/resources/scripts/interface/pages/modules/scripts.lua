local sandbox = require("sandbox")
local ecs = require("ecs")
local imgui = require("imgui")
local ffi = require("ffi")
local spectre = require("spectre")

require("utilities.actions.write_file")
local history = require("utilities.history")
local search = require("utilities.search")

local scripts_page = Page.new()

_G.PrefabsDrawers = _G.PrefabsDrawers or {}
local Drawers = _G.PrefabsDrawers

-- ============================================================================
-- INLINE FUNCTIONS
-- ============================================================================
local available_scripts_cache = nil
local scripts_search_query = ffi.new("char[256]")
local add_script_search_query = ffi.new("char[256]")
local add_script_popup = false
local add_script_target_list = ""
local selected_script_idx = 0

local function get_available_scripts()
	if available_scripts_cache then
		return available_scripts_cache
	end

	local world = ecs.from_ptr(g_world)
	local files = sandbox.filesystem.list_files(world, "project://resources/scripts", false)
	local scripts_info = {}

	if not files then
		return scripts_info
	end

	for _, file in ipairs(files) do
		if file:match("%.lua$") then
			local path = file
			if not file:match("^project://") then
				path = "project://resources/scripts/" .. file
			end

			sandbox.logs.info(world, "[Scripts UI] Loading script file: " .. path)

			local success, script_module = pcall(require, path)

			if success and type(script_module) == "table" then
				local found_scripts = 0
				for func_name, script_def in pairs(script_module) do
					if type(script_def) == "table" and script_def.func and type(script_def.args) == "table" then
						local args = {}
						for _, arg in ipairs(script_def.args) do
							table.insert(args, arg)
						end
						sandbox.logs.info(
							world,
							"[Scripts UI] Registered script: "
								.. func_name
								.. " with "
								.. #args
								.. " args from "
								.. path
						)
						scripts_info[func_name] = args
						found_scripts = found_scripts + 1
					end
				end

				if found_scripts == 0 then
					sandbox.logs.error(world, "[Scripts UI] No valid ecs.Script.define tables returned by " .. path)
				end
			else
				local err = type(script_module) == "string" and script_module or "Did not return a table"
				sandbox.logs.error(world, "[Scripts UI] Failed to require " .. path .. " - " .. err)
			end
		end
	end

	available_scripts_cache = scripts_info
	return scripts_info
end

local function get_max_key(props, list_path)
	local max_k = 0
	for _, k in ipairs(props:keys(list_path) or {}) do
		local n = tonumber(k)
		if n and n >= max_k then
			max_k = n + 1
		end
	end
	return max_k
end

-- ============================================================================
-- ACTIONS
-- ============================================================================

local function create_property_set_action(props, path, value)
	local old_value = props:read_string(path)
	return Action.new(function()
		props:set_string(path, value)
	end, function()
		if old_value then
			props:set_string(path, old_value)
		else
			props:clear(path)
		end
	end, false, "Set Property")
end

local function create_property_clear_action(props, path)
	local old_value = props:read_string(path)
	local dumped = props:sub(path):dump(0)
	return Action.new(function()
		props:clear(path)
	end, function()
		if dumped then
			props:set_string(path .. "/dummy", "0")
			props:sub(path):load(dumped, 0)
			props:clear(path .. "/dummy")
		else
			if old_value then
				props:set_string(path, old_value)
			end
		end
	end, false, "Clear Property")
end

local function create_property_duplicate_action(props, src_path, dest_path)
	local dumped = props:sub(src_path):dump(0)
	return Action.new(function()
		if dumped then
			props:set_string(dest_path .. "/dummy", "0")
			props:sub(dest_path):load(dumped, 0)
			props:clear(dest_path .. "/dummy")
		end
	end, function()
		props:clear(dest_path)
	end, false, "Duplicate Property")
end

-- ============================================================================
-- ACTION FUNCTIONS
-- ============================================================================

local function action_set_property(props, path, value)
	history.execute(create_property_set_action(props, path, value))
end

local function action_clear_property(props, path)
	history.execute(create_property_clear_action(props, path))
end

local function action_duplicate_property(props, src_path, dest_path)
	history.execute(create_property_duplicate_action(props, src_path, dest_path))
end

-- ============================================================================
-- PAGE CODE
-- ============================================================================

function scripts_page:on_enter()
	available_scripts_cache = nil
end

function scripts_page:on_render()
	imgui.Text("Available Scripts in Project")
	imgui.Separator()

	if imgui.Button("Refresh Scripts") then
		available_scripts_cache = nil
	end

	imgui.SameLine()
	imgui.InputText("Search Scripts", scripts_search_query, 256)

	imgui.Separator()

	local scripts_info = get_available_scripts()
	local keys = {}
	for k, _ in pairs(scripts_info) do
		table.insert(keys, k)
	end

	local query_str = ffi.string(scripts_search_query)
	if query_str ~= "" then
		keys = search.filter(keys, query_str)
	end
	table.sort(keys)

	for _, func_name in ipairs(keys) do
		local args = scripts_info[func_name] or {}
		if imgui.TreeNodeEx(func_name, 32) then
			imgui.Text("Expected Arguments:")
			for _, arg in ipairs(args) do
				local arg_name = type(arg) == "table" and arg.name or arg
				local arg_type = type(arg) == "table" and arg.type or "string"
				imgui.BulletText(arg_name .. " (" .. arg_type .. ")")
			end
			imgui.TreePop()
		end
	end
end

function scripts_page:on_exit()
end

Drawers["scripts"] = function(props, path)
	local p = path .. "/components/scripts"
	local modified = false

	local lists = { "on_create", "on_update", "on_destroy", "on_enter", "on_exit", "on_render" }

	if imgui.Button("Refresh Available Scripts") then
		available_scripts_cache = nil
	end

	for _, list_name in ipairs(lists) do
		local list_path = p .. "/" .. list_name

		imgui.Separator()
		if imgui.TreeNodeEx(list_name, 34) then
			if imgui.BeginPopupContextItem("Context_" .. list_name) then
				if imgui.MenuItem("Add Script") then
					add_script_popup = true
					add_script_target_list = list_name
				end
				imgui.EndPopup()
			end

			if imgui.Button("Add Script##" .. list_name) then
				add_script_popup = true
				add_script_target_list = list_name
			end

			local keys = props:keys(list_path) or {}
			for _, key in ipairs(keys) do
				local script_path = list_path .. "/" .. key
				local func_name = props:read_string(script_path .. "/function") or ""
				imgui.PushID_Str(list_name .. "_" .. key)
				if imgui.TreeNodeEx(func_name ~= "" and func_name or "Unknown", 0) then
					if imgui.BeginPopupContextItem("Context_" .. key) then
						if imgui.MenuItem("Remove") then
							action_clear_property(props, script_path)
							modified = true
						end
						if imgui.MenuItem("Duplicate") then
							local new_script_path = list_path .. "/" .. get_max_key(props, list_path)
							action_duplicate_property(props, script_path, new_script_path)
							modified = true
						end
						imgui.EndPopup()
					end

					local scripts_info = get_available_scripts()
					local args = scripts_info[func_name] or {}
					for _, arg in ipairs(args) do
						local arg_name = type(arg) == "table" and arg.name or arg
						local arg_type = type(arg) == "table" and arg.type or "string"
						local display_name = arg_name .. " (" .. arg_type .. ")"

						if arg_type == "integer" or arg_type == "int" then
							local arg_val = props:read_string(script_path .. "/arguments/" .. arg_name) or "0"
							local n = tonumber(arg_val) or 0
							local buf = ffi.new("int[1]", n)
							if imgui.InputInt(display_name, buf) then
								action_set_property(props, script_path .. "/arguments/" .. arg_name, tostring(buf[0]))
								modified = true
							end
						elseif arg_type == "number" or arg_type == "float" then
							local arg_val = props:read_string(script_path .. "/arguments/" .. arg_name) or "0.0"
							local n = tonumber(arg_val) or 0.0
							local buf = ffi.new("float[1]", n)
							if imgui.InputFloat(display_name, buf) then
								action_set_property(props, script_path .. "/arguments/" .. arg_name, tostring(buf[0]))
								modified = true
							end
						elseif arg_type == "boolean" or arg_type == "bool" then
							local arg_val = props:read_string(script_path .. "/arguments/" .. arg_name) or "false"
							local b = arg_val == "true" or arg_val == "1"
							local buf = ffi.new("bool[1]", b)
							if imgui.Checkbox(display_name, buf) then
								action_set_property(props, script_path .. "/arguments/" .. arg_name, buf[0] and "true" or "false")
								modified = true
							end
						else
							local arg_val = props:read_string(script_path .. "/arguments/" .. arg_name) or ""
							local buf = ffi.new("char[256]")
							ffi.copy(buf, arg_val)
							if imgui.InputText(display_name, buf, 256) then
								action_set_property(props, script_path .. "/arguments/" .. arg_name, ffi.string(buf))
								modified = true
							end
						end
					end
					imgui.TreePop()
				end
				imgui.PopID()
			end
			imgui.TreePop()
		else
			if imgui.BeginPopupContextItem("Context_" .. list_name) then
				if imgui.MenuItem("Add Script") then
					add_script_popup = true
					add_script_target_list = list_name
				end
				imgui.EndPopup()
			end
		end
	end

	if add_script_popup then
		imgui.OpenPopup("Add Script")
		imgui.SetNextWindowSize(imgui.ImVec2(500, 400), 0)
	end
	if imgui.BeginPopupModal("Add Script", nil, 64) then
		local scripts_info = get_available_scripts()
		local script_names = {}
		for func_name, _ in pairs(scripts_info) do
			table.insert(script_names, func_name)
		end
		
		imgui.InputText("Search Scripts", add_script_search_query, 256)
		local add_query_str = ffi.string(add_script_search_query)
		if add_query_str ~= "" then
			script_names = search.filter(script_names, add_query_str)
		end
		table.sort(script_names)

		if imgui.BeginListBox("##ScriptsList") then
			for i, name in ipairs(script_names) do
				if imgui.Selectable(name, selected_script_idx == i) then
					selected_script_idx = i
				end
			end
			imgui.EndListBox()
		end

		if selected_script_idx > 0 and selected_script_idx <= #script_names then
			local selected_name = script_names[selected_script_idx]
			local args = scripts_info[selected_name] or {}
			imgui.Text("Arguments:")
			for _, arg in ipairs(args) do
				local arg_name = type(arg) == "table" and arg.name or arg
				local arg_type = type(arg) == "table" and arg.type or "string"
				imgui.Text("- " .. arg_name .. " : " .. arg_type)
			end

			if imgui.Button("Add") then
				local list_path = p .. "/" .. add_script_target_list
				local max_k = get_max_key(props, list_path)

				local script_path = list_path .. "/" .. max_k
				action_set_property(props, script_path .. "/function", selected_name)

				if #args > 0 then
					action_set_property(props, script_path .. "/arguments/dummy", "0")
					action_clear_property(props, script_path .. "/arguments/dummy")
					for _, arg in ipairs(args) do
						local arg_name = type(arg) == "table" and arg.name or arg
						action_set_property(props, script_path .. "/arguments/" .. arg_name, "")
					end
				else
					action_set_property(props, script_path .. "/arguments/dummy", "0")
					action_clear_property(props, script_path .. "/arguments/dummy")
				end

				modified = true
				imgui.CloseCurrentPopup()
				selected_script_idx = 0
				add_script_popup = false
				ffi.copy(add_script_search_query, "")
			end
			imgui.SameLine()
		end

		if imgui.Button("Cancel") then
			imgui.CloseCurrentPopup()
			selected_script_idx = 0
			add_script_popup = false
			ffi.copy(add_script_search_query, "")
		end
		imgui.EndPopup()
	else
		add_script_popup = false
	end

	return modified
end

-- ============================================================================
-- TESTS
-- ============================================================================

local function run_test()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[Tests] Running scripts.lua tests...")
	local success = true

	-- Test get_available_scripts
	local scripts = get_available_scripts()
	if type(scripts) ~= "table" then
		sandbox.logs.error(world, "[Tests] get_available_scripts returned invalid type")
		success = false
	end

	if success then
		sandbox.logs.info(world, "[Tests] scripts.lua tests passed!")
	else
		sandbox.logs.error(world, "[Tests] scripts.lua tests failed!")
	end
end

return scripts_page
