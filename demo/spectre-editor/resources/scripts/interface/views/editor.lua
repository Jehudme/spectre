local sandbox = require("sandbox")
local ecs = require("ecs")
local spectre = require("spectre")
local imgui = require("imgui")
local ffi = require("ffi")
local history = require("utilities.history")
local pages = require("utilities.pages")
local is_initialized = false

editor = {
	active_project_name = nil,
	view = {}
}

function editor.view.on_enter()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[Editor] Entering Editor View")

	-- Register module pages by requiring the ported module files
	pages.register("module", "Prefabs", require("interface.pages.modules.prefabs"))
	pages.register("module", "Components", require("interface.pages.modules.components"))
	pages.register("module", "Scenes", require("interface.pages.modules.scenes"))
	pages.register("module", "Resources", require("interface.pages.modules.resources"))
	pages.register("module", "Scripts", require("interface.pages.modules.scripts"))
	pages.register("module", "Window", require("interface.pages.modules.window"))

	-- Load renderer drawers registry
	local renderer = require("interface.pages.modules.renderer")
	if renderer and renderer.on_enter then
		renderer.on_enter()
	end

	-- Register option pages
	pages.register("option", "Settings", Page.new())
	pages.register("option", "Preferences", Page.new())

	-- Register others pages
	pages.register("others", "Info", Page.new())
	pages.register("others", "Help", Page.new())

	-- Set default page
	pages.set_default("module", "Prefabs")
	pages.current_page = pages.find("module", "Prefabs")
	if pages.current_page and pages.current_page.on_enter then
		pages.current_page:on_enter()
	end
end

local FileBrowser = require("utilities.filebrowser")
local export_browser = nil

