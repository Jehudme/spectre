local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local ffi = require("ffi")

local imgui = require("imgui")
local project = require("utilities.project")

local world = ecs.from_ptr(g_world)

-- Initialize spectre components
spectre.init(world)

menu = {}

local projects_list = nil
local show_new_project_popup = false
local new_project_name = ffi.new("char[256]")
local rename_target = nil
local new_rename_name = ffi.new("char[256]")

function menu.on_render(self_id, scene_id, state_id)
    if not projects_list then
        projects_list = project.list()
    end

    local screen_w = spectre.window.get_width(world)
    local screen_h = spectre.window.get_height(world)
    imgui.SetNextWindowPos(ffi.new("ImVec2", 0, 0), 1) -- ImGuiCond_Always
    imgui.SetNextWindowSize(ffi.new("ImVec2", screen_w, screen_h), 1) -- ImGuiCond_Always
    
    -- Dark background for the main window
    imgui.PushStyleColor(2, ffi.new("ImVec4", 0.1, 0.1, 0.12, 1.0)) -- ImGuiCol_WindowBg
    local window_flags = bit.bor(1, 32, 2, 4, 8192, 524288) -- NoTitleBar, NoCollapse, NoResize, NoMove, NoBringToFrontOnFocus, NoNavFocus
    
    imgui.PushStyleVar(2, ffi.new("ImVec2", 0, 0)) -- ImGuiStyleVar_WindowPadding
    imgui.Begin("Project Manager##Main", nil, window_flags)
    imgui.PopStyleVar()
    
    -- Center inner window
    imgui.SetNextWindowPos(ffi.new("ImVec2", screen_w * 0.5, screen_h * 0.5), 1, ffi.new("ImVec2", 0.5, 0.5)) -- ImGuiCond_Always
    imgui.SetNextWindowSize(ffi.new("ImVec2", 700, 500), 4) -- ImGuiCond_FirstUseEver
    
    -- Inner window style
    imgui.PushStyleColor(2, ffi.new("ImVec4", 0.15, 0.15, 0.18, 1.0)) -- ImGuiCol_WindowBg
    imgui.PushStyleColor(10, ffi.new("ImVec4", 0.2, 0.2, 0.25, 1.0)) -- ImGuiCol_TitleBg
    imgui.PushStyleColor(11, ffi.new("ImVec4", 0.25, 0.25, 0.35, 1.0)) -- ImGuiCol_TitleBgActive
    imgui.PushStyleVar(2, ffi.new("ImVec2", 20, 20)) -- ImGuiStyleVar_WindowPadding
    
    imgui.Begin("Spectre Editor - Project Hub", nil, 32) -- ImGuiWindowFlags_NoCollapse
    
    imgui.Text("Welcome to Spectre Editor")
    imgui.Separator()
    imgui.Spacing()
    
    -- Toolbar
    imgui.PushStyleColor(22, ffi.new("ImVec4", 0.2, 0.6, 0.3, 1.0)) -- ImGuiCol_Button
    imgui.PushStyleColor(23, ffi.new("ImVec4", 0.3, 0.7, 0.4, 1.0)) -- ImGuiCol_ButtonHovered
    if imgui.Button("  + New Project  ", ffi.new("ImVec2", 150, 40)) then
        show_new_project_popup = true
        ffi.copy(new_project_name, "MyProject")
    end
    imgui.PopStyleColor(2)
    
    imgui.Spacing()
    imgui.Spacing()
    
    imgui.TextColored(ffi.new("ImVec4", 0.6, 0.6, 0.6, 1.0), "Recent Projects")
    
    -- Project List
    imgui.PushStyleColor(7, ffi.new("ImVec4", 0.12, 0.12, 0.14, 1.0)) -- ImGuiCol_FrameBg
    if imgui.BeginListBox("##Projects", ffi.new("ImVec2", -1, -1)) then
        for i, proj_name in ipairs(projects_list) do
            imgui.PushID_Str(proj_name)
            imgui.Spacing()
            
            if imgui.Selectable("   " .. proj_name .. "   ", false, 4) then -- ImGuiSelectableFlags_AllowDoubleClick
                if imgui.IsMouseDoubleClicked(0) then
                    project.mount(proj_name)
                    local edition_state = spectre.scenes.find_state(world, "Edition")
                    if edition_state ~= 0 then
                        spectre.scenes.push_state(world, edition_state)
                    end
                end
            end
            
            if imgui.IsItemHovered() then
                imgui.SetTooltip("Double-click to open, Right-click for options")
            end
            
            -- Context Menu
            if imgui.BeginPopupContextItem("Context") then
                imgui.TextColored(ffi.new("ImVec4", 0.5, 0.5, 0.5, 1.0), proj_name)
                imgui.Separator()
                if imgui.MenuItem("Open") then
                    project.mount(proj_name)
                    local edition_state = spectre.scenes.find_state(world, "Edition")
                    if edition_state ~= 0 then
                        spectre.scenes.push_state(world, edition_state)
                    end
                end
                if imgui.MenuItem("Rename") then
                    rename_target = proj_name
                    ffi.copy(new_rename_name, proj_name)
                end
                if imgui.MenuItem("Duplicate") then
                    project.duplicate(proj_name)
                    projects_list = project.list()
                end
                imgui.Separator()
                imgui.PushStyleColor(0, ffi.new("ImVec4", 1.0, 0.4, 0.4, 1.0)) -- ImGuiCol_Text
                if imgui.MenuItem("Delete") then
                    project.delete(proj_name)
                    projects_list = project.list()
                end
                imgui.PopStyleColor()
                imgui.EndPopup()
            end
            
            imgui.Spacing()
            imgui.PopID()
        end
        imgui.EndListBox()
    end
    imgui.PopStyleColor() -- FrameBg
    
    imgui.End() -- End Spectre Editor inner window
    imgui.PopStyleVar()
    imgui.PopStyleColor(3)
    
    if show_new_project_popup then
        imgui.OpenPopup("Create New Project")
    end
    
    if rename_target then
        imgui.OpenPopup("Rename Project")
    end
    
    if imgui.BeginPopupModal("Create New Project", nil, 64) then -- ImGuiWindowFlags_AlwaysAutoResize
        imgui.InputText("Name", new_project_name, 256)
        if imgui.Button("Create", ffi.new("ImVec2", 120, 0)) then
            local name_str = ffi.string(new_project_name)
            project.create(name_str)
            projects_list = project.list()
            show_new_project_popup = false
            imgui.CloseCurrentPopup()
        end
        imgui.SameLine()
        if imgui.Button("Cancel", ffi.new("ImVec2", 120, 0)) then
            show_new_project_popup = false
            imgui.CloseCurrentPopup()
        end
        imgui.EndPopup()
    end
    
    if imgui.BeginPopupModal("Rename Project", nil, 64) then -- ImGuiWindowFlags_AlwaysAutoResize
        imgui.InputText("New Name", new_rename_name, 256)
        if imgui.Button("Rename", ffi.new("ImVec2", 120, 0)) then
            local name_str = ffi.string(new_rename_name)
            project.rename(rename_target, name_str)
            projects_list = project.list()
            rename_target = nil
            imgui.CloseCurrentPopup()
        end
        imgui.SameLine()
        if imgui.Button("Cancel", ffi.new("ImVec2", 120, 0)) then
            rename_target = nil
            imgui.CloseCurrentPopup()
        end
        imgui.EndPopup()
    end
    
    imgui.End() -- End Main background window
    imgui.PopStyleColor()
end

return {
	on_init_menu = ecs.Script.define(project.initialize, "self_id:integer", "scene_id:integer", "state_id:integer"),
	on_render_menu = ecs.Script.define(menu.on_render, "self_id:integer", "scene_id:integer", "state_id:integer")
}
