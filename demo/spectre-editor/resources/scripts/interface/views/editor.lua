local sandbox = require("sandbox")
local ecs = require("ecs")
local spectre = require("spectre")
local imgui = require("imgui")
local ffi = require("ffi")
local history = require("utilities.history")
local pages = require("utilities.pages")

editor = {
	active_project_name = nil,
	view = {}
}

function editor.view.on_enter()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[Editor] Entering Editor View")

	-- Register module pages
	pages.register("module", "Prefabs", Page.new())
	pages.register("module", "Components", Page.new())
	pages.register("module", "Scenes", Page.new())
	pages.register("module", "Resources", Page.new())
	pages.register("module", "Scripts", Page.new())
	pages.register("module", "Window", Page.new())

	-- Register option pages
	pages.register("option", "Settings", Page.new())
	pages.register("option", "Preferences", Page.new())

	-- Register others pages
	pages.register("others", "Info", Page.new())
	pages.register("others", "Help", Page.new())

	-- Set default page
	pages.set_default("module", "Prefabs")
	pages.current_page = pages.find("module", "Prefabs")
end

function editor.view.on_render()
	local world = ecs.from_ptr(g_world)

	-- 1. Render Top Main Menu Bar
	if imgui.BeginMainMenuBar() then
		-- File Menu
		if imgui.BeginMenu("File") then
			if imgui.MenuItem("Run") then
				sandbox.logs.info(world, "[Editor] Menu item clicked: File -> Run")
				if editor.active_project_name then
					projects.run(editor.active_project_name)
				else
					sandbox.logs.error(world, "[Editor] No active project loaded to run.")
				end
			end

			if imgui.MenuItem("Export") then
				sandbox.logs.info(world, "[Editor] Menu item clicked: File -> Export")
				if editor.active_project_name then
					sandbox.logs.info(world, "[Editor] Triggering project export: " .. editor.active_project_name)
					projects.export(editor.active_project_name, "save://projects_export_test")
				else
					sandbox.logs.error(world, "[Editor] No active project loaded to export.")
				end
			end

			if imgui.MenuItem("Exit") then
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
			if imgui.MenuItem("Undo") then
				sandbox.logs.info(world, "[Editor] Menu item clicked: Edit -> Undo")
				history.undo()
			end

			if imgui.MenuItem("Redo") then
				sandbox.logs.info(world, "[Editor] Menu item clicked: Edit -> Redo")
				history.redo()
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

		imgui.EndMainMenuBar()
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
end

function editor.view.on_exit()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[Editor] Exiting Editor View")
end

return {
	on_enter_editor_view = ecs.Script.define(editor.view.on_enter),
	on_render_editor_view = ecs.Script.define(editor.view.on_render),
	on_exit_editor_view = ecs.Script.define(editor.view.on_exit),
}
