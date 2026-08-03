local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local world = ecs.from_ptr(g_world)

Prefabs = {}
Prefabs.Drawers = {}
_G.modules = _G.modules or {}
_G.modules["Prefabs"] = Prefabs

local ffi = require("ffi")

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

local selected_entity = nil

local function write_file(path, content)
    local c_str = ffi.cast("const void*", content)
    sandbox.filesystem.write_all_bytes(world, path, c_str, #content)
end

local function read_file(path)
    if not sandbox.filesystem.exists(world, path) then return nil end
    local out_data = ffi.new("uint8_t*[1]")
    local out_size = ffi.new("size_t[1]")
    if sandbox.filesystem.read_all_bytes(world, path, out_data, out_size) then
        if tonumber(out_size[0]) > 0 and out_data[0] ~= nil then
            local content = ffi.string(out_data[0], tonumber(out_size[0]))
            sandbox.filesystem.free_bytes(world, out_data[0])
            return content
        end
    end
    return nil
end

local function get_prefab_path(name)
    return "project://scenes/prefabs/" .. name .. ".json"
end

local function load_prefab(name)
    local path = get_prefab_path(name)
    local content = read_file(path)
    local props = sandbox.Properties.new()
    if content then
        props:load(content, 0)
    else
        props:load('{"entities":{}}', 0)
    end
    return props
end

local function save_prefab(name, props)
    if props then
        local dumped = props:dump(0)
        if dumped then
            if not sandbox.filesystem.exists(world, "project://scenes/prefabs") then
                sandbox.filesystem.create_directory(world, "project://scenes/prefabs", true)
            end
            write_file(get_prefab_path(name), dumped)
        end
    end
end

local function refresh_prefabs()
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

local function select_prefab(name)
    selected_prefab = name
    if current_prefab_props then
        current_prefab_props:destroy()
        current_prefab_props = nil
    end
    if name then
        current_prefab_props = load_prefab(name)
        selected_entity = nil
    end
end

function Prefabs.on_enter()
    refresh_prefabs()
    selected_prefab = nil
    selected_entity = nil
    if current_prefab_props then
        current_prefab_props:destroy()
        current_prefab_props = nil
    end
end

local function draw_hierarchy(props, path, name)
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
    
    if imgui.BeginPopupContextItem("EntityCtx_" .. path) then
        if imgui.MenuItem("Add Child") then
            show_add_child_popup = true
            add_child_parent = path
            add_child_buffer[0] = 0
        end
        if imgui.MenuItem("Rename") then
            show_rename_ent_popup = true
            rename_ent_target = path
            ffi.copy(rename_entity_buffer, name)
        end
        if imgui.MenuItem("Duplicate") then
            local parent_path = string.match(path, "(.*)/[^/]+") or "entities"
            local new_name = name .. "_copy"
            local idx = 1
            while props:has(parent_path .. "/" .. new_name) do
                new_name = name .. "_copy" .. tostring(idx)
                idx = idx + 1
            end
            -- Note: in a real implementation we would deep copy the property tree
            props:set_string(parent_path .. "/" .. new_name .. "/dummy", "0")
            props:clear(parent_path .. "/" .. new_name .. "/dummy")
            save_prefab(selected_prefab, props)
        end
        if imgui.MenuItem("Delete") then
            props:clear(path)
            if selected_entity == path then selected_entity = nil end
            save_prefab(selected_prefab, props)
        end
        imgui.EndPopup()
    end
    
    if open and has_children then
        for _, child_name in ipairs(children_keys) do
            draw_hierarchy(props, children_path .. "/" .. child_name, child_name)
        end
    end
    
    if open and not bit.band(flags, 256) ~= 0 then
        -- Only Pop if it was not a Leaf and it's open
        imgui.TreePop()
    end
end

function Prefabs.on_update()
    local screen_w = spectre.window.get_width(world)
    local screen_h = spectre.window.get_height(world)
    
    imgui.SetNextWindowPos(ffi.new("ImVec2", 0, 20), 1)
    imgui.SetNextWindowSize(ffi.new("ImVec2", screen_w, screen_h - 20), 1)
    
    local window_flags = bit.bor(1, 32, 2, 4, 8192, 524288)
    imgui.Begin("Prefabs Manager##Main", nil, window_flags)

    imgui.BeginChild("PrefabsList", ffi.new("ImVec2", 300, 0), true)
    
    imgui.InputText("##Search", search_buffer, 256)
    imgui.SameLine()
    if imgui.Button("New") then
        show_add_popup = true
        add_name_buffer[0] = 0
    end
    imgui.SameLine()
    if imgui.Button("Refresh") then
        refresh_prefabs()
    end
    
    imgui.Separator()
    
    local search_str = ffi.string(search_buffer)
    for _, name in ipairs(prefabs_list) do
        if search_str == "" or string.find(name:lower(), search_str:lower(), 1, true) then
            local is_selected = (selected_prefab == name)
            if imgui.Selectable(name, is_selected) then
                if not is_selected then
                    select_prefab(name)
                end
            end
            
            if imgui.BeginPopupContextItem("PrefabCtx_" .. name) then
                if imgui.MenuItem("Rename") then
                    show_rename_popup = true
                    rename_target = name
                    ffi.copy(rename_name_buffer, name)
                end
                if imgui.MenuItem("Duplicate") then
                    local old_path = get_prefab_path(name)
                    local new_name = name .. "_copy"
                    local i = 1
                    while sandbox.filesystem.exists(world, get_prefab_path(new_name)) do
                        new_name = name .. "_copy" .. tostring(i)
                        i = i + 1
                    end
                    sandbox.filesystem.copy(world, old_path, get_prefab_path(new_name), false, true)
                    refresh_prefabs()
                end
                if imgui.MenuItem("Delete") then
                    sandbox.filesystem.remove_file(world, get_prefab_path(name))
                    if selected_prefab == name then
                        select_prefab(nil)
                    end
                    refresh_prefabs()
                end
                imgui.EndPopup()
            end
        end
    end
    imgui.EndChild()

    imgui.SameLine()

    imgui.BeginChild("RightPanel", ffi.new("ImVec2", 0, 0), false)
    
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
            draw_hierarchy(current_prefab_props, "entities/" .. ent_name, ent_name)
        end
        if #ent_keys == 0 then
            if imgui.Button("Add Root Entity") then
                current_prefab_props:set_string("entities/Root/dummy", "0")
                current_prefab_props:clear("entities/Root/dummy")
                save_prefab(selected_prefab, current_prefab_props)
            end
        end
    else
        imgui.Text("Select a prefab to view hierarchy.")
    end
    imgui.EndChild()

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
                
                if Prefabs.Drawers[comp_name] then
                    if Prefabs.Drawers[comp_name](current_prefab_props, selected_entity) then
                        modified = true
                    end
                else
                    local dyn_comps = _G.modules["Components"].list_dynamic_components()
                    local is_dyn = false
                    for _, d in ipairs(dyn_comps) do
                        if d == comp_name then is_dyn = true break end
                    end
                    if is_dyn then
                        if _G.modules["Components"].draw_dynamic_component(current_prefab_props, selected_entity, comp_name) then
                            modified = true
                        end
                    else
                        imgui.Text("No drawer for component: " .. comp_name)
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
            save_prefab(selected_prefab, current_prefab_props)
        end
        
        if imgui.Button("Add Component") then
            show_add_comp_popup = true
            add_comp_target = comps_path
            
            available_components = {}
            for k, _ in pairs(Prefabs.Drawers) do
                table.insert(available_components, k)
            end
            local dyn_comps = _G.modules["Components"].list_dynamic_components()
            for _, d in ipairs(dyn_comps) do
                table.insert(available_components, d)
            end
            table.sort(available_components)
        end
    else
        imgui.Text("Select an entity to view its components.")
    end
    imgui.EndChild()

    imgui.EndChild()
    
    if show_add_popup then imgui.OpenPopup("New Prefab") end
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
                    write_file(p, '{"entities":{}}')
                    refresh_prefabs()
                    select_prefab(new_name)
                end
            end
            imgui.CloseCurrentPopup()
        end
        imgui.SameLine()
        if imgui.Button("Cancel") then imgui.CloseCurrentPopup() end
        imgui.EndPopup()
    end
    
    if show_rename_popup then imgui.OpenPopup("Rename Prefab") end
    if imgui.BeginPopupModal("Rename Prefab", nil, 64) then
        show_rename_popup = false
        imgui.Text("New Name:")
        imgui.InputText("##RenamePrefabName", rename_name_buffer, 256)
        if imgui.Button("Rename") then
            local new_name = ffi.string(rename_name_buffer)
            if new_name ~= "" and new_name ~= rename_target then
                local old_path = get_prefab_path(rename_target)
                local new_path = get_prefab_path(new_name)
                if not sandbox.filesystem.exists(world, new_path) then
                    sandbox.filesystem.move(world, old_path, new_path, false, true)
                    if selected_prefab == rename_target then
                        select_prefab(new_name)
                    end
                    refresh_prefabs()
                end
            end
            imgui.CloseCurrentPopup()
        end
        imgui.SameLine()
        if imgui.Button("Cancel") then imgui.CloseCurrentPopup() end
        imgui.EndPopup()
    end
    
    if show_add_child_popup then imgui.OpenPopup("Add Child Entity") end
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
                save_prefab(selected_prefab, current_prefab_props)
            end
            imgui.CloseCurrentPopup()
        end
        imgui.SameLine()
        if imgui.Button("Cancel") then imgui.CloseCurrentPopup() end
        imgui.EndPopup()
    end
    
    if show_rename_ent_popup then imgui.OpenPopup("Rename Entity") end
    if imgui.BeginPopupModal("Rename Entity", nil, 64) then
        show_rename_ent_popup = false
        imgui.Text("New Name:")
        imgui.InputText("##RenameEntName", rename_entity_buffer, 256)
        if imgui.Button("Rename") then
            local nname = ffi.string(rename_entity_buffer)
            if nname ~= "" and current_prefab_props then
                local parent_path = string.match(rename_ent_target, "(.*)/[^/]+")
                current_prefab_props:set_string(parent_path .. "/" .. nname .. "/dummy", "0")
                current_prefab_props:clear(parent_path .. "/" .. nname .. "/dummy")
            end
            imgui.CloseCurrentPopup()
        end
        imgui.SameLine()
        if imgui.Button("Cancel") then imgui.CloseCurrentPopup() end
        imgui.EndPopup()
    end
    
    if show_add_comp_popup then imgui.OpenPopup("Add Component") end
    if imgui.BeginPopupModal("Add Component", nil, 64) then
        show_add_comp_popup = false
        imgui.Text("Select Component:")
        for _, comp in ipairs(available_components) do
            if imgui.Button(comp) then
                if current_prefab_props then
                    local p = add_comp_target .. "/" .. comp
                    current_prefab_props:set_string(p .. "/dummy", "0")
                    current_prefab_props:clear(p .. "/dummy")
                    save_prefab(selected_prefab, current_prefab_props)
                end
                imgui.CloseCurrentPopup()
            end
        end
        if imgui.Button("Cancel") then imgui.CloseCurrentPopup() end
        imgui.EndPopup()
    end

    imgui.End()
end

function Prefabs.on_exit()
    if current_prefab_props then
        current_prefab_props:destroy()
        current_prefab_props = nil
    end
end

return {}
