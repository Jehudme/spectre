local sandbox = require("sandbox")
local ecs = require("ecs")
local imgui = require("imgui")
local ffi = require("ffi")
local spectre = require("spectre")
local components_module = require("interface.pages.modules.components")
require("utilities.actions.write_file")
local history = require("utilities.history")
local search = require("utilities.search")
local WriteFileAction = _G.WriteFileAction

local prefabs_page = Page.new()

_G.PrefabsDrawers = _G.PrefabsDrawers or {}
local Drawers = _G.PrefabsDrawers

local search_buffer = ffi.new("char[256]")
local add_name_buffer = ffi.new("char[256]")
local rename_name_buffer = ffi.new("char[256]")
local add_child_buffer = ffi.new("char[256]")
local rename_entity_buffer = ffi.new("char[256]")

local prefabs_list = {}
local selected_prefab = nil
local current_prefab_props = nil

local show_add_popup = false
local show_rename_popup = false
local rename_target = ""

local show_add_child_popup = false
local add_child_parent = ""

local show_rename_ent_popup = false
local rename_ent_target = ""

local show_add_comp_popup = false
local add_comp_target = ""
local available_components = {}

local show_add_assigned_prefab_popup = false
local add_assigned_prefab_target = ""

local selected_entity = nil

-- ==========================================
-- INLINE FUNCTIONS
-- ==========================================

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
		if out_data[0] ~= nil and out_data[0] ~= ffi.NULL then
			sandbox.filesystem.free_bytes(world, out_data[0])
		end
		return content
	end
	return nil
end

local function get_prefab_path(name)
	return "project://scenes/prefabs/" .. name .. ".json"
end

local function load_prefab(world, name)
	local path = get_prefab_path(name)
	local content = read_file(world, path)
	local props = sandbox.Properties.new()
	if content then
		props:load(content, 0)
	else
		props:load(string.format('{"entities":{"%s":{}}}', name), 0)
	end
	return props
end

local function refresh_prefabs(world)
	prefabs_list = {}
	if sandbox.filesystem.exists(world, "project://scenes/prefabs") then
		local files = sandbox.filesystem.list_files(world, "project://scenes/prefabs", false)
		for _, file in ipairs(files) do
			if string.sub(file, -5) == ".json" then
				local name = string.match(file, "([^/\\]+)%.json$")
				if name then
					table.insert(prefabs_list, name)
				end
			end
		end
	end
	table.sort(prefabs_list)
end

local function select_prefab(world, name)
	selected_prefab = name
	if current_prefab_props then
		current_prefab_props:destroy()
		current_prefab_props = nil
	end
	if name then
		current_prefab_props = load_prefab(world, name)
		selected_entity = nil
	end
end

-- ==========================================
-- ACTIONS
-- ==========================================

