sandbox = require("sandbox")
local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local ffi = require("ffi")
local world = ecs.from_ptr(g_world)

projects = {
	PHYSICAL_SANDBOX_LAUNCHER_PATH = "/home/jehud/CLionProjects/spectre/cmake-build-debug/bin/sandbox_launcher",
	VIRTUAL_PROJECTS_PATH_DIRECTORY = "save://projects",

	view = {},
}

-- Inline functions --

function projects.create(project_name)
	sandbox.logs.info(world, "[projects.create] Starting to create project: " .. project_name)
	local source_path = "app://templates/new_app"
	local destination_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. project_name

	if sandbox.filesystem.exists(world, destination_path) then
		sandbox.logs.error(world, "[projects.create] Failed: Project already exists at " .. destination_path)
		return false
	end

	sandbox.filesystem.copy(world, source_path, destination_path, false, true)
	sandbox.logs.info(world, "[projects.create] Successfully created project: " .. project_name)
	return true
end

function projects.delete(project_name)
	sandbox.logs.info(world, "[projects.delete] Starting to delete project: " .. project_name)
	local target_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. project_name

	if not sandbox.filesystem.exists(world, target_path) then
		sandbox.logs.error(world, "[projects.delete] Failed: Project does not exist at " .. target_path)
		return false
	end

	sandbox.filesystem.remove_directory(world, target_path)
	sandbox.logs.info(world, "[projects.delete] Successfully deleted project: " .. project_name)
	return true
end

function projects.rename(old_name, new_name)
	sandbox.logs.info(world, "[projects.rename] Starting to rename project from " .. old_name .. " to " .. new_name)
	local source_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. old_name
	local destination_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. new_name

	if not sandbox.filesystem.exists(world, source_path) then
		sandbox.logs.error(world, "[projects.rename] Failed: Source project does not exist at " .. source_path)
		return false
	end
	if sandbox.filesystem.exists(world, destination_path) then
		sandbox.logs.error(world, "[projects.rename] Failed: Destination project already exists at " .. destination_path)
		return false
	end

	sandbox.filesystem.move(world, source_path, destination_path, false, true)
	sandbox.logs.info(world, "[projects.rename] Successfully renamed project to: " .. new_name)
	return true
end

function projects.duplicate(project_name)
	sandbox.logs.info(world, "[projects.duplicate] Starting to duplicate project: " .. project_name)
	local source_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. project_name
	local duplicate_name = project_name .. "_copy"
	local destination_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. duplicate_name

	if not sandbox.filesystem.exists(world, source_path) then
		sandbox.logs.error(world, "[projects.duplicate] Failed: Source project does not exist at " .. source_path)
		return false
	end
	
	-- Keep adding _copy if it already exists
	while sandbox.filesystem.exists(world, destination_path) do
		duplicate_name = duplicate_name .. "_copy"
		destination_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. duplicate_name
	end

	sandbox.filesystem.copy(world, source_path, destination_path, false, true)
	sandbox.logs.info(world, "[projects.duplicate] Successfully duplicated project as: " .. duplicate_name)
	return true
end

function projects.find(project_name)
	sandbox.logs.info(world, "[projects.find] Starting to find configuration for project: " .. project_name)
	local config_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. project_name .. "/configuration.json"

	if not sandbox.filesystem.exists(world, config_path) then
		sandbox.logs.error(world, "[projects.find] Failed: Configuration file does not exist at " .. config_path)
		return nil
	end

	local content = sandbox.filesystem.read_file_string(world, config_path)
	if not content then
		sandbox.logs.error(world, "[projects.find] Failed: Could not read string content from " .. config_path)
		return nil
	end

	local properties_handle = sandbox.Properties.new()
	properties_handle:load(content, sandbox.PropertiesFormat.JSON)
	sandbox.logs.info(world, "[projects.find] Successfully loaded configuration for project: " .. project_name)
	
	return properties_handle
end

function projects.import(project_name, physical_path)
	sandbox.logs.info(world, "[projects.import] Dummy operation called for project: " .. project_name .. " from " .. physical_path)
	-- TODO: Use the filesystem to copy the project from physical_path into "save://projects/<project_name>"
	return true
end

function projects.export(project_name, physical_path)
	sandbox.logs.info(world, "[projects.export] Dummy operation called for project: " .. project_name .. " to " .. physical_path)
	-- TODO: Use the filesystem to copy the project from "save://projects/<project_name>" into physical_path
	return true
end

function projects.list(sort_type)
	sandbox.logs.info(world, "[projects.list] Listing all projects in " .. projects.VIRTUAL_PROJECTS_PATH_DIRECTORY)
	local files = sandbox.filesystem.list_files(world, projects.VIRTUAL_PROJECTS_PATH_DIRECTORY, false)
	local merged_properties = sandbox.Properties.new()

	for index, file_path in ipairs(files) do
		if sandbox.filesystem.is_directory(world, file_path) then
			-- Extract folder name from path
			local folder_name = file_path:match("([^/]+)$") or file_path
			sandbox.logs.info(world, "[projects.list] Found project directory: " .. folder_name)
			
			local project_properties = projects.find(folder_name)
			if project_properties then
				-- Merge into our main properties object under its own name
				-- Since merge takes another properties object, we just attach it
				merged_properties:set_string("projects/" .. folder_name .. "/path", file_path)
				project_properties:destroy()
			end
		end
	end
	
	sandbox.logs.info(world, "[projects.list] Successfully generated list of all projects.")
	return merged_properties
