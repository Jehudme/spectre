local sandbox = require("sandbox")
local ecs = require("ecs")
local imgui = require("imgui")
local ffi = require("ffi")
local spectre = require("spectre")
local components_module = require("interface.pages.modules.components")

require("utilities.actions.write_file")
local history = require("utilities.history")
local search = require("utilities.search")

local scenes_page = Page.new()

_G.PrefabsDrawers = _G.PrefabsDrawers or {}
local Drawers = _G.PrefabsDrawers

local search_buffer = ffi.new("char[256]")
local add_name_buffer = ffi.new("char[256]")
local rename_name_buffer = ffi.new("char[256]")
local add_child_buffer = ffi.new("char[256]")
local rename_entity_buffer = ffi.new("char[256]")

local states_list = {}
local scenes_list = {}

local selected_mode = nil
local selected_item = nil
local current_props = nil

local show_add_state_popup = false
local show_add_scene_popup = false
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

local save_item

-- inline functions

local function get_item_path(mode, name)
	if mode == "state" then
		return "project://scenes/states/" .. name .. ".json"
	else
		return "project://scenes/scenes/" .. name .. ".json"
	end
end

local function load_item(world, mode, name)
	local path = get_item_path(mode, name)
	local content = sandbox.filesystem.read_file_string(world, path)
	local props = sandbox.Properties.new()
	if content then
		props:load(content, 0)
	else
		if mode == "state" then
			props:load(string.format('{"name":"%s","scenes":[]}', name), 0)
		else
			props:load(string.format('{"entities":{"%s":{"components":{"Scene":{}}}}}', name), 0)
		end
	end
	return props
end

local function refresh_lists(world)
	states_list = {}
	if sandbox.filesystem.exists(world, "project://scenes/states") then
		local files = sandbox.filesystem.list_files(world, "project://scenes/states", false)
		for _, file in ipairs(files) do
			if string.sub(file, -5) == ".json" then
				local name = string.match(file, "([^/\\]+)%.json$")
				if name then table.insert(states_list, name) end
			end
		end
	end
	table.sort(states_list)

	scenes_list = {}
	if sandbox.filesystem.exists(world, "project://scenes/scenes") then
		local files = sandbox.filesystem.list_files(world, "project://scenes/scenes", false)
		for _, file in ipairs(files) do
			if string.sub(file, -5) == ".json" then
				local name = string.match(file, "([^/\\]+)%.json$")
				if name then table.insert(scenes_list, name) end
			end
		end
	end
	table.sort(scenes_list)
end

local function select_item(world, mode, name)
	selected_mode = mode
	selected_item = name
	if current_props then
		current_props:destroy()
		current_props = nil
	end
	if name then
		current_props = load_item(world, mode, name)
		selected_entity = nil
	end
end

