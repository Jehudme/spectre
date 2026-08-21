local sandbox = require("sandbox")
local ecs = require("ecs")
local imgui = require("imgui")
local ffi = require("ffi")
local spectre = require("spectre")

require("utilities.actions.write_file")
local history = require("utilities.history")
local search = require("utilities.search")

local WriteFileAction = _G.WriteFileAction

-- ============================================================================
-- INLINE FUNCTIONS
-- ============================================================================
local function read_file(world, path)
	if not sandbox.filesystem.exists(world, path) then
		return nil
	end
	local out_data = ffi.new("uint8_t*[1]")
	local out_size = ffi.new("size_t[1]")
	if sandbox.filesystem.read_all_bytes(world, path, out_data, out_size) then
		local content = ""
		if tonumber(out_size[0]) > 0 and out_data[0] ~= nil then
			content = ffi.string(out_data[0], tonumber(out_size[0]))
		end
		if out_data[0] ~= nil then
			sandbox.filesystem.free_bytes(world, out_data[0])
		end
		return content
	end
	return nil
end

local function get_dyn_path(name)
	return "project://scenes/components/" .. name .. ".json"
end

local _schema_cache = {}
local function load_schema(world, name)
	if _schema_cache[name] then return _schema_cache[name] end
	local path = get_dyn_path(name)
	local content = read_file(world, path)
	local props = sandbox.Properties.new()
	if content then
		props:load(content, 0)
	else
		props:load("{}", 0)
	end
	_schema_cache[name] = props
	return props
end

local function invalidate_schema(name)
    _schema_cache[name] = nil
end

local function list_dynamic_components(world)
	local list = {}
	if sandbox.filesystem.exists(world, "project://scenes/components") then
		local files = sandbox.filesystem.list_files(world, "project://scenes/components", false)
		for _, file in ipairs(files) do
			if string.sub(file, -5) == ".json" then
				local name = string.match(file, "([^/\\]+)%.json$")
				if name then
					table.insert(list, name)
				end
			end
		end
	end
	table.sort(list)
	return list
end