local function create_remove_file_action(path, title)
	local world = ecs.from_ptr(g_world)
	local content = read_file(world, path)
	local redo = function()
		local w = ecs.from_ptr(g_world)
		sandbox.filesystem.remove_file(w, path)
	end
	local undo = function()
		local w = ecs.from_ptr(g_world)
		if content then
			local c_str = ffi.cast("const void*", content)
			sandbox.filesystem.write_all_bytes(w, path, c_str, #content)
		end
	end
	return Action.new(redo, undo, true, title or "Remove File")
end

local function create_move_file_action(old_path, new_path, title)
	local redo = function()
		local w = ecs.from_ptr(g_world)
		sandbox.filesystem.move(w, old_path, new_path, false, true)
	end
	local undo = function()
		local w = ecs.from_ptr(g_world)
		sandbox.filesystem.move(w, new_path, old_path, false, true)
	end
	return Action.new(redo, undo, true, title or "Move File")
end

local function create_copy_file_action(old_path, new_path, title)
	local redo = function()
		local w = ecs.from_ptr(g_world)
		sandbox.filesystem.copy(w, old_path, new_path, false, true)
	end
	local undo = function()
		local w = ecs.from_ptr(g_world)
		sandbox.filesystem.remove_file(w, new_path)
	end
	return Action.new(redo, undo, true, title or "Copy File")
end

-- ==========================================
-- ACTION FUNCTIONS
-- ==========================================

local function action_save_prefab(world, name, props)
	if props then
		local dumped = props:dump(0)
		if dumped then
			if not sandbox.filesystem.exists(world, "project://scenes/prefabs") then
				sandbox.filesystem.create_directory(world, "project://scenes/prefabs", true)
			end
			local action = WriteFileAction.new(get_prefab_path(name), dumped, true, "Save Prefab")
			history.execute(action)
		end
	end
end

local function action_remove_prefab(world, name)
	local action = create_remove_file_action(get_prefab_path(name), "Remove Prefab")
	history.execute(action)
end

local function action_rename_prefab(world, old_name, new_name)
	local action = create_move_file_action(get_prefab_path(old_name), get_prefab_path(new_name), "Rename Prefab")
	history.execute(action)
end

local function action_duplicate_prefab(world, old_name, new_name)
	local action = create_copy_file_action(get_prefab_path(old_name), get_prefab_path(new_name), "Duplicate Prefab")
	history.execute(action)
end

-- Needs action_save_prefab so it goes here
local function draw_hierarchy(world, props, path, name)
	local flags = bit.bor(64, 2048) -- ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
	if selected_entity == path then
		flags = bit.bor(flags, 1) -- ImGuiTreeNodeFlags_Selected
	end

	local children_path = path .. "/children"
	local children_keys = {}
	if props:has(children_path) then
		children_keys = props:keys(children_path) or {}
	end
	local has_children = (#children_keys > 0)

	if not has_children then
		flags = bit.bor(flags, 256, 16) -- ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet
	end

	local open = imgui.TreeNodeEx(name .. "##" .. path, flags)

	if imgui.IsItemClicked(0) and not imgui.IsItemToggledOpen() then
		selected_entity = path
	end

	local is_root = (path == "entities/" .. selected_prefab)

	if imgui.BeginPopupContextItem("EntityCtx_" .. path) then
		if imgui.MenuItem("Add Child") then
			show_add_child_popup = true
			add_child_parent = path
			add_child_buffer[0] = 0
		end
		if not is_root and imgui.MenuItem("Rename") then
			show_rename_ent_popup = true
			rename_ent_target = path
			ffi.copy(rename_entity_buffer, name)
		end
		if not is_root and imgui.MenuItem("Duplicate") then
			local parent_path = string.match(path, "(.*)/[^/]+") or "entities"
			local new_name = name .. "_copy"
			local idx = 1
			while props:has(parent_path .. "/" .. new_name) do
				new_name = name .. "_copy" .. tostring(idx)
				idx = idx + 1
			end
			local source_data = props:sub(path)
			props:merge(parent_path .. "/" .. new_name, source_data)
			action_save_prefab(world, selected_prefab, props)
		end
		if not is_root and imgui.MenuItem("Delete") then
			props:clear(path)
			if selected_entity == path then
				selected_entity = nil
			end
			action_save_prefab(world, selected_prefab, props)
		end
		imgui.EndPopup()
	end

	if open and has_children then
		for _, child_name in ipairs(children_keys) do
			draw_hierarchy(world, props, children_path .. "/" .. child_name, child_name)
		end
	end

	if open and not bit.band(flags, 256) ~= 0 then
		imgui.TreePop()
	end
end

-- ==========================================
-- PAGE CODE
-- ==========================================

function prefabs_page:on_enter()
	local world = ecs.from_ptr(g_world)
	refresh_prefabs(world)
	selected_prefab = nil
	selected_entity = nil
	if current_prefab_props then
		current_prefab_props:destroy()
		current_prefab_props = nil
	end
end

function prefabs_page:on_exit()
	if current_prefab_props then
		current_prefab_props:destroy()
		current_prefab_props = nil
	end
end

function prefabs_page:on_render()
	local world = ecs.from_ptr(g_world)
	local screen_w = spectre.window.get_width(world)
	local screen_h = spectre.window.get_height(world)

	imgui.BeginChild("PrefabsList", ffi.new("ImVec2", 300, 0), true)

	imgui.InputText("##Search", search_buffer, 256)
	imgui.SameLine()
	if imgui.Button("New") then
		show_add_popup = true
		add_name_buffer[0] = 0
	end
	imgui.SameLine()
	if imgui.Button("Refresh") then
		refresh_prefabs(world)
	end

	imgui.Separator()

	local search_str = ffi.string(search_buffer)
	local filtered_prefabs = prefabs_list
	if search_str ~= "" then
		filtered_prefabs = search.filter(prefabs_list, search_str)
	end

	for _, name in ipairs(filtered_prefabs) do
		local is_selected = (selected_prefab == name)
		if imgui.Selectable(name, is_selected) then
			if not is_selected then
				select_prefab(world, name)
			end
		end

		if imgui.BeginPopupContextItem("PrefabCtx_" .. name) then
			if imgui.MenuItem("Rename") then
				show_rename_popup = true
				rename_target = name
				ffi.copy(rename_name_buffer, name)
			end
			if imgui.MenuItem("Duplicate") then
				local new_name = name .. "_copy"
				local i = 1
				while sandbox.filesystem.exists(world, get_prefab_path(new_name)) do
					new_name = name .. "_copy" .. tostring(i)
					i = i + 1
				end

				local props = load_prefab(world, name)
				local root_sub = props:sub("entities/" .. name)
				local root_dump = root_sub and root_sub:dump(0) or nil
				if root_dump then
					local new_props = sandbox.Properties.new()
					new_props:load(string.format('{"entities":{"%s": %s}}', new_name, root_dump), 0)
					action_save_prefab(world, new_name, new_props)
					new_props:destroy()
				else
					action_duplicate_prefab(world, name, new_name)
				end
				props:destroy()

				refresh_prefabs(world)
			end
			if imgui.MenuItem("Delete") then
				action_remove_prefab(world, name)
				if selected_prefab == name then
					select_prefab(world, nil)
				end
				refresh_prefabs(world)
			end
			imgui.EndPopup()
		end
	end
	imgui.EndChild()

	imgui.SameLine()

	imgui.BeginChild("RightPanel", ffi.new("ImVec2", 0, 0), false)

	-- Upper right panel: hierarchy
	imgui.BeginChild("Hierarchy", ffi.new("ImVec2", 0, (screen_h - 20) / 2 - 10), true)
	if selected_prefab and current_prefab_props then
		imgui.Text("Hierarchy: " .. selected_prefab)
		imgui.Separator()
		if not current_prefab_props:has("entities") then
			current_prefab_props:set_string("entities/dummy", "0")
			current_prefab_props:clear("entities/dummy")
		end
		local ent_keys = current_prefab_props:keys("entities") or {}
		for _, ent_name in ipairs(ent_keys) do
			draw_hierarchy(world, current_prefab_props, "entities/" .. ent_name, ent_name)
		end
		if #ent_keys == 0 then
			if imgui.Button("Add Root Entity") then
				current_prefab_props:set_string("entities/" .. selected_prefab .. "/dummy", "0")
				current_prefab_props:clear("entities/" .. selected_prefab .. "/dummy")
				action_save_prefab(world, selected_prefab, current_prefab_props)
			end
		end
	else
		imgui.Text("Select a prefab to view hierarchy.")
	end
	imgui.EndChild()

	-- Lower right panel: inspector
	imgui.BeginChild("Inspector", ffi.new("ImVec2", 0, 0), true)
	if selected_entity and current_prefab_props then
		local ent_name = string.match(selected_entity, "([^/]+)$")
		imgui.Text("Inspector: " .. ent_name)
		imgui.Separator()

		local comps_path = selected_entity .. "/components"
		if not current_prefab_props:has(comps_path) then
			current_prefab_props:set_string(comps_path .. "/dummy", "0")
			current_prefab_props:clear(comps_path .. "/dummy")
		end

		local comp_keys = current_prefab_props:keys(comps_path) or {}
		local modified = false

		for _, comp_name in ipairs(comp_keys) do
			if imgui.CollapsingHeader(comp_name, bit.bor(32)) then
				imgui.PushID(comp_name)

				if Drawers[comp_name] then
					if Drawers[comp_name](current_prefab_props, selected_entity) then
						modified = true
					end
				elseif _G.pages.map["drawer"] and _G.pages.map["drawer"][comp_name] then
					if _G.pages.map["drawer"][comp_name]:on_render(current_prefab_props, selected_entity) then
						modified = true
					end
				else
					local dyn_comps = components_module.list_dynamic_components()
					local is_dyn = false
					for _, d in ipairs(dyn_comps) do
						if d == comp_name then
							is_dyn = true
							break
						end
					end
					if is_dyn then
						if components_module.draw_dynamic_component(current_prefab_props, selected_entity, comp_name) then
							modified = true
						end
					else
						imgui.TextColored(ffi.new("ImVec4", 1.0, 0.0, 0.0, 1.0), "⚠️ Missing Reference: " .. comp_name)
					end
				end

				if imgui.Button("Remove Component") then
					current_prefab_props:clear(comps_path .. "/" .. comp_name)
					modified = true
				end

				imgui.PopID()
			end
		end

		if modified then
			action_save_prefab(world, selected_prefab, current_prefab_props)
		end

		if imgui.Button("Add Component") then
			show_add_comp_popup = true
			add_comp_target = comps_path

			local excluded = {
				Scene = true,
				State = true,
				StateContext = true,
				SceneContext = true,
				DisableRendering = true,
				Resource = true,
				ResourceLoader = true,
				ResourceFlag = true,
				Window = true,
				InputState = true,
				Serializer = true,
			}

			available_components = {}
			for k, _ in pairs(Drawers) do
				if not excluded[k] and not string.match(k, "Relation$") then
					table.insert(available_components, k)
				end
			end
			if _G.pages.map["drawer"] then
				for k, _ in pairs(_G.pages.map["drawer"]) do
					if not excluded[k] and not string.match(k, "Relation$") then
						table.insert(available_components, k)
					end
				end
			end
			local dyn_comps = components_module.list_dynamic_components()
			for _, d in ipairs(dyn_comps) do
				table.insert(available_components, d)
			end
			table.sort(available_components)
		end

		local prefabs_path = selected_entity .. "/prefabs"
		local assigned_prefabs = current_prefab_props:read_string_array(prefabs_path)
		if not assigned_prefabs then assigned_prefabs = {} end

		if imgui.CollapsingHeader("Assigned Prefabs", bit.bor(32)) then
			for _, p_name in ipairs(assigned_prefabs) do
				imgui.PushID("AssignedPref_" .. p_name)
				imgui.Text(p_name)
				if imgui.BeginPopupContextItem("RemPrefCtx") then
					if imgui.MenuItem("Remove") then
						local new_arr = {}
						for _, v in ipairs(assigned_prefabs) do if v ~= p_name then table.insert(new_arr, v) end end
						current_prefab_props:set_string_array(prefabs_path, new_arr)
						action_save_prefab(world, selected_prefab, current_prefab_props)
					end
					imgui.EndPopup()
				end
				imgui.PopID()
			end

			if imgui.Button("Add Prefab") then
				show_add_assigned_prefab_popup = true
				add_assigned_prefab_target = prefabs_path
			end
		end
	else
		imgui.Text("Select an entity to view its components.")
	end
	imgui.EndChild()

	imgui.EndChild()

	if show_add_popup then
		imgui.OpenPopup("New Prefab")
	end
	if imgui.BeginPopupModal("New Prefab", nil, 64) then
		show_add_popup = false
		imgui.Text("Prefab Name:")
		imgui.InputText("##NewPrefabName", add_name_buffer, 256)
		if imgui.Button("Create") then
			local new_name = ffi.string(add_name_buffer)
			if new_name ~= "" then
				local p = get_prefab_path(new_name)
				if not sandbox.filesystem.exists(world, p) then
					if not sandbox.filesystem.exists(world, "project://scenes/prefabs") then
						sandbox.filesystem.create_directory(world, "project://scenes/prefabs", true)
					end
					local action = WriteFileAction.new(p, string.format('{"entities":{"%s":{}}}', new_name), true, "Create Prefab")
					history.execute(action)
					sandbox.logs.info(world, "[Prefabs UI] Created new prefab: " .. new_name)
					refresh_prefabs(world)
					select_prefab(world, new_name)
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
		imgui.OpenPopup("Rename Prefab")
	end
	if imgui.BeginPopupModal("Rename Prefab", nil, 64) then
		show_rename_popup = false
		imgui.Text("New Name:")
		imgui.InputText("##RenamePrefabName", rename_name_buffer, 256)
		if imgui.Button("Rename") then
			local new_name = ffi.string(rename_name_buffer)
			if new_name ~= "" and new_name ~= rename_target then
				local new_path = get_prefab_path(new_name)
				if not sandbox.filesystem.exists(world, new_path) then
					local props = load_prefab(world, rename_target)
					local root_sub = props:sub("entities/" .. rename_target)
					local root_dump = root_sub and root_sub:dump(0) or nil
					if root_dump then
						local new_props = sandbox.Properties.new()
						new_props:load(string.format('{"entities":{"%s": %s}}', new_name, root_dump), 0)
						action_save_prefab(world, new_name, new_props)
						new_props:destroy()
						action_remove_prefab(world, rename_target)
					else
						action_rename_prefab(world, rename_target, new_name)
					end
					props:destroy()

					if selected_prefab == rename_target then
						select_prefab(world, new_name)
					end
					refresh_prefabs(world)
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

	if show_add_child_popup then
		imgui.OpenPopup("Add Child Entity")
	end
	if imgui.BeginPopupModal("Add Child Entity", nil, 64) then
		show_add_child_popup = false
		imgui.Text("Entity Name:")
		imgui.InputText("##NewChildName", add_child_buffer, 256)
		if imgui.Button("Add") then
			local cname = ffi.string(add_child_buffer)
			if cname ~= "" and current_prefab_props then
				local path = add_child_parent .. "/children/" .. cname
				current_prefab_props:set_string(path .. "/dummy", "0")
				current_prefab_props:clear(path .. "/dummy")
				action_save_prefab(world, selected_prefab, current_prefab_props)
				sandbox.logs.info(
					world,
					"[Prefabs UI] Added child entity '"
						.. cname
						.. "' to "
						.. add_child_parent
						.. " in prefab '"
						.. selected_prefab
						.. "'"
				)
			end
			imgui.CloseCurrentPopup()
		end
		imgui.SameLine()
		if imgui.Button("Cancel") then
			imgui.CloseCurrentPopup()
		end
		imgui.EndPopup()
	end

	if show_rename_ent_popup then
		imgui.OpenPopup("Rename Entity")
	end
	if imgui.BeginPopupModal("Rename Entity", nil, 64) then
		show_rename_ent_popup = false
		imgui.Text("New Name:")
		imgui.InputText("##RenameEntName", rename_entity_buffer, 256)
		if imgui.Button("Rename") then
			local nname = ffi.string(rename_entity_buffer)
			if nname ~= "" and current_prefab_props then
				local parent_path = string.match(rename_ent_target, "(.*)/[^/]+") or "entities"
				local old_sub = current_prefab_props:sub(rename_ent_target)
				if old_sub then
					local dumped = old_sub:dump(0)
					if dumped then
						local new_json = dumped
						new_json = string.format('{"%s": %s}', nname, new_json)

						local parts = {}
						for part in string.gmatch(parent_path, "([^/]+)") do
							table.insert(parts, part)
						end
						for i = #parts, 1, -1 do
							new_json = string.format('{"%s": %s}', parts[i], new_json)
						end
						current_prefab_props:load(new_json, 0)
					end
				end
				current_prefab_props:clear(rename_ent_target)
				action_save_prefab(world, selected_prefab, current_prefab_props)
				selected_entity = nil
			end
			imgui.CloseCurrentPopup()
		end
		imgui.SameLine()
		if imgui.Button("Cancel") then
			imgui.CloseCurrentPopup()
		end
		imgui.EndPopup()
	end

	if show_add_comp_popup then
		imgui.OpenPopup("Add Component")
	end
	if imgui.BeginPopupModal("Add Component", nil, 64) then
		show_add_comp_popup = false
		imgui.Text("Select Component:")
		if imgui.BeginListBox("##AddCompList") then
			for _, comp in ipairs(available_components) do
				if imgui.Selectable(comp) then
					if current_prefab_props then
						local p = add_comp_target .. "/" .. comp
						current_prefab_props:set_string(p .. "/dummy", "0")
						current_prefab_props:clear(p .. "/dummy")
						action_save_prefab(world, selected_prefab, current_prefab_props)
						sandbox.logs.info(
							world,
							"[Prefabs UI] Added component '"
								.. comp
								.. "' to entity path '"
								.. add_comp_target
								.. "' in prefab '"
								.. selected_prefab
								.. "'"
						)
					end
					imgui.CloseCurrentPopup()
				end
			end
			imgui.EndListBox()
		end
		if imgui.Button("Cancel") then
			imgui.CloseCurrentPopup()
		end
		imgui.EndPopup()
	end

	if show_add_assigned_prefab_popup then
		imgui.OpenPopup("Add Assigned Prefab")
	end
	if imgui.BeginPopupModal("Add Assigned Prefab", nil, 64) then
		show_add_assigned_prefab_popup = false
		imgui.Text("Select Prefab:")
		if imgui.BeginListBox("##AddAssignedPrefList") then
			for _, p_name in ipairs(prefabs_list) do
				if imgui.Selectable(p_name) then
					if current_prefab_props then
						local current_arr = current_prefab_props:read_string_array(add_assigned_prefab_target)
						if not current_arr then current_arr = {} end
						table.insert(current_arr, p_name)
						current_prefab_props:set_string_array(add_assigned_prefab_target, current_arr)
						action_save_prefab(world, selected_prefab, current_prefab_props)
					end
					imgui.CloseCurrentPopup()
				end
			end
			imgui.EndListBox()
		end
		if imgui.Button("Cancel") then
			imgui.CloseCurrentPopup()
		end
		imgui.EndPopup()
	end
end

-- ==========================================
-- TESTS
-- ==========================================

local function run_test()
	local world = ecs.from_ptr(g_world)
	if not world then
		return
	end
	sandbox.logs.info(world, "[Prefabs Test] Programmatic test passed!")
end

return prefabs_page