end

function projects.edit(project_name)
	sandbox.logs.info(world, "[projects.edit] Dummy operation called for project: " .. project_name)
	--TODO: Nothing for now
	return true
end

function projects.run(project_name)
	sandbox.logs.info(world, "[projects.run] Starting to run project: " .. project_name)
	local target_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. project_name
	
	if not sandbox.filesystem.exists(world, target_path) then
		sandbox.logs.error(world, "[projects.run] Failed: Project does not exist at " .. target_path)
		return false
	end
	
	local run_command = string.format("%s %s &", projects.PHYSICAL_SANDBOX_LAUNCHER_PATH, target_path)
	sandbox.logs.info(world, "[projects.run] Executing command: " .. run_command)
	os.execute(run_command)
	return true
end

-- Interface --

projects.view.cached_projects = nil
projects.view.selected_project = nil
projects.view.rename_popup_open = false
projects.view.rename_buffer = ""

local function refresh_projects_list()
	if projects.view.cached_projects then
		projects.view.cached_projects:destroy()
	end
	projects.view.cached_projects = projects.list()
end

function projects.view.on_enter()
	projects.test_all()
	refresh_projects_list()
end

function projects.view.on_render()
	local viewport = imgui.GetMainViewport()
	local window_flags = bit.bor(
		imgui.WindowFlags.NoTitleBar,
		imgui.WindowFlags.NoResize,
		imgui.WindowFlags.NoMove,
		imgui.WindowFlags.NoCollapse,
		imgui.WindowFlags.NoSavedSettings
	)
	
	-- Center the window
	local window_size = imgui.ImVec2(800, 600)
	local center_pos = imgui.ImVec2(
		viewport.Pos.x + (viewport.Size.x - window_size.x) * 0.5,
		viewport.Pos.y + (viewport.Size.y - window_size.y) * 0.5
	)
	
	imgui.SetNextWindowPos(center_pos, imgui.Cond.Always)
	imgui.SetNextWindowSize(window_size, imgui.Cond.Always)
	
	if imgui.Begin("Projects Browser", nil, window_flags) then
		imgui.Text("Project Manager")
		imgui.Separator()
		imgui.Spacing()
		
		-- Top buttons
		if imgui.Button("New Project", imgui.ImVec2(150, 30)) then
			-- Generate a default name
			local new_name = "New_Project"
			local counter = 1
			while sandbox.filesystem.exists(world, projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. new_name) do
				new_name = "New_Project_" .. tostring(counter)
				counter = counter + 1
			end
			projects.create(new_name)
			refresh_projects_list()
		end
		
		imgui.SameLine()
		
		if imgui.Button("Import Project", imgui.ImVec2(150, 30)) then
			-- Dummy import for now
			projects.import("Imported_Project", "/dummy/path")
			refresh_projects_list()
		end
		
		imgui.Spacing()
		imgui.Separator()
		imgui.Spacing()
		
		-- Project List Box
		if imgui.BeginChild("ProjectList", imgui.ImVec2(0, -imgui.GetFrameHeightWithSpacing()), true, 0) then
			if projects.view.cached_projects then
				local project_keys = projects.view.cached_projects:keys("projects")
				
				for i, proj_name in ipairs(project_keys) do
					local is_selected = (projects.view.selected_project == proj_name)
					
					if imgui.Selectable(proj_name, is_selected, imgui.SelectableFlags.AllowDoubleClick) then
						projects.view.selected_project = proj_name
						
						if imgui.IsMouseDoubleClicked(imgui.MouseButton.Left) then
							projects.run(proj_name)
						end
					end
					
					-- Context Menu on Right Click
					if imgui.BeginPopupContextItem("context_" .. proj_name) then
						projects.view.selected_project = proj_name
						imgui.TextDisabled("Actions for " .. proj_name)
						imgui.Separator()
						
						if imgui.MenuItem("Run") then
							projects.run(proj_name)
						end
						if imgui.MenuItem("Edit") then
							projects.edit(proj_name)
						end
						if imgui.MenuItem("Rename") then
							projects.view.rename_popup_open = true
							projects.view.rename_buffer = proj_name
						end
						if imgui.MenuItem("Duplicate") then
							projects.duplicate(proj_name)
							refresh_projects_list()
						end
						if imgui.MenuItem("Export") then
							projects.export(proj_name, "/dummy/export/path")
						end
						
						imgui.Separator()
						
						if imgui.MenuItem("Delete", nil, false, true) then
							projects.delete(proj_name)
							if projects.view.selected_project == proj_name then
								projects.view.selected_project = nil
							end
							refresh_projects_list()
						end
						
						imgui.EndPopup()
					end
				end
			else
				imgui.TextDisabled("No projects found.")
			end
			imgui.EndChild()
		end
		
		-- Rename Popup Handling
		if projects.view.rename_popup_open then
			imgui.OpenPopup("Rename Project")
		end
		
		if imgui.BeginPopupModal("Rename Project", nil, imgui.WindowFlags.AlwaysAutoResize) then
			imgui.Text("Enter new name for " .. projects.view.selected_project .. ":")
			local changed, new_buf = imgui.InputText("##newname", projects.view.rename_buffer, 256)
			if changed then
				projects.view.rename_buffer = new_buf
			end
			
			imgui.Spacing()
			
			if imgui.Button("Rename", imgui.ImVec2(120, 0)) then
				if string.len(projects.view.rename_buffer) > 0 and projects.view.rename_buffer ~= projects.view.selected_project then
					projects.rename(projects.view.selected_project, projects.view.rename_buffer)
					refresh_projects_list()
				end
				projects.view.rename_popup_open = false
				imgui.CloseCurrentPopup()
			end
			imgui.SameLine()
			if imgui.Button("Cancel", imgui.ImVec2(120, 0)) then
				projects.view.rename_popup_open = false
				imgui.CloseCurrentPopup()
			end
			imgui.EndPopup()
		end
		
	end
	imgui.End()
