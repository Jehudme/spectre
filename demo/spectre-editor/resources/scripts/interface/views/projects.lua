sandbox = require("sandbox")
local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local ffi = require("ffi")
local FileBrowser = require("utilities.filebrowser")
local world = ecs.from_ptr(g_world)

projects = {
	PHYSICAL_SANDBOX_LAUNCHER_PATH = "/home/jehud/CLionProjects/spectre/cmake-build-debug/bin/sandbox_launcher",
	VIRTUAL_PROJECTS_PATH_DIRECTORY = "save://projects",
	VIRTUAL_TEMPLATES_PATH = "app://templates/new_app",

	view = {
		import_browser = FileBrowser.new("both"),
		export_browser = FileBrowser.new("directory", "os://", true)
	},
}

-- Inline functions --

function projects.create(project_name)
	sandbox.logs.info(world, "[projects.create] Starting to create project: " .. project_name)
	local source_path = "app://templates/new_app"
	local destination_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. project_name
	local target_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. project_name

	if sandbox.filesystem.exists(world, target_path) then
		sandbox.logs.error(world, "[projects.create] Failed: Project already exists at " .. target_path)
		return false
	end

	if not sandbox.filesystem.exists(world, projects.VIRTUAL_TEMPLATES_PATH) then
		sandbox.logs.error(world, "[projects.create] Template does not exist: " .. projects.VIRTUAL_TEMPLATES_PATH)
		return false
	end
	local success = sandbox.filesystem.copy(world, projects.VIRTUAL_TEMPLATES_PATH, target_path, false, true)
	local exists_after = sandbox.filesystem.exists(world, target_path)
	if not success or not exists_after then
		sandbox.logs.error(world, string.format("[projects.create] Failed. success=%s, exists_after=%s", tostring(success), tostring(exists_after)))
		return false
	end
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

	local success = sandbox.filesystem.remove_directory(world, target_path)
	if not success or sandbox.filesystem.exists(world, target_path) then
		sandbox.logs.error(world, "[projects.delete] Failed to delete project: " .. target_path)
		return false
	end
	sandbox.logs.info(world, "[projects.delete] Successfully deleted project: " .. project_name)
	return true
end

function projects.rename(old_name, new_project_name)
	sandbox.logs.info(world, "[projects.rename] Starting to rename project from " .. old_name .. " to " .. new_project_name)
	local source_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. old_name
	local target_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. new_project_name

	if not sandbox.filesystem.exists(world, source_path) then
		sandbox.logs.error(world, "[projects.rename] Failed: Source project does not exist at " .. source_path)
		return false
	end
	if sandbox.filesystem.exists(world, target_path) then
		sandbox.logs.error(world, "[projects.rename] Failed: Destination project already exists at " .. target_path)
		return false
	end

	local success = sandbox.filesystem.move(world, source_path, target_path, false, true)
	if not success or not sandbox.filesystem.exists(world, target_path) then
		sandbox.logs.error(world, "[projects.rename] Failed to rename project to: " .. target_path)
		return false
	end
	sandbox.logs.info(world, "[projects.rename] Successfully renamed project to: " .. new_project_name)
	return true
end

function projects.duplicate(project_name)
	sandbox.logs.info(world, "[projects.duplicate] Starting to duplicate project: " .. project_name)
	local source_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. project_name
	local new_project_name = project_name .. "_copy"
	local target_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. new_project_name

	if not sandbox.filesystem.exists(world, source_path) then
		sandbox.logs.error(world, "[projects.duplicate] Failed: Source project does not exist at " .. source_path)
		return false
	end
	
	-- Keep adding _copy if it already exists
	while sandbox.filesystem.exists(world, target_path) do
		new_project_name = new_project_name .. "_copy"
		target_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. new_project_name
	end

	local success = sandbox.filesystem.copy(world, source_path, target_path, false, true)
	if not success or not sandbox.filesystem.exists(world, target_path) then
		sandbox.logs.error(world, "[projects.duplicate] Failed to duplicate project to: " .. target_path)
		return false
	end
	sandbox.logs.info(world, "[projects.duplicate] Successfully duplicated project as: " .. new_project_name)
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
	sandbox.logs.info(world, "[projects.import] Importing project: " .. project_name .. " from " .. physical_path)
	local target_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. project_name
	
	if sandbox.filesystem.exists(world, target_path) then
		sandbox.logs.error(world, "[projects.import] Failed: Destination project already exists at " .. target_path)
		return false
	end
	
	local success = sandbox.filesystem.copy(world, physical_path, target_path, false, true)
	if not success or not sandbox.filesystem.exists(world, target_path) then
		sandbox.logs.error(world, "[projects.import] Failed to import project to: " .. target_path)
		return false
	end
	sandbox.logs.info(world, "[projects.import] Successfully imported project to: " .. target_path)
	return true