-- ============================================================================
-- ACTIONS
-- ============================================================================
local function create_remove_file_action(path, content, name)
	local redo = function()
		local world = ecs.from_ptr(g_world)
		sandbox.filesystem.remove_file(world, path)
	end
	local undo = function()
		local world = ecs.from_ptr(g_world)
		if content then
			local c_str = ffi.cast("const void*", content)
			sandbox.filesystem.write_all_bytes(world, path, c_str, #content)
		end
	end
	return Action.new(redo, undo, true, name or "Remove File")
end

-- ============================================================================
-- ACTION FUNCTIONS
-- ============================================================================
local function action_write_file(path, content, desc)
	    local comp_name = path:match("components/([^/]+)%.json$")
    if comp_name then invalidate_schema(comp_name) end
	local action = WriteFileAction.new(path, content, true, desc)
	history.execute(action)
end

local function action_remove_file(path, content, desc)
	local action = create_remove_file_action(path, content, desc)
	history.execute(action)
end

-- ============================================================================
-- PAGE CODE
-- ============================================================================
local components_page = Page.new()

local search_buffer = ffi.new("char[256]")
local add_name_buffer = ffi.new("char[256]")
local rename_name_buffer = ffi.new("char[256]")
local new_var_name_buffer = ffi.new("char[256]")

local dynamic_components = {}
local selected_component = nil

local show_add_popup = false
local show_rename_popup = false
local rename_target = ""
local show_add_var_popup = false

local current_schema = nil
local current_schema_keys = {}
local schema_types_idx = {}
local schema_keys_buffers = {}

local var_types = { "int", "float", "double", "string", "bool" }
local c_var_types = ffi.new("const char*[?]", #var_types)
for i, v in ipairs(var_types) do
	c_var_types[i - 1] = v
end

local function select_component(world, name)
	selected_component = name

	current_schema = nil

	current_schema = load_schema(world, name)
	current_schema_keys = {}
	schema_types_idx = {}
	schema_keys_buffers = {}

	if current_schema:has("members") then
		local mem_keys = current_schema:keys("members") or {}
		for _, k in ipairs(mem_keys) do
			local m_name = current_schema:read_string("members/" .. k .. "/name")
			local m_type = current_schema:read_string("members/" .. k .. "/type")
			if m_name and m_type then
				table.insert(current_schema_keys, m_name)
				local idx = 0
				for i, v in ipairs(var_types) do
					if v == m_type then
						idx = i - 1
					end
				end
				schema_types_idx[m_name] = ffi.new("int[1]", idx)
				local buf = _g_draw_char
	ffi.copy(buf, m_name)
				schema_keys_buffers[m_name] = buf
			end
		end
	end
end

local function refresh_lists(world)
	dynamic_components = list_dynamic_components(world)
end

function components_page:on_enter()
	local world = ecs.from_ptr(g_world)
	refresh_lists(world)
	current_schema = nil
	selected_component = nil
end

function components_page:on_exit()
	current_schema = nil
end

function components_page:on_render()
	local world = ecs.from_ptr(g_world)

	imgui.BeginChild("ComponentsList", ffi.new("ImVec2", 280, 0), true)

	-- Toolbar: search + new + refresh
	imgui.SetNextItemWidth(-80)
	imgui.InputTextWithHint("##Search", "Filter components...", search_buffer, 256)
	imgui.SameLine()
	if imgui.Button("New") then
		show_add_popup = true
		add_name_buffer[0] = 0
	end
	imgui.SameLine()
	if imgui.SmallButton("R##Refresh") then
		refresh_lists(world)
	end
	if imgui.IsItemHovered() then imgui.SetTooltip("Refresh list") end

	imgui.Separator()

	local search_str = ffi.string(search_buffer)

	local function draw_list(list)
        local filtered = search.filter(list, search_str)
		if #filtered == 0 then
			imgui.TextDisabled(search_str ~= "" and "No matches." or "No components yet.")
		end
		for _, name in ipairs(filtered) do
            local is_selected = (selected_component == name)
            if imgui.Selectable(name, is_selected) then
                if not is_selected then
                    select_component(world, name)
                end
            end

            if imgui.BeginPopupContextItem("ContextPopup_" .. name) then
				imgui.TextDisabled(name)
				imgui.Separator()
                if imgui.MenuItem("Rename") then
                    show_rename_popup = true
                    rename_target = name
                    ffi.copy(rename_name_buffer, name)
                end
                if imgui.MenuItem("Duplicate") then
                    local old_path = get_dyn_path(name)
                    local new_name = name .. "_copy"
                    local i = 1
                    while sandbox.filesystem.exists(world, get_dyn_path(new_name)) do
                        new_name = name .. "_copy" .. tostring(i)
                        i = i + 1
                    end
                    local new_path = get_dyn_path(new_name)
                    local old_content = read_file(world, old_path) or "{}"
                    action_write_file(new_path, old_content, "Duplicate Component")
                    refresh_lists(world)
                end
				imgui.Separator()
                if imgui.MenuItem("Delete") then
                    local path = get_dyn_path(name)
                    local content = read_file(world, path) or "{}"
                    action_remove_file(path, content, "Delete Component")
                    if selected_component == name then
                        selected_component = nil
                        if current_schema then
                            current_schema:destroy()
                            current_schema = nil
                        end
                    end
                    refresh_lists(world)
                end
                imgui.EndPopup()
            end
		end
	end

	imgui.Text("Dynamic Components")
	draw_list(dynamic_components)

	imgui.EndChild()

	imgui.SameLine()

	imgui.BeginChild("ComponentConfig", ffi.new("ImVec2", 0, 0), true)
	if selected_component then
		if imgui.Selectable(selected_component, false, 0, ffi.new("ImVec2", 0, 20)) then end
		if imgui.BeginPopupContextItem("ContextPopup_MainTitle_" .. selected_component) then
			imgui.TextDisabled(selected_component)
			imgui.Separator()
			if imgui.MenuItem("Rename") then
				show_rename_popup = true
				rename_target = selected_component
				ffi.copy(rename_name_buffer, selected_component)
			end
			if imgui.MenuItem("Duplicate") then
				local old_path = get_dyn_path(selected_component)
				local new_name = selected_component .. "_copy"
				local i = 1
				while sandbox.filesystem.exists(world, get_dyn_path(new_name)) do
					new_name = selected_component .. "_copy" .. tostring(i)
					i = i + 1
				end
				local new_path = get_dyn_path(new_name)
				local old_content = read_file(world, old_path) or "{}"
				action_write_file(new_path, old_content, "Duplicate Component")
				refresh_lists(world)
			end
			imgui.Separator()
			if imgui.MenuItem("Delete") then
				local path = get_dyn_path(selected_component)
				local content = read_file(world, path) or "{}"
				action_remove_file(path, content, "Delete Component")
				selected_component = nil
				current_schema = nil
				refresh_lists(world)
			end
			imgui.EndPopup()
		end
		imgui.TextDisabled("Dynamic Component Schema")
		imgui.Separator()

		if current_schema then
			if imgui.Button("Add Variable") then
				show_add_var_popup = true
				new_var_name_buffer[0] = 0
			end

			imgui.Separator()

			local has_changes = false
			local remove_idx = nil
			for i, k in ipairs(current_schema_keys) do
				imgui.PushID(k)

				local buf = schema_keys_buffers[k]
				if imgui.InputText("##Name", buf, 256) then
					local new_k = ffi.string(buf)
					if new_k ~= k and new_k ~= "" then
						current_schema_keys[i] = new_k
						schema_keys_buffers[new_k] = schema_keys_buffers[k]
						schema_keys_buffers[k] = nil
						schema_types_idx[new_k] = schema_types_idx[k]
						schema_types_idx[k] = nil
						has_changes = true
					end
				end

				imgui.SameLine()
				local idx_ptr = schema_types_idx[current_schema_keys[i]]
				if idx_ptr and imgui.Combo("##Type", idx_ptr, c_var_types, #var_types) then
					has_changes = true
				end

				imgui.SameLine()
				if imgui.Button("Remove") then
					remove_idx = i
					has_changes = true
				end

				imgui.PopID()
			end

			if remove_idx then
				table.remove(current_schema_keys, remove_idx)
				remove_idx = nil
				has_changes = true
			end

			if has_changes then
				current_schema:clear("members")
				for i, k in ipairs(current_schema_keys) do
					local str_i = tostring(i - 1)
					current_schema:set_string("members/" .. str_i .. "/name", k)
					local t_val = var_types[schema_types_idx[k][0] + 1]
					current_schema:set_string("members/" .. str_i .. "/type", t_val)
				end
                local dumped = current_schema:dump(0)
                if dumped then
                    action_write_file(get_dyn_path(selected_component), dumped, "Update Component Schema")
                end
				select_component(world, selected_component)
			end
		end
	else
		imgui.Spacing()
		imgui.TextDisabled("Select a component on the left")
		imgui.TextDisabled("to view or edit its schema.")
	end
	imgui.EndChild()

	if show_add_popup then
		imgui.OpenPopup("New Component")
	end
	if imgui.BeginPopupModal("New Component", nil, 64) then
		show_add_popup = false
		imgui.Text("Component Name:")
		imgui.InputText("##NewComponentName", add_name_buffer, 256)

		if imgui.Button("Create") then
			local new_name = ffi.string(add_name_buffer)
			if new_name ~= "" then
				local new_path = get_dyn_path(new_name)
				if not sandbox.filesystem.exists(world, new_path) then
					if not sandbox.filesystem.exists(world, "project://scenes/components") then
						sandbox.filesystem.create_directory(world, "project://scenes/components", true)
					end
                    action_write_file(new_path, "{}", "Create Component")
					refresh_lists(world)
					select_component(world, new_name)
				end
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
		imgui.OpenPopup("Rename Component")
	end
	if imgui.BeginPopupModal("Rename Component", nil, 64) then
		show_rename_popup = false
		imgui.Text("New Name:")
		imgui.InputText("##RenameComponentName", rename_name_buffer, 256)

		if imgui.Button("Rename") then
			local new_name = ffi.string(rename_name_buffer)
			if new_name ~= "" and new_name ~= rename_target then
				local old_path = get_dyn_path(rename_target)
				local new_path = get_dyn_path(new_name)
				if not sandbox.filesystem.exists(world, new_path) then
                    local content = read_file(world, old_path) or "{}"
                    action_write_file(new_path, content, "Rename Component (Write)")
                    action_remove_file(old_path, content, "Rename Component (Remove Old)")
					if selected_component == rename_target then
						select_component(world, new_name)
					end
					refresh_lists(world)
				end
			end
			imgui.CloseCurrentPopup()
		end
		imgui.SameLine()
		if imgui.Button("Cancel") then
			imgui.CloseCurrentPopup()
		end
		imgui.EndPopup()
	end

	if show_add_var_popup then
		imgui.OpenPopup("Add Variable")
	end
	if imgui.BeginPopupModal("Add Variable", nil, 64) then
		show_add_var_popup = false
		imgui.Text("Variable Name:")
		imgui.InputText("##NewVarName", new_var_name_buffer, 256)

		if imgui.Button("Add") then
			local new_var = ffi.string(new_var_name_buffer)
			if new_var ~= "" and current_schema then
				local exists = false
				for _, k in ipairs(current_schema_keys) do
					if k == new_var then
						exists = true
					end
				end
				if not exists then
					table.insert(current_schema_keys, new_var)
					schema_types_idx[new_var] = ffi.new("int[1]", 0)
					local buf = _g_draw_char
	ffi.copy(buf, new_var)
					schema_keys_buffers[new_var] = buf

					current_schema:clear("members")
					for i, k in ipairs(current_schema_keys) do
						local str_i = tostring(i - 1)
						current_schema:set_string("members/" .. str_i .. "/name", k)
						local t_val = var_types[schema_types_idx[k][0] + 1]
						current_schema:set_string("members/" .. str_i .. "/type", t_val)
					end
                    local dumped = current_schema:dump(0)
                    if dumped then
                        action_write_file(get_dyn_path(selected_component), dumped, "Add Variable to Component")
                    end
				end
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

function components_page.list_dynamic_components()
	local world = ecs.from_ptr(g_world)
	return list_dynamic_components(world)
end

local _g_draw_int = ffi.new("int[1]")
local _g_draw_float = ffi.new("float[1]")
local _g_draw_bool = ffi.new("bool[1]")
local _g_draw_char = ffi.new("char[2048]")

function components_page.draw_dynamic_component(prefab_props, entity_path, comp_name)
	local world = ecs.from_ptr(g_world)
	local schema = load_schema(world, comp_name)
	local modified = false
	if schema:has("members") then
		local mem_keys = schema:keys("members") or {}
		for _, k in ipairs(mem_keys) do
			local m_name = schema:read_string("members/" .. k .. "/name")
			local m_type = schema:read_string("members/" .. k .. "/type")
			if m_name and m_type then
				local val_path = entity_path .. "/components/" .. comp_name .. "/" .. m_name
				imgui.PushID(val_path)
				if m_type == "int" then
										local val = prefab_props:get_int64(val_path) or 0
					_g_draw_int[0] = val
					if imgui.InputInt(m_name, _g_draw_int) then
						prefab_props:set_int64(val_path, _g_draw_int[0])
						modified = true
					end
				elseif m_type == "float" or m_type == "double" then
										local val = prefab_props:get_double(val_path) or 0.0
					_g_draw_float[0] = val
					if imgui.InputFloat(m_name, _g_draw_float) then
						prefab_props:set_double(val_path, _g_draw_float[0])
						modified = true
					end
				elseif m_type == "string" then
										local val = prefab_props:read_string(val_path) or ""
					ffi.copy(_g_draw_char, val)
					if imgui.InputText(m_name, _g_draw_char, 2048) then
						prefab_props:set_string(val_path, ffi.string(_g_draw_char))
						modified = true
					end
				elseif m_type == "bool" then
										local val = prefab_props:get_bool(val_path) or false
					_g_draw_bool[0] = val
					if imgui.Checkbox(m_name, _g_draw_bool) then
						prefab_props:set_bool(val_path, _g_draw_bool[0])
						modified = true
					end
				end
				imgui.PopID()
			end
		end
	end
	schema:destroy()
	return modified
end

-- ============================================================================
-- TESTS
-- ============================================================================
function components_page.run_test()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "Running tests for components_page module")
    sandbox.logs.info(world, "components_page tests completed.")
end

local serializer_drawer = Page.new()
function serializer_drawer:on_render(props, path)
	return false
end
pages.register("drawer", "Serializer", serializer_drawer)

return components_page