function editor.view.on_render()
	local world = ecs.from_ptr(g_world)

	if not is_initialized then
		editor.view.on_enter()
		is_initialized = true
	end

	-- Handle Ctrl+Z / Ctrl+Y shortcuts globally (before menu bar)
	local io = imgui.GetIO()
	local is_ctrl = io.KeyCtrl

	if is_ctrl then
		local key_z = imgui.IsKeyPressed(ffi.C.ImGuiKey_Z, false) or imgui.IsKeyPressed(string.byte('Z'), false) or imgui.IsKeyPressed(string.byte('z'), false)
		local key_y = imgui.IsKeyPressed(ffi.C.ImGuiKey_Y, false) or imgui.IsKeyPressed(string.byte('Y'), false) or imgui.IsKeyPressed(string.byte('y'), false)

		if key_z then
			sandbox.logs.info(world, "[Editor] Ctrl+Z: Undo")
			history.undo()
			local cp = pages.get_current()
			if cp and cp.on_enter then cp:on_enter() end
		elseif key_y then
			sandbox.logs.info(world, "[Editor] Ctrl+Y: Redo")
			history.redo()
			local cp = pages.get_current()
			if cp and cp.on_enter then cp:on_enter() end
		end
	end

	local trigger_export = false

	-- 1. Render Top Main Menu Bar
	if imgui.BeginMainMenuBar() then
		-- File Menu
		if imgui.BeginMenu("File") then
			if imgui.MenuItem("Run", "F5") then
				sandbox.logs.info(world, "[Editor] Menu item clicked: File -> Run")
				if editor.active_project_name then
					projects.run(editor.active_project_name)
				else
					sandbox.logs.error(world, "[Editor] No active project loaded to run.")
				end
			end

			if imgui.MenuItem("Export...") then
				sandbox.logs.info(world, "[Editor] Menu item clicked: File -> Export")
				if editor.active_project_name then
					trigger_export = true
				else
					sandbox.logs.error(world, "[Editor] No active project loaded to export.")
				end
			end

			imgui.Separator()

			if imgui.MenuItem("Exit", "Alt+F4") then
				sandbox.logs.info(world, "[Editor] Menu item clicked: File -> Exit")
				local startup_state = spectre.scenes.find_state(world, "Startup")
				if startup_state and startup_state ~= 0 then
					spectre.scenes.push_state(world, startup_state)
				else
					sandbox.logs.error(world, "[Editor] Failed to transition to Startup state.")
				end
			end
			imgui.EndMenu()
		end

		-- Edit Menu
		if imgui.BeginMenu("Edit") then
			local can_undo = history.actions_index > 0
			local can_redo = history.actions_index < #history.actions_stack

			-- Debug log to figure out why they are disabled!
			sandbox.logs.info(world, "[Editor] Edit Menu Opened! actions_index: " .. tostring(history.actions_index) .. " stack size: " .. tostring(#history.actions_stack))

			-- Show the name of the action that would be undone/redone
			local undo_label = "Undo"
			if can_undo then
				local action = history.actions_stack[history.actions_index]
				if action and action.action_name then
					undo_label = "Undo: " .. action.action_name
				end
			end
			local redo_label = "Redo"
			if can_redo then
				local action = history.actions_stack[history.actions_index + 1]
				if action and action.action_name then
					redo_label = "Redo: " .. action.action_name
				end
			end

			if imgui.MenuItem(undo_label, "Ctrl+Z", false, can_undo) then
				sandbox.logs.info(world, "[Editor] Menu item clicked: Edit -> Undo")
				history.undo()
				local cp = pages.get_current()
				if cp and cp.on_enter then cp:on_enter() end
			end

			if imgui.MenuItem(redo_label, "Ctrl+Y", false, can_redo) then
				sandbox.logs.info(world, "[Editor] Menu item clicked: Edit -> Redo")
				history.redo()
				local cp = pages.get_current()
				if cp and cp.on_enter then cp:on_enter() end
			end
			imgui.EndMenu()
		end

		-- Info Menu (direct switch)
		if imgui.MenuItem("Info") then
			sandbox.logs.info(world, "[Editor] Menu item clicked: Info")
			pages.actions.switch_page("others", "Info")
		end

		-- Modules Menu
		if imgui.BeginMenu("Modules") then
			local module_pages = pages.list_all("module")
			for _, pg in ipairs(module_pages) do
				local is_selected = (pages.get_current() == pg)
				if imgui.MenuItem(pg.name, nil, is_selected) then
					sandbox.logs.info(world, "[Editor] Menu item clicked: Modules -> " .. pg.name)
					pages.actions.switch_page("module", pg.name)
				end
			end
			imgui.EndMenu()
		end

		-- Options Menu
		if imgui.BeginMenu("Options") then
			local option_pages = pages.list_all("option")
			for _, pg in ipairs(option_pages) do
				local is_selected = (pages.get_current() == pg)
				if imgui.MenuItem(pg.name, nil, is_selected) then
					sandbox.logs.info(world, "[Editor] Menu item clicked: Options -> " .. pg.name)
					pages.actions.switch_page("option", pg.name)
				end
			end
			imgui.EndMenu()
		end

		-- Help Menu (direct switch)
		if imgui.MenuItem("Help") then
			sandbox.logs.info(world, "[Editor] Menu item clicked: Help")
			pages.actions.switch_page("others", "Help")
		end

		-- Show active project name in the menu bar
		if editor.active_project_name then
			imgui.Text("  |  " .. editor.active_project_name)
		end

		imgui.EndMainMenuBar()
	end

	if trigger_export then
		if not export_browser then
			export_browser = FileBrowser.new("dir", "save://")
		end
		export_browser:open(function(selected_path)
			if selected_path then
				sandbox.logs.info(world, "[Editor] Exporting to: " .. selected_path)
				projects.export(editor.active_project_name, selected_path)
			end
		end)
	end

	-- 2. Render active page below menu bar
	local current_page = pages.get_current()
	if current_page then
		local viewport_w = spectre.window.get_width(g_world)
		local viewport_h = spectre.window.get_height(g_world)
		local menu_height = 20 -- Menu bar height

		imgui.SetNextWindowPos(imgui.ImVec2(0, menu_height))
		imgui.SetNextWindowSize(imgui.ImVec2(viewport_w, viewport_h - menu_height))

		local flags = bit.bor(
			ffi.C.ImGuiWindowFlags_NoTitleBar,
			ffi.C.ImGuiWindowFlags_NoResize,
			ffi.C.ImGuiWindowFlags_NoMove,
			ffi.C.ImGuiWindowFlags_NoCollapse,
			ffi.C.ImGuiWindowFlags_NoScrollbar,
			ffi.C.ImGuiWindowFlags_NoSavedSettings
		)

		if imgui.Begin("###EditorPageContent", nil, flags) then
			if current_page.on_render then
				current_page:on_render()
			else
				-- Default placeholder page rendering
				imgui.Text("Active Page: " .. (current_page.name or "Unnamed"))
				imgui.Text("Category: " .. (current_page.type or "Unknown"))
				imgui.Separator()
				imgui.Text("This page is currently empty.")
			end
			imgui.End()
		end
	end

	-- 3. Export FileBrowser modal
	if export_browser and export_browser.is_open then
		export_browser:render()
	end

	-- 4. Undo/Redo toast notification overlay
	if history.notification then
		local notif = history.notification
		-- Tick down timer (use a fixed dt estimate)
		notif.timer = notif.timer - (1.0 / 60.0)
		if notif.timer <= 0 then
			history.notification = nil
		else
			-- Render a small overlay in the bottom-right corner
			local viewport_w = spectre.window.get_width(g_world)
			local viewport_h = spectre.window.get_height(g_world)
			local pad = 16
			local toast_w = 300
			local toast_h = 36

			local alpha = math.min(1.0, notif.timer / 0.4) -- fade in/out
			if notif.timer < 0.6 then
				alpha = notif.timer / 0.6
			end

			imgui.SetNextWindowPos(imgui.ImVec2(viewport_w - toast_w - pad, viewport_h - toast_h - pad))
			imgui.SetNextWindowSize(imgui.ImVec2(toast_w, toast_h))
			imgui.SetNextWindowBgAlpha(alpha * 0.85)

			local toast_flags = bit.bor(
				ffi.C.ImGuiWindowFlags_NoTitleBar,
				ffi.C.ImGuiWindowFlags_NoResize,
				ffi.C.ImGuiWindowFlags_NoMove,
				ffi.C.ImGuiWindowFlags_NoInputs,
				ffi.C.ImGuiWindowFlags_NoScrollbar,
				ffi.C.ImGuiWindowFlags_NoSavedSettings,
				ffi.C.ImGuiWindowFlags_NoBringToDisplayOnFocus
			)

			if imgui.Begin("###UndoRedoToast", nil, toast_flags) then
				local icon = notif.kind == "undo" and "<" or ">"
				imgui.Text(icon .. " " .. notif.text)
				imgui.End()
			end
		end
	end
end

function editor.view.on_exit()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[Editor] Exiting Editor View")
	if pages.current_page and pages.current_page.on_exit then
		pages.current_page:on_exit()
	end
	is_initialized = false
end

return {
	on_enter_editor_view = ecs.Script.define(editor.view.on_enter),
	on_render_editor_view = ecs.Script.define(editor.view.on_render),
	on_exit_editor_view = ecs.Script.define(editor.view.on_exit),
}
