local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local ffi = require("ffi")
local scripts = require("spectre").scripts
local world = ecs.from_ptr(g_world)

pcall(ffi.cdef, [[
    typedef enum {
        SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL = 0,
        SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN,
        SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER,
        SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER,
        SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING,
        SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE,
        SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA
    } spectre_script_argument_type_t;
    typedef struct spectre_script_t {
        int lua_function_ref;
        const char** arguments_name;
        const spectre_script_argument_type_t* argument_types;
        uint32_t argument_count;
    } spectre_script_t;
]])

local function get_script_args(func_name)
    local script_ent = spectre.scripts.find_script(world, func_name)
    if script_ent == 0 then return {} end
    local ecs_type = ffi.C.ecs_get_type(world, script_ent)
    local comp_id = 0
    if ecs_type ~= nil then
        for i = 0, tonumber(ecs_type.count) - 1 do
            local id = ecs_type.array[i]
            local name = ffi.C.ecs_get_name(world, id)
            if name ~= nil and ffi.string(name) == "Script" then
                comp_id = id
                break
            end
        end
    end
    if comp_id == 0 then return {} end
    local ptr = ffi.C.ecs_get_id(world, script_ent, comp_id)
    if ptr == nil then return {} end
    local script_data = ffi.cast("const spectre_script_t*", ptr)
    local args = {}
    for i = 0, tonumber(script_data.argument_count) - 1 do
        table.insert(args, ffi.string(script_data.arguments_name[i]))
    end
    return args
end

Scripts = {}
_G.modules = _G.modules or {}
_G.modules["Scripts"] = Scripts

function Scripts.on_enter() end
function Scripts.on_update() end
function Scripts.on_exit() end

_G.modules["Prefabs"] = _G.modules["Prefabs"] or { Drawers = {} }
local Drawers = _G.modules["Prefabs"].Drawers

local add_script_popup = false
local add_script_target_list = ""
local selected_script_idx = 0

Drawers["scripts"] = function(props, path)
    local p = path .. "/components/scripts"
    local modified = false
    
    local lists = {"on_create", "on_update", "on_destroy", "on_enter", "on_exit", "on_render"}
    for _, list_name in ipairs(lists) do
        local list_path = p .. "/" .. list_name
        
        if imgui.TreeNodeEx(list_name, 0) then
            if imgui.BeginPopupContextItem("Context_" .. list_name) then
                if imgui.MenuItem("Add Script") then
                    add_script_popup = true
                    add_script_target_list = list_path
                end
                imgui.EndPopup()
            end
            
            local keys = props:keys(list_path) or {}
            for _, key in ipairs(keys) do
                local script_path = list_path .. "/" .. key
                local func_name = props:read_string(script_path .. "/function") or ""
                imgui.PushID_Str(list_name .. "_" .. key)
                if imgui.TreeNodeEx(func_name ~= "" and func_name or "Unknown", 0) then
                    if imgui.BeginPopupContextItem("Context_" .. key) then
                        if imgui.MenuItem("Remove") then
                            props:clear(script_path)
                            modified = true
                        end
                        if imgui.MenuItem("Duplicate") then
                            local dumped = props:sub(script_path):dump(0)
                            if dumped then
                                local max_k = 0
                                for _, k in ipairs(props:keys(list_path) or {}) do
                                    local n = tonumber(k)
                                    if n and n >= max_k then max_k = n + 1 end
                                end
                                local new_json = string.format('{"%d": %s}', max_k, dumped)
                                props:sub(list_path):load(new_json, 0)
                                modified = true
                            end
                        end
                        imgui.EndPopup()
                    end
                    
                    local args = get_script_args(func_name)
                    for _, arg_name in ipairs(args) do
                        local arg_val = props:read_string(script_path .. "/arguments/" .. arg_name) or ""
                        local buf = ffi.new("char[256]")
                        ffi.copy(buf, arg_val)
                        if imgui.InputText(arg_name, buf, 256) then
                            props:set_string(script_path .. "/arguments/" .. arg_name, ffi.string(buf))
                            modified = true
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
                    add_script_target_list = list_path
                end
                imgui.EndPopup()
            end
        end
    end
    
    if add_script_popup then
        imgui.OpenPopup("Add Script")
    end
    if imgui.BeginPopupModal("Add Script", nil, 64) then
        add_script_popup = false
        local all_scripts = spectre.scripts.list_scripts(world)
        local script_names = {}
        for _, id in ipairs(all_scripts) do
            local name = ffi.C.ecs_get_name(world, id)
            if name ~= nil then
                table.insert(script_names, ffi.string(name))
            end
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
            local args = get_script_args(selected_name)
            imgui.Text("Arguments:")
            for _, arg_name in ipairs(args) do
                imgui.Text("- " .. arg_name)
            end
            
            if imgui.Button("Add") then
                local max_k = 0
                for _, k in ipairs(props:keys(add_script_target_list) or {}) do
                    local n = tonumber(k)
                    if n and n >= max_k then max_k = n + 1 end
                end
                
                local args_json = {}
                for _, arg_name in ipairs(args) do
                    table.insert(args_json, string.format('"%s": ""', arg_name))
                end
                local args_str = "{" .. table.concat(args_json, ",") .. "}"
                
                local new_json = string.format('{"%d": {"function": "%s", "arguments": %s}}', max_k, selected_name, args_str)
                local list_name = string.match(add_script_target_list, "([^/]+)$")
                local wrap_json = string.format('{"%s": %s}', list_name, new_json)
                props:sub(p):load(wrap_json, 0)
                modified = true
                imgui.CloseCurrentPopup()
                selected_script_idx = 0
            end
            imgui.SameLine()
        end
        
        if imgui.Button("Cancel") then
            imgui.CloseCurrentPopup()
            selected_script_idx = 0
        end
        imgui.EndPopup()
    end
    
    return modified
end

return {}