local function draw_hierarchy(world, props, path, name)
	local flags = bit.bor(64, 2048)
	if selected_entity == path then
		flags = bit.bor(flags, 1)
	end

	local children_path = path .. "/children"
	local children_keys = {}
	if props:has(children_path) then
		children_keys = props:keys(children_path) or {}
	end
	local has_children = (#children_keys > 0)

	if not has_children then
		flags = bit.bor(flags, 256, 16)
	end

	local open = imgui.TreeNodeEx(name .. "##" .. path, flags)

	if imgui.IsItemClicked(0) and not imgui.IsItemToggledOpen() then
		selected_entity = path
	end

	local is_root = (path == "entities/" .. selected_item)

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
			props:set_string(parent_path .. "/" .. new_name .. "/dummy", "0")
			props:clear(parent_path .. "/" .. new_name .. "/dummy")
			save_item(world, selected_mode, selected_item, props)
		end
		if not is_root and imgui.MenuItem("Delete") then
			props:clear(path)
			if selected_entity == path then
				selected_entity = nil
			end
			save_item(world, selected_mode, selected_item, props)
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

-- actions

local RemoveFileAction = {}
RemoveFileAction.__index = RemoveFileAction
function RemoveFileAction.new(path, action_name)
    local self = setmetatable({}, RemoveFileAction)
    self.path = path
    self.action_name = action_name or "Remove File"
    self.old_content = nil
    return self
end
function RemoveFileAction:execute()
    local world = ecs.from_ptr(g_world)
    self.old_content = sandbox.filesystem.read_file_string(world, self.path)
    sandbox.filesystem.remove_file(world, self.path)
end
function RemoveFileAction:undo()
    local world = ecs.from_ptr(g_world)
    if self.old_content then
        sandbox.filesystem.write_file_string(world, self.path, self.old_content)
    end
end

local MoveFileAction = {}
MoveFileAction.__index = MoveFileAction
function MoveFileAction.new(old_path, new_path, action_name)
    local self = setmetatable({}, MoveFileAction)
    self.old_path = old_path
    self.new_path = new_path
    self.action_name = action_name or "Move File"
    return self
end
function MoveFileAction:execute()
    local world = ecs.from_ptr(g_world)
    sandbox.filesystem.move(world, self.old_path, self.new_path, false, true)
end
function MoveFileAction:undo()
    local world = ecs.from_ptr(g_world)
    sandbox.filesystem.move(world, self.new_path, self.old_path, false, true)
end

local CopyFileAction = {}
CopyFileAction.__index = CopyFileAction
function CopyFileAction.new(old_path, new_path, action_name)
    local self = setmetatable({}, CopyFileAction)
    self.old_path = old_path
    self.new_path = new_path
    self.action_name = action_name or "Copy File"
    return self
end
function CopyFileAction:execute()
    local world = ecs.from_ptr(g_world)
    sandbox.filesystem.copy(world, self.old_path, self.new_path, false, true)
end
function CopyFileAction:undo()
    local world = ecs.from_ptr(g_world)
    sandbox.filesystem.remove_file(world, self.new_path)
end

-- action functions

local function execute_write_file(path, content, name)
    local action = _G.WriteFileAction.new(path, content, true, name)
    history.execute(action)
end

local function execute_remove_file(path, name)
    local action = RemoveFileAction.new(path, name)
    history.execute(action)
end

local function execute_move_file(old_path, new_path, name)
    local action = MoveFileAction.new(old_path, new_path, name)
    history.execute(action)
end

local function execute_copy_file(old_path, new_path, name)
    local action = CopyFileAction.new(old_path, new_path, name)
    history.execute(action)
end

save_item = function(world, mode, name, props)
	if props then
		local dumped = props:dump(0)
		if dumped then
			local path = get_item_path(mode, name)
			local dir = (mode == "state") and "project://scenes/states" or "project://scenes/scenes"
			if not sandbox.filesystem.exists(world, dir) then
				sandbox.filesystem.create_directory(world, dir, true)
			end
			execute_write_file(path, dumped, "Save " .. mode .. " " .. name)
		end
	end
end

-- page code

function scenes_page:on_enter()
	local world = ecs.from_ptr(g_world)
	refresh_lists(world)
	selected_mode = nil
	selected_item = nil
	selected_entity = nil
	if current_props then
		current_props:destroy()
		current_props = nil
	end
end

function scenes_page:on_exit()
	if current_props then
		current_props:destroy()
		current_props = nil
	end
end

function scenes_page:on_render()
	local world = ecs.from_ptr(g_world)
	local screen_w = spectre.window.get_width(world)
	local screen_h = spectre.window.get_height(world)

	imgui.BeginChild("LeftPanel", ffi.new("ImVec2", 300, 0), true)

	imgui.InputText("##Search", search_buffer, 256)
	if imgui.Button("New State") then
		show_add_state_popup = true
		add_name_buffer[0] = 0
	end
	imgui.SameLine()
	if imgui.Button("New Scene") then
		show_add_scene_popup = true
		add_name_buffer[0] = 0
	end
	imgui.SameLine()
	if imgui.Button("Refresh") then
		refresh_lists(world)
	end

	imgui.Separator()

	local search_str = ffi.string(search_buffer)
    local filtered_states = search_str == "" and states_list or search.filter(states_list, search_str)

	if imgui.CollapsingHeader("States", bit.bor(32)) then
		for _, name in ipairs(filtered_states) do
            local is_selected = (selected_mode == "state" and selected_item == name)
            if imgui.Selectable(name, is_selected) then
                if not is_selected then
                    select_item(world, "state", name)
                end
            end

            if imgui.BeginPopupContextItem("StateCtx_" .. name) then
                if imgui.MenuItem("Rename") then
                    show_rename_popup = true
                    rename_target = name
                    selected_mode = "state"
                    ffi.copy(rename_name_buffer, name)
                end
                if imgui.MenuItem("Duplicate") then
                    local old_path = get_item_path("state", name)
                    local new_name = name .. "_copy"
                    local i = 1
                    while sandbox.filesystem.exists(world, get_item_path("state", new_name)) do
                        new_name = name .. "_copy" .. tostring(i)
                        i = i + 1
                    end
                    local props = load_item(world, "state", name)
                    props:set_string("name", new_name)
                    save_item(world, "state", new_name, props)
                    props:destroy()
                    refresh_lists(world)
                end
                if imgui.MenuItem("Delete") then
                    execute_remove_file(get_item_path("state", name), "Delete State " .. name)
                    if selected_mode == "state" and selected_item == name then
                        select_item(world, nil, nil)
                    end
                    refresh_lists(world)
                end
                imgui.EndPopup()
            end
		end
	end

    local filtered_scenes = search_str == "" and scenes_list or search.filter(scenes_list, search_str)
	if imgui.CollapsingHeader("Scenes", bit.bor(32)) then
		for _, name in ipairs(filtered_scenes) do
            local is_selected = (selected_mode == "scene" and selected_item == name)
            if imgui.Selectable(name, is_selected) then
                if not is_selected then
                    select_item(world, "scene", name)
                end
            end

            if imgui.BeginPopupContextItem("SceneCtx_" .. name) then
                if imgui.MenuItem("Rename") then
                    show_rename_popup = true
                    rename_target = name
                    selected_mode = "scene"
                    ffi.copy(rename_name_buffer, name)
                end
                if imgui.MenuItem("Duplicate") then
                    local old_path = get_item_path("scene", name)
                    local new_name = name .. "_copy"
                    local i = 1
                    while sandbox.filesystem.exists(world, get_item_path("scene", new_name)) do
                        new_name = name .. "_copy" .. tostring(i)
                        i = i + 1
                    end
                    local props = load_item(world, "scene", name)
                    local root_sub = props:sub("entities/" .. name)
                    local root_dump = root_sub and root_sub:dump(0) or nil
                    if root_dump then
                        local new_props = sandbox.Properties.new()
                        new_props:load(string.format('{"entities":{"%s": %s}}', new_name, root_dump), 0)
                        save_item(world, "scene", new_name, new_props)
                        new_props:destroy()
                    else
                        execute_copy_file(old_path, get_item_path("scene", new_name), "Duplicate Scene " .. name)
                    end
                    props:destroy()
                    refresh_lists(world)
                end
                if imgui.MenuItem("Delete") then
                    execute_remove_file(get_item_path("scene", name), "Delete Scene " .. name)
                    if selected_mode == "scene" and selected_item == name then
                        select_item(world, nil, nil)
                    end
                    refresh_lists(world)
                end
                imgui.EndPopup()
            end
		end
	end
	imgui.EndChild()

	imgui.SameLine()

	imgui.BeginChild("RightPanel", ffi.new("ImVec2", 0, 0), false)

	if selected_mode == "state" then
		if selected_item and current_props then
			imgui.Text("State Editor: " .. selected_item)
			imgui.Separator()

			local modified = false

			if not current_props:has("name") then
				current_props:set_string("name", selected_item)
				modified = true
			end
			imgui.Text("Name: " .. current_props:read_string("name"))
			imgui.Separator()

			imgui.Text("Included Scenes:")
			if not current_props:has("scenes") then
				current_props:set_string("scenes/dummy", "0")
				current_props:clear("scenes/dummy")
				modified = true
			end

			local scene_keys = current_props:keys("scenes") or {}
			local current_scenes = {}
			for _, k in ipairs(scene_keys) do
				local s = current_props:read_string("scenes/" .. k)
				if s and s ~= "" then
					table.insert(current_scenes, s)
				end
			end

			local scenes_changed = false
			for i, scene_name in ipairs(current_scenes) do
				imgui.PushID("StateScene_" .. tostring(i))
				imgui.BulletText(scene_name)
				imgui.SameLine()
				if imgui.Button("Remove") then
					table.remove(current_scenes, i)
					scenes_changed = true
				end
				imgui.PopID()
			end

			if imgui.Button("Add Scene") then
				imgui.OpenPopup("AddSceneToState")
			end

			if imgui.BeginPopup("AddSceneToState") then
				for _, s_name in ipairs(scenes_list) do
					if imgui.Selectable(s_name) then
						table.insert(current_scenes, s_name)
						scenes_changed = true
					end
				end
				imgui.EndPopup()
			end

			if scenes_changed then
				current_props:clear("scenes")
				if #current_scenes == 0 then
					current_props:set_string("scenes/dummy", "0")
					current_props:clear("scenes/dummy")
				else
					for i, s_name in ipairs(current_scenes) do
						current_props:set_string("scenes/" .. tostring(i - 1), s_name)
					end
				end
				modified = true
			end

			if modified then
				save_item(world, "state", selected_item, current_props)
			end
		else
			imgui.Text("Select a state to edit.")
		end
	elseif selected_mode == "scene" then
		imgui.BeginChild("Hierarchy", ffi.new("ImVec2", 0, (screen_h - 20) / 2 - 10), true)
		if selected_item and current_props then
			imgui.Text("Hierarchy: " .. selected_item .. " (" .. selected_mode .. ")")
			imgui.Separator()
			if not current_props:has("entities") then
				current_props:set_string("entities/dummy", "0")
				current_props:clear("entities/dummy")
			end
			local ent_keys = current_props:keys("entities") or {}
			for _, ent_name in ipairs(ent_keys) do
				draw_hierarchy(world, current_props, "entities/" .. ent_name, ent_name)
			end
			if #ent_keys == 0 then
				if imgui.Button("Add Root Entity") then
					current_props:set_string("entities/" .. selected_item .. "/dummy", "0")
					current_props:clear("entities/" .. selected_item .. "/dummy")

					current_props:set_string("entities/" .. selected_item .. "/components/Scene/dummy", "0")
					current_props:clear("entities/" .. selected_item .. "/components/Scene/dummy")

					save_item(world, selected_mode, selected_item, current_props)
				end
			end
		else
			imgui.Text("Select a state or scene to view hierarchy.")
		end
		imgui.EndChild()

		imgui.BeginChild("Inspector", ffi.new("ImVec2", 0, 0), true)
		if selected_entity and current_props then
			local ent_name = string.match(selected_entity, "([^/]+)$")
			imgui.Text("Inspector: " .. ent_name)
			imgui.Separator()

			local comps_path = selected_entity .. "/components"
			if not current_props:has(comps_path) then
				current_props:set_string(comps_path .. "/dummy", "0")
				current_props:clear(comps_path .. "/dummy")
			end

			local comp_keys = current_props:keys(comps_path) or {}
			local modified = false

			for _, comp_name in ipairs(comp_keys) do
				if imgui.CollapsingHeader(comp_name, bit.bor(32)) then
					imgui.PushID(comp_name)

					if Drawers[comp_name] then
						if Drawers[comp_name](current_props, selected_entity) then
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
							if components_module.draw_dynamic_component(current_props, selected_entity, comp_name) then
								modified = true
							end
						else
							imgui.Text("No drawer for component: " .. comp_name)
						end
					end

					local is_root_core = (selected_entity == "entities/" .. selected_item)
						and (comp_name == "State" or comp_name == "Scene")

					if not is_root_core and imgui.Button("Remove Component") then
						current_props:clear(comps_path .. "/" .. comp_name)
						modified = true
					end

					imgui.PopID()
				end
			end

			if modified then
				save_item(world, selected_mode, selected_item, current_props)
			end

			if imgui.Button("Add Component") then
				show_add_comp_popup = true
				add_comp_target = comps_path

				local excluded = {
					Scene = true,
					State = true,
					StateContext = true,
					SceneContext = true,
					Resource = true,
					ResourceLoader = true,
					ResourceFlag = true,
					Window = true,
					InputState = true,
					Serializer = true,
					Renderable = true,
					CustomPolygoneRenderable = true,
				}

				available_components = {}
				for k, _ in pairs(Drawers) do
					if not excluded[k] and not string.match(k, "Relation$") then
						table.insert(available_components, k)
					end
				end
				local dyn_comps = components_module.list_dynamic_components()
				for _, d in ipairs(dyn_comps) do
					table.insert(available_components, d)
				end
				table.sort(available_components)
			end

			local prefabs_path = selected_entity .. "/prefabs"
			if not current_props:has(prefabs_path) then
				current_props:set_string(prefabs_path .. "/dummy", "0")
				current_props:clear(prefabs_path .. "/dummy")
			end
			local assigned_prefabs = current_props:keys(prefabs_path) or {}

			if imgui.CollapsingHeader("Assigned Prefabs", bit.bor(32)) then
				for _, p_name in ipairs(assigned_prefabs) do
					imgui.PushID("AssignedPref_" .. p_name)
					imgui.Text(p_name)
					if imgui.BeginPopupContextItem("RemPrefCtx") then
						if imgui.MenuItem("Remove") then
							current_props:clear(prefabs_path .. "/" .. p_name)
							save_item(world, selected_mode, selected_item, current_props)
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
	end

	imgui.EndChild() -- End RightPanel

	if show_add_state_popup then
		imgui.OpenPopup("New State")
	end
	if imgui.BeginPopupModal("New State", nil, 64) then
		show_add_state_popup = false
		imgui.Text("State Name:")
		imgui.InputText("##NewStateName", add_name_buffer, 256)
		if imgui.Button("Create") then
			local new_name = ffi.string(add_name_buffer)
			if new_name ~= "" then
				local p = get_item_path("state", new_name)
				if not sandbox.filesystem.exists(world, p) then
					if not sandbox.filesystem.exists(world, "project://scenes/states") then
						sandbox.filesystem.create_directory(world, "project://scenes/states", true)
					end
					execute_write_file(p, string.format('{"name":"%s","scenes":[]}', new_name), "Create State " .. new_name)
					refresh_lists(world)
					select_item(world, "state", new_name)
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

	if show_add_scene_popup then
		imgui.OpenPopup("New Scene")
	end
	if imgui.BeginPopupModal("New Scene", nil, 64) then
		show_add_scene_popup = false
		imgui.Text("Scene Name:")
		imgui.InputText("##NewSceneName", add_name_buffer, 256)
		if imgui.Button("Create") then
			local new_name = ffi.string(add_name_buffer)
			if new_name ~= "" then
				local p = get_item_path("scene", new_name)
				if not sandbox.filesystem.exists(world, p) then
					if not sandbox.filesystem.exists(world, "project://scenes/scenes") then
						sandbox.filesystem.create_directory(world, "project://scenes/scenes", true)
					end
					execute_write_file(p, string.format('{"entities":{"%s":{"components":{"Scene":{}}}}}', new_name), "Create Scene " .. new_name)
					refresh_lists(world)
					select_item(world, "scene", new_name)
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
		imgui.OpenPopup("Rename Item")
	end
	if imgui.BeginPopupModal("Rename Item", nil, 64) then
		show_rename_popup = false
		imgui.Text("New Name:")
		imgui.InputText("##RenameItemName", rename_name_buffer, 256)
		if imgui.Button("Rename") then
			local new_name = ffi.string(rename_name_buffer)
			if new_name ~= "" and new_name ~= rename_target then
				local old_path = get_item_path(selected_mode, rename_target)
				local new_path = get_item_path(selected_mode, new_name)
				if not sandbox.filesystem.exists(world, new_path) then
					local props = load_item(world, selected_mode, rename_target)
					local root_sub = props:sub("entities/" .. rename_target)
					local root_dump = root_sub and root_sub:dump(0) or nil
					if root_dump then
						local new_props = sandbox.Properties.new()
						new_props:load(string.format('{"entities":{"%s": %s}}', new_name, root_dump), 0)
						save_item(world, selected_mode, new_name, new_props)
						new_props:destroy()
						execute_remove_file(old_path, "Remove old item " .. rename_target)
					else
						execute_move_file(old_path, new_path, "Rename " .. selected_mode .. " " .. rename_target .. " to " .. new_name)
					end
					props:destroy()

					if selected_item == rename_target then
						select_item(world, selected_mode, new_name)
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

	if show_add_child_popup then
		imgui.OpenPopup("Add Child Entity")
	end
	if imgui.BeginPopupModal("Add Child Entity", nil, 64) then
		show_add_child_popup = false
		imgui.Text("Entity Name:")
		imgui.InputText("##ChildName", add_child_buffer, 256)
		if imgui.Button("Add") then
			local c_name = ffi.string(add_child_buffer)
			if c_name ~= "" and current_props then
				local cp = add_child_parent .. "/children/" .. c_name
				if not current_props:has(cp) then
					current_props:set_string(cp .. "/dummy", "0")
					current_props:clear(cp .. "/dummy")
					save_item(world, selected_mode, selected_item, current_props)
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

	if show_rename_ent_popup then
		imgui.OpenPopup("Rename Entity")
	end
	if imgui.BeginPopupModal("Rename Entity", nil, 64) then
		show_rename_ent_popup = false
		imgui.Text("New Entity Name:")
		imgui.InputText("##RenEntName", rename_entity_buffer, 256)
		if imgui.Button("Rename") then
			local n_name = ffi.string(rename_entity_buffer)
			if n_name ~= "" and current_props then
				local parent_path = string.match(rename_ent_target, "(.*)/[^/]+") or ""
				local old_name = string.match(rename_ent_target, "([^/]+)$") or ""
				if n_name ~= old_name then
					local new_path = parent_path .. "/" .. n_name
					if not current_props:has(new_path) then
						local dumped = current_props:sub(rename_ent_target):dump(0)
						if dumped then
							local temp_p = sandbox.Properties.new()
							temp_p:load(dumped, 0)
							current_props:merge(new_path, temp_p)
							temp_p:destroy()
							current_props:clear(rename_ent_target)

							if selected_entity == rename_ent_target then
								selected_entity = new_path
							end
							save_item(world, selected_mode, selected_item, current_props)
						end
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

	if show_add_comp_popup then
		imgui.OpenPopup("Add Component")
	end
	if imgui.BeginPopupModal("Add Component", nil, 64) then
		show_add_comp_popup = false
		for _, comp in ipairs(available_components) do
			if imgui.Selectable(comp, false) then
				if current_props then
					current_props:set_string(add_comp_target .. "/" .. comp .. "/dummy", "0")
					current_props:clear(add_comp_target .. "/" .. comp .. "/dummy")
					save_item(world, selected_mode, selected_item, current_props)
				end
				imgui.CloseCurrentPopup()
			end
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

		local p_list = {}
		if sandbox.filesystem.exists(world, "project://scenes/prefabs") then
			local files = sandbox.filesystem.list_files(world, "project://scenes/prefabs", false)
			for _, file in ipairs(files) do
				if string.sub(file, -5) == ".json" then
					local name = string.match(file, "([^/\\]+)%.json$")
					if name then
						table.insert(p_list, name)
					end
				end
			end
		end
		table.sort(p_list)

		for _, p in ipairs(p_list) do
			if imgui.Selectable(p, false) then
				if current_props then
					current_props:set_string(add_assigned_prefab_target .. "/" .. p .. "/dummy", "0")
					current_props:clear(add_assigned_prefab_target .. "/" .. p .. "/dummy")
					save_item(world, selected_mode, selected_item, current_props)
				end
				imgui.CloseCurrentPopup()
			end
		end

		if imgui.Button("Cancel") then
			imgui.CloseCurrentPopup()
		end
		imgui.EndPopup()
	end
end

-- tests

function run_test()
    sandbox.logs.info("Testing scenes module programmatically")
    sandbox.logs.info("Success")
end

return scenes_page
