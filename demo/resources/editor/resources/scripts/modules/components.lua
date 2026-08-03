local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local ffi = require("ffi")
local world = ecs.from_ptr(g_world)

ComponentsUI = {}

_G.modules = _G.modules or {}
_G.modules["Components"] = ComponentsUI

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

-- current dynamic schema
local current_schema = nil
local current_schema_keys = {}
local schema_types_idx = {}
local schema_keys_buffers = {}

local var_types = { "int", "float", "double", "string", "bool" }
local c_var_types = ffi.new("const char*[?]", #var_types)
for i, v in ipairs(var_types) do c_var_types[i-1] = v end

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

local function get_dyn_path(name)
    return "project://scenes/components/" .. name .. ".json"
end

local function load_schema(name)
    local path = get_dyn_path(name)
    local content = read_file(path)
    local props = sandbox.Properties.new()
    if content then
        props:load(content, 0)
    else
        props:load("{}", 0)
    end
    return props
end

local function save_schema(name, props)
    if props then
        local dumped = props:dump(0)
        if dumped then
            if not sandbox.filesystem.exists(world, "project://scenes/components") then
                sandbox.filesystem.create_directory(world, "project://scenes/components", true)
            end
            write_file(get_dyn_path(name), dumped)
        end
    end
end

local function select_component(name)
    selected_component = name
    
    if current_schema then
        current_schema:destroy()
        current_schema = nil
    end
    
    if true then
        current_schema = load_schema(name)
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
                        if v == m_type then idx = i - 1 end
                    end
                    schema_types_idx[m_name] = ffi.new("int[1]", idx)
                    local buf = ffi.new("char[256]")
                    ffi.copy(buf, m_name)
                    schema_keys_buffers[m_name] = buf
                end
            end
        end
    end
end

local function refresh_lists()

    dynamic_components = {}
    dynamic_components = {}
    if sandbox.filesystem.exists(world, "project://scenes/components") then
        local files = sandbox.filesystem.list_files(world, "project://scenes/components", false)
        for _, file in ipairs(files) do
            if string.sub(file, -5) == ".json" then
                local name = string.match(file, "([^/\\]+)%.json$")
                if name then
                    table.insert(dynamic_components, name)
                end
            end
        end
    end
    table.sort(dynamic_components)
end

function ComponentsUI.on_enter()
    refresh_lists()
    if current_schema then
        current_schema:destroy()
        current_schema = nil
    end
    selected_component = nil
end

function ComponentsUI.on_update()
    
    local screen_w = spectre.window.get_width(world)
    local screen_h = spectre.window.get_height(world)
    
    imgui.SetNextWindowPos(ffi.new("ImVec2", 0, 20), 1)
    imgui.SetNextWindowSize(ffi.new("ImVec2", screen_w, screen_h - 20), 1)
    
    local window_flags = bit.bor(1, 32, 2, 4, 8192, 524288)
    imgui.Begin("Components Manager##Main", nil, window_flags)

    imgui.BeginChild("ComponentsList", ffi.new("ImVec2", 300, 0), true)
    
    imgui.InputText("##Search", search_buffer, 256)
    imgui.SameLine()
    if imgui.Button("New") then
        show_add_popup = true
        add_name_buffer[0] = 0
    end
    imgui.SameLine()
    if imgui.Button("Refresh") then
        refresh_lists()
    end
    
    imgui.Separator()
    
    local search_str = ffi.string(search_buffer)
    
    local function draw_list(list)
        for _, name in ipairs(list) do
            if search_str == "" or string.find(name:lower(), search_str:lower(), 1, true) then
                local is_selected = (selected_component == name)
                if imgui.Selectable(name, is_selected) then
                    if not is_selected then
                        select_component(name)
                    end
                end
                
                if imgui.BeginPopupContextItem("ContextPopup_" .. name) then
                    if imgui.MenuItem("Rename") then
                        sandbox.logs.info(world, "Rename clicked on " .. name)
                        show_rename_popup = true
                        rename_target = name
                        ffi.copy(rename_name_buffer, name)
                    end
                    if imgui.MenuItem("Duplicate") then
                        sandbox.logs.info(world, "Duplicate clicked on " .. name)
                        local old_path = get_dyn_path(name)
                        local new_name = name .. "_copy"
                        local i = 1
                        while sandbox.filesystem.exists(world, get_dyn_path(new_name)) do
                            new_name = name .. "_copy" .. tostring(i)
                            i = i + 1
                        end
                        local new_path = get_dyn_path(new_name)
                        sandbox.filesystem.copy(world, old_path, new_path, false, true)
                        refresh_lists()
                    end
                    if imgui.MenuItem("Delete") then
                        sandbox.logs.info(world, "Delete clicked on " .. name)
                        sandbox.filesystem.remove_file(world, get_dyn_path(name))
                        if selected_component == name then
                            selected_component = nil
                            if current_schema then
                                current_schema:destroy()
                                current_schema = nil
                            end
                        end
                        refresh_lists()
                    end
                    imgui.EndPopup()
                end
            end
        end
    end
    
    imgui.Text("Dynamic Components:")
    draw_list(dynamic_components)
    
    imgui.EndChild()
    
    imgui.SameLine()
    
    imgui.BeginChild("ComponentConfig", ffi.new("ImVec2", 0, 0), true)
    if selected_component then
        imgui.Text("Schema for: " .. selected_component)
        imgui.Separator()
        
        if current_schema then
            if imgui.Button("Add Variable") then
                show_add_var_popup = true
                new_var_name_buffer[0] = 0
            end
            
            imgui.Separator()
            
            local has_changes = false
            for i, k in ipairs(current_schema_keys) do
                imgui.PushID(k)
                
                local buf = schema_keys_buffers[k]
                if imgui.InputText("##Name", buf, 256) then
                    local new_k = ffi.string(buf)
                    if new_k ~= k and new_k ~= "" then
                        -- Name changed, we update the key name directly
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
                    table.remove(current_schema_keys, i)
                    has_changes = true
                end
                
                imgui.PopID()
            end
            
            if has_changes then
                -- Rebuild the entire members list
                current_schema:clear("members")
                for i, k in ipairs(current_schema_keys) do
                    local str_i = tostring(i - 1)
                    current_schema:set_string("members/" .. str_i .. "/name", k)
                    local t_val = var_types[schema_types_idx[k][0] + 1]
                    current_schema:set_string("members/" .. str_i .. "/type", t_val)
                end
                save_schema(selected_component, current_schema)
                select_component(selected_component) -- reload buffers safely
            end
        end
    else
        imgui.Text("Select a component to view its schema.")
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
                    write_file(new_path, "{}")
                    refresh_lists()
                    select_component(new_name)
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
                    sandbox.filesystem.move(world, old_path, new_path, false, true)
                    if selected_component == rename_target then
                        select_component(new_name)
                    end
                    refresh_lists()
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
                    if k == new_var then exists = true end
                end
                if not exists then
                    table.insert(current_schema_keys, new_var)
                    schema_types_idx[new_var] = ffi.new("int[1]", 0)
                    local buf = ffi.new("char[256]")
                    ffi.copy(buf, new_var)
                    schema_keys_buffers[new_var] = buf
                    
                    -- force save
                    current_schema:clear("members")
                    for i, k in ipairs(current_schema_keys) do
                        local str_i = tostring(i - 1)
                        current_schema:set_string("members/" .. str_i .. "/name", k)
                        local t_val = var_types[schema_types_idx[k][0] + 1]
                        current_schema:set_string("members/" .. str_i .. "/type", t_val)
                    end
                    save_schema(selected_component, current_schema)
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
    
    imgui.End()
end

function ComponentsUI.on_exit()
    if current_schema then
        current_schema:destroy()
        current_schema = nil
    end
end

return {}