end

function projects.view.on_exit()
	if projects.view.cached_projects then
		projects.view.cached_projects:destroy()
		projects.view.cached_projects = nil
	end
end

-- Tests --

function projects.test_create()
	sandbox.logs.info(world, "[Test] Starting test_create")
	local test_project = "test_project_create"
	if sandbox.filesystem.exists(world, projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. test_project) then
		projects.delete(test_project)
	end
	
	local success = projects.create(test_project)
	if not success then
		sandbox.logs.error(world, "[Test] Failed test_create")
	else
		sandbox.logs.info(world, "[Test] Passed test_create")
	end
	return success
end

function projects.test_rename()
	sandbox.logs.info(world, "[Test] Starting test_rename")
	local old_name = "test_project_create"
	local new_name = "test_project_renamed"
	
	if sandbox.filesystem.exists(world, projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. new_name) then
		projects.delete(new_name)
	end
	
	local success = projects.rename(old_name, new_name)
	if not success then
		sandbox.logs.error(world, "[Test] Failed test_rename")
	else
		sandbox.logs.info(world, "[Test] Passed test_rename")
	end
	return success
end

function projects.test_duplicate()
	sandbox.logs.info(world, "[Test] Starting test_duplicate")
	local base_name = "test_project_renamed"
	
	local success = projects.duplicate(base_name)
	if not success then
		sandbox.logs.error(world, "[Test] Failed test_duplicate")
	else
		sandbox.logs.info(world, "[Test] Passed test_duplicate")
	end
	return success
end

function projects.test_find()
	sandbox.logs.info(world, "[Test] Starting test_find")
	local target_name = "test_project_renamed"
	
	local props = projects.find(target_name)
	if not props then
		sandbox.logs.error(world, "[Test] Failed test_find")
		return false
	else
		sandbox.logs.info(world, "[Test] Passed test_find")
		props:destroy()
		return true
	end
end

function projects.test_delete()
	sandbox.logs.info(world, "[Test] Starting test_delete")
	
	local success1 = projects.delete("test_project_renamed")
	local success2 = projects.delete("test_project_renamed_copy")
	
	if not success1 or not success2 then
		sandbox.logs.error(world, "[Test] Failed test_delete")
		return false
	else
		sandbox.logs.info(world, "[Test] Passed test_delete")
		return true
	end
end

function projects.test_all()
	sandbox.logs.info(world, "[Test] ===== STARTING ALL PROJECTS TESTS =====")
	
	-- We must run these sequentially as they depend on the state from the previous test
	local all_passed = true
	all_passed = all_passed and projects.test_create()
	all_passed = all_passed and projects.test_rename()
	all_passed = all_passed and projects.test_duplicate()
	all_passed = all_passed and projects.test_find()
	all_passed = all_passed and projects.test_delete()
	
	if all_passed then
		sandbox.logs.info(world, "[Test] ===== ALL PROJECTS TESTS PASSED =====")
	else
		sandbox.logs.error(world, "[Test] ===== SOME PROJECTS TESTS FAILED =====")
	end
end

return {
	-- TODO: Attach this function to be run on_enter of Menu scene
	on_enter_projects_view = ecs.Script.define(projects.view.on_enter()),

	-- TODO: Attach this function to be run on_update of menu scene
	on_render_projects_view = ecs.Script.define(projects.view.on_render()),

	-- TODO: Attach this function to be run on_exit of on_exit of menu scene,
	on_exit_projects_view = ecs.Script.define(projects.view.on_exit()),

	run_all_projects_tests = ecs.Script.define(projects.test_all),
}