end

function projects.export(project_name, dest_path)
	sandbox.logs.info(world, "[projects.export] Exporting project: " .. project_name .. " to " .. dest_path)
	local source_path = projects.VIRTUAL_PROJECTS_PATH_DIRECTORY .. "/" .. project_name
	local target_path = dest_path .. "/" .. project_name
	
	local success = sandbox.filesystem.copy(world, source_path, target_path, false, true)
	if not success then
		sandbox.logs.error(world, "[projects.export] Failed to export project to: " .. target_path)
		return false
	end
	sandbox.logs.info(world, "[projects.export] Successfully exported project to: " .. target_path)
	return true
end

function projects.list(sort_type)
	sandbox.logs.info(world, "[projects.list] Listing all projects in " .. projects.VIRTUAL_PROJECTS_PATH_DIRECTORY)
	local files = sandbox.filesystem.list_directories(world, projects.VIRTUAL_PROJECTS_PATH_DIRECTORY, false)
	local merged_properties = sandbox.Properties.new()

	for index, file_path in ipairs(files) do
		-- list_directories already guarantees these are directories
		local folder_name = file_path:match("([^/]+)$") or file_path
		sandbox.logs.info(world, "[projects.list] Found project directory: " .. folder_name)
		
		local project_properties = projects.find(folder_name)
		if project_properties then
			merged_properties:set_string("projects/" .. folder_name .. "/path", file_path)
			project_properties:destroy()
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
	
	local physical_path = ffi.new("char*[1]")
	local res = ffi.C.sandbox_filesystem_resolve_physical_path(
		(type(world) == "table" and world.ptr) and world.ptr or world, 
		target_path, physical_path)

	if not res or physical_path[0] == nil then
		sandbox.logs.error(world, "[projects.run] Failed: Could not resolve physical path for " .. target_path)
		return false
	end
	
	local actual_path = ffi.string(physical_path[0])
	ffi.C.free(physical_path[0]) -- Assuming it alloc'd it using malloc or strdup in C++

	local run_command = string.format("nohup %s \"%s\" > /dev/null 2>&1 &", projects.PHYSICAL_SANDBOX_LAUNCHER_PATH, actual_path)
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
	local window_flags = bit.bor(
		ffi.C.ImGuiWindowFlags_NoTitleBar,
		ffi.C.ImGuiWindowFlags_NoResize,
		ffi.C.ImGuiWindowFlags_NoMove,
		ffi.C.ImGuiWindowFlags_NoCollapse,
		ffi.C.ImGuiWindowFlags_NoSavedSettings
	)
	
	local w_width = spectre.window.get_width(g_world)
	local w_height = spectre.window.get_height(g_world)
	-- Use a proportion of the main window or hardcoded size
	local window_size = imgui.ImVec2(math.floor(w_width * 0.8), math.floor(w_height * 0.8))
	local center_pos = imgui.ImVec2(math.floor((w_width - window_size.x) * 0.5), math.floor((w_height - window_size.y) * 0.5))
	
	imgui.SetNextWindowPos(center_pos, ffi.C.ImGuiCond_FirstUseEver)
	imgui.SetNextWindowSize(window_size, ffi.C.ImGuiCond_FirstUseEver)
	
	if imgui.Begin("Projects Browser", nil, window_flags) then
		projects.view.import_browser:render()
		projects.view.export_browser:render()

		imgui.Text("Project Manager")
		imgui.Separator()
		imgui.Spacing()
		
		-- Top buttons
		if imgui.Button("New Project", imgui.ImVec2(150, 30)) then
			imgui.OpenPopup("New Project Modal")
		end
		
		if imgui.BeginPopupModal("New Project Modal", nil, ffi.C.ImGuiWindowFlags_AlwaysAutoResize) then
			if not projects.view.new_project_buffer then
				projects.view.new_project_buffer = ffi.new("char[256]")
				ffi.copy(projects.view.new_project_buffer, "New_Project")
			end
			
			imgui.Text("Enter Project Name:")
			imgui.InputText("##ProjectName", projects.view.new_project_buffer, 256)
			
			imgui.Spacing()
			
			if imgui.Button("Create", imgui.ImVec2(120, 0)) then
				local new_name = ffi.string(projects.view.new_project_buffer)
				if new_name and new_name ~= "" then
					projects.create(new_name)
					refresh_projects_list()
					imgui.CloseCurrentPopup()
				end
			end
			
			imgui.SetItemDefaultFocus()
			imgui.SameLine()
			
			if imgui.Button("Cancel", imgui.ImVec2(120, 0)) then
				imgui.CloseCurrentPopup()
			end
			
			imgui.EndPopup()
		end
		
		imgui.SameLine()
		
		if imgui.Button("Import Project", imgui.ImVec2(150, 30)) then
			projects.view.import_browser:open(function(paths)
				if type(paths) == "table" then
					for _, p in ipairs(paths) do
						local name = p:match("([^/]+)$") or p
						projects.import(name, p)
					end
				else
					local name = paths:match("([^/]+)$") or paths
					projects.import(name, paths)
				end
				refresh_projects_list()
			end)
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
					
					if imgui.Selectable(proj_name, is_selected, ffi.C.ImGuiSelectableFlags_AllowDoubleClick) then
						projects.view.selected_project = proj_name
						
						if imgui.IsMouseDoubleClicked(ffi.C.ImGuiMouseButton_Left) then
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
							projects.view.rename_buffer = ffi.new("char[256]")
							ffi.copy(projects.view.rename_buffer, proj_name)
						end
						if imgui.MenuItem("Duplicate") then
							projects.duplicate(proj_name)
							refresh_projects_list()
						end
						if imgui.MenuItem("Export") then
							projects.view.export_browser:open(function(paths)
								local dest = type(paths) == "table" and paths[1] or paths
								projects.export(proj_name, dest)
							end)
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
		
		if imgui.BeginPopupModal("Rename Project", nil, ffi.C.ImGuiWindowFlags_AlwaysAutoResize) then
			imgui.Text("Enter new name for " .. projects.view.selected_project .. ":")
			imgui.InputText("##newname", projects.view.rename_buffer, 256)
			
			imgui.Spacing()
			
			if imgui.Button("Rename", imgui.ImVec2(120, 0)) then
				local renamed = ffi.string(projects.view.rename_buffer)
				if string.len(renamed) > 0 and renamed ~= projects.view.selected_project then
					projects.rename(projects.view.selected_project, renamed)
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
	
	FileBrowser.run_test()
	
	if all_passed then
		sandbox.logs.info(world, "[Test] ===== ALL PROJECTS TESTS PASSED =====")
	else
		sandbox.logs.error(world, "[Test] ===== SOME PROJECTS TESTS FAILED =====")
	end
end

return {
	-- TODO: Attach this function to be run on_enter of Menu scene
	on_enter_projects_view = ecs.Script.define(projects.view.on_enter),

	-- TODO: Attach this function to be run on_update of menu scene
	on_render_projects_view = ecs.Script.define(projects.view.on_render),

	-- TODO: Attach this function to be run on_exit of on_exit of menu scene,
	on_exit_projects_view = ecs.Script.define(projects.view.on_exit),

	run_all_projects_tests = ecs.Script.define(projects.test_all),
}
