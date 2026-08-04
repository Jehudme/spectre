local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local ffi = require("ffi")
local available_scripts_cache = nil

local function get_available_scripts()
    if available_scripts_cache then return available_scripts_cache end
    
    local files = sandbox.filesystem.list_files(world, "projects://resources/scripts", true)
    local scripts_info = {}
    
    if not files then return scripts_info end
    
    for _, file in ipairs(files) do
        if file:match("%.lua$") then
            local path = "projects://resources/scripts/" .. file
            local out_data = ffi.new("uint8_t*[1]")
            local out_size = ffi.new("size_t[1]")
            if sandbox.filesystem.read_all_bytes(world, path, out_data, out_size) then
                local code = ffi.string(out_data[0], out_size[0])
                sandbox.filesystem.free_bytes(world, out_data[0])
                
                for func_name, args_str in code:gmatch("([%w_]+)%s*=%s*ecs%.Script%.define%s*%(%s*[^,%)]+([^%)]*)%)") do
                    local args = {}
                    for arg_def in args_str:gmatch('"(.-)"') do
                        local name, type = arg_def:match("^([%w_]+):([%w_]+)$")
                        if name then
                            table.insert(args, name)
                        end
                    end
                    scripts_info[func_name] = args
                end
            end
        end
    end
    
    available_scripts_cache = scripts_info
    return scripts_info
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
    
    if imgui.Button("Refresh Available Scripts") then
        available_scripts_cache = nil
    end
    
    for _, list_name in ipairs(lists) do
        local list_path = p .. "/" .. list_name
        
        imgui.Separator()
        if imgui.TreeNodeEx(list_name, 34) then -- ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen
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
                                local new_json = string.format('{"scripts": {"%s": {"%d": %s}}}', list_name, max_k, dumped)
                                props:sub(path .. "/components"):load(new_json, 0)
                                modified = true
                            end
                        end
                        imgui.EndPopup()
                    end
                    
                    local scripts_info = get_available_scripts()
                    local args = scripts_info[func_name] or {}
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
                    add_script_target_list = list_name
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
        local scripts_info = get_available_scripts()
        local script_names = {}
        for func_name, _ in pairs(scripts_info) do
            table.insert(script_names, func_name)
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
            for _, arg_name in ipairs(args) do
                imgui.Text("- " .. arg_name)
            end
            
            if imgui.Button("Add") then
                local list_path = p .. "/" .. add_script_target_list
                local max_k = 0
                for _, k in ipairs(props:keys(list_path) or {}) do
                    local n = tonumber(k)
                    if n and n >= max_k then max_k = n + 1 end
                end
                
                local args_json = {}
                for _, arg_name in ipairs(args) do
                    table.insert(args_json, string.format('"%s": ""', arg_name))
                end
                local args_str = "{" .. table.concat(args_json, ",") .. "}"
                
                local new_json = string.format('{"scripts": {"%s": {"%d": {"function": "%s", "arguments": %s}}}}', add_script_target_list, max_k, selected_name, args_str)
                props:sub(path .. "/components"):load(new_json, 0)
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
