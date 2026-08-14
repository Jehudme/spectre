local sandbox = require("sandbox")
local ecs = require("ecs")
local history = require("utilities.history")

local write_file_action = {}

---@class WriteFileAction
local WriteFileAction = {}
WriteFileAction.__index = WriteFileAction

---Creates a new Action for writing to a file, with undo/redo capability.
---@param path string Virtual path of the file
---@param new_content string The new content to write to the file
---@param is_head boolean? If true (or nil), this action marks the start/head of a transaction group
---@param action_name string? Descriptive name of the action
---@return Action
function WriteFileAction.new(path, new_content, is_head, action_name)
	local world = ecs.from_ptr(g_world)
	local existed = sandbox.filesystem.exists(world, path)
	local old_content = nil
	if existed then
		old_content = sandbox.filesystem.read_file_string(world, path)
	end

	local redo_fn = function()
		local w = ecs.from_ptr(g_world)
		local parent = path:match("(.*)/[^/]+$")
		if parent and not sandbox.filesystem.exists(w, parent) then
			sandbox.filesystem.create_directory(w, parent, true)
		end
		sandbox.filesystem.write_file_string(w, path, new_content)
	end

	local undo_fn = function()
		local w = ecs.from_ptr(g_world)
		if existed then
			sandbox.filesystem.write_file_string(w, path, old_content)
		else
			sandbox.filesystem.remove_file(w, path)
		end
	end

	return Action.new(redo_fn, undo_fn, is_head, action_name or ("Write " .. path))
end

_G.WriteFileAction = WriteFileAction

function write_file_action.run_test()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[WriteFileAction Test] ===== STARTING ALL TESTS =====")

	local test_path = "save://write_action_test.txt"
	if sandbox.filesystem.exists(world, test_path) then
		sandbox.filesystem.remove_file(world, test_path)
	end

	history.clear()

	-- Test 1: Write file that didn't exist
	local action = WriteFileAction.new(test_path, "hello world", true, "Test Create File")
	history.execute(action)

	if sandbox.filesystem.exists(world, test_path) and sandbox.filesystem.read_file_string(world, test_path) == "hello world" then
		sandbox.logs.info(world, "[WriteFileAction Test] Passed initial write")
	else
		sandbox.logs.error(world, "[WriteFileAction Test] Failed initial write")
	end

	-- Test 2: Undo write (should delete since it didn't exist)
	history.undo()
	if not sandbox.filesystem.exists(world, test_path) then
		sandbox.logs.info(world, "[WriteFileAction Test] Passed undo (file deleted)")
	else
		sandbox.logs.error(world, "[WriteFileAction Test] Failed undo (file not deleted)")
	end

	-- Test 3: Redo write
	history.redo()
	if sandbox.filesystem.exists(world, test_path) and sandbox.filesystem.read_file_string(world, test_path) == "hello world" then
		sandbox.logs.info(world, "[WriteFileAction Test] Passed redo")
	else
		sandbox.logs.error(world, "[WriteFileAction Test] Failed redo")
	end

	-- Test 4: Overwrite existing file
	local action2 = WriteFileAction.new(test_path, "overwritten content", true, "Test Overwrite")
	history.execute(action2)

	if sandbox.filesystem.read_file_string(world, test_path) == "overwritten content" then
		sandbox.logs.info(world, "[WriteFileAction Test] Passed overwrite")
	else
		sandbox.logs.error(world, "[WriteFileAction Test] Failed overwrite")
	end

	-- Test 5: Undo overwrite (should restore "hello world")
	history.undo()
	if sandbox.filesystem.read_file_string(world, test_path) == "hello world" then
		sandbox.logs.info(world, "[WriteFileAction Test] Passed undo overwrite")
	else
		sandbox.logs.error(world, "[WriteFileAction Test] Failed undo overwrite")
	end

	-- Clean up
	if sandbox.filesystem.exists(world, test_path) then
		sandbox.filesystem.remove_file(world, test_path)
	end

	sandbox.logs.info(world, "[WriteFileAction Test] ===== ALL TESTS FINISHED =====")
	return true
end

return write_file_action
