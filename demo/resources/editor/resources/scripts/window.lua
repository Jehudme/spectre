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
edit = {}
_G.modules = {}

require("modules")

local projects_list = nil
local show_new_project_popup = false
local new_project_name = ffi.new("char[256]")
local show_rename_popup = false
local rename_target = nil
local new_rename_name = ffi.new("char[256]")

function styling_ui()
	local style = imgui.GetStyle()
	-- Main Window styling
	style.Colors[2] = ffi.new("ImVec4", 0.1, 0.1, 0.12, 1.0) -- ImGuiCol_WindowBg
	style.Colors[7] = ffi.new("ImVec4", 0.12, 0.12, 0.14, 1.0) -- ImGuiCol_FrameBg
	style.Colors[10] = ffi.new("ImVec4", 0.2, 0.2, 0.25, 1.0) -- ImGuiCol_TitleBg
	style.Colors[11] = ffi.new("ImVec4", 0.25, 0.25, 0.35, 1.0) -- ImGuiCol_TitleBgActive
	style.Colors[22] = ffi.new("ImVec4", 0.2, 0.6, 0.3, 1.0) -- ImGuiCol_Button
	style.Colors[23] = ffi.new("ImVec4", 0.3, 0.7, 0.4, 1.0) -- ImGuiCol_ButtonHovered
	style.WindowPadding = ffi.new("ImVec2", 20, 20)
end

function menu.on_render(self_id, scene_id, state_id)
	sandbox.logs.info(world, "[window.lua] on_render_menu called, ImGui is drawing!")
	
	if not projects_list then
		projects_list = project.list()
	end

	local screen_w = spectre.window.get_width(world)
	local screen_h = spectre.window.get_height(world)
	imgui.SetNextWindowPos(ffi.new("ImVec2", 0, 0), 1) -- ImGuiCond_Always
	imgui.SetNextWindowSize(ffi.new("ImVec2", screen_w, screen_h), 1) -- ImGuiCond_Always

	local window_flags = bit.bor(1, 32, 2, 4, 8192, 524288) -- NoTitleBar, NoCollapse, NoResize, NoMove, NoBringToFrontOnFocus, NoNavFocus

	imgui.Begin("Project Manager##Main", nil, window_flags)

	-- Center inner window
	imgui.SetNextWindowPos(ffi.new("ImVec2", screen_w * 0.5, screen_h * 0.5), 1, ffi.new("ImVec2", 0.5, 0.5)) -- ImGuiCond_Always
	imgui.SetNextWindowSize(ffi.new("ImVec2", 700, 500), 4) -- ImGuiCond_FirstUseEver

	imgui.Begin("Spectre Editor - Project Hub", nil, 32) -- ImGuiWindowFlags_NoCollapse

	imgui.Text("Welcome to Spectre Editor")
	imgui.Separator()
	imgui.Spacing()

	-- Toolbar
	if imgui.Button("  + New Project  ", ffi.new("ImVec2", 150, 40)) then
		show_new_project_popup = true
		ffi.copy(new_project_name, "MyProject")
	end

	imgui.Spacing()
	imgui.Spacing()

	imgui.TextColored(ffi.new("ImVec4", 0.6, 0.6, 0.6, 1.0), "Recent Projects")

	-- Project List
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
					show_rename_popup = true
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

	imgui.End() -- End Spectre Editor inner window

	if show_new_project_popup then
		imgui.OpenPopup("Create New Project")
		show_new_project_popup = false
	end

	if show_rename_popup then
		imgui.OpenPopup("Rename Project")
		show_rename_popup = false
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
end

local current_module = nil

function edit.on_render()
	sandbox.logs.info(world, "[window.lua] on_render_edit called, Editor ImGui is drawing!")
	if imgui.BeginMainMenuBar() then
		if imgui.BeginMenu("File") then
			if imgui.MenuItem("Save") then
				sandbox.logs.info(world, "[Menu] File -> Save clicked")
			end
			if imgui.MenuItem("Exit") then
				spectre.window.set_should_close(world, true)
			end
			imgui.EndMenu()
		end
		if imgui.BeginMenu("Edit") then
			if imgui.MenuItem("Undo") then
				sandbox.logs.info(world, "[Menu] Edit -> Undo clicked")
			end
			imgui.EndMenu()
		end
		
		-- Dynamic modules menu
		if imgui.BeginMenu("Modules") then
			for mod_name, mod_table in pairs(_G.modules) do
				if imgui.MenuItem(mod_name) then
					sandbox.logs.info(world, "[Menu] Module -> " .. mod_name .. " clicked")
					if current_module and current_module.on_exit then
						current_module.on_exit()
					end
					current_module = mod_table
					if current_module and current_module.on_enter then
						current_module.on_enter()
					end
				end
			end
			imgui.EndMenu()
		end

		if imgui.BeginMenu("Options") then
			if imgui.MenuItem("Settings") then
				sandbox.logs.info(world, "[Menu] Options -> Settings clicked")
			end
			imgui.EndMenu()
		end
		if imgui.BeginMenu("Help") then
			if imgui.MenuItem("About") then
				sandbox.logs.info(world, "[Menu] Help -> About clicked")
			end
			imgui.EndMenu()
		end
		imgui.EndMainMenuBar()
	end

	if current_module and current_module.on_update then
		current_module.on_update()
	end
end

return {
	on_style_imgui = ecs.Script.define(styling_ui),
	on_init_menu = ecs.Script.define(project.initialize),
	on_render_menu = ecs.Script.define(menu.on_render, "self_id:integer", "scene_id:integer", "state_id:integer"),
	on_render_edit = ecs.Script.define(edit.on_render)
}
