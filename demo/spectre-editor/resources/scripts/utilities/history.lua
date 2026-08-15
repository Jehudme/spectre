local sandbox = require("sandbox")
local ecs = require("ecs")

-- ==========================================
-- Action Class
-- ==========================================

---@class Action
local Action = {}
Action.__index = Action

---Creates a new historical action.
---@param redo_function function The function to execute or redo.
---@param undo_function function The function to undo the action.
---@param is_head boolean? If true (or nil), this action marks the start/head of a transaction group.
---@param action_name string? Descriptive name of the action.
---@return Action
function Action.new(redo_function, undo_function, is_head, action_name)
	local instance = setmetatable({}, Action)
	instance.redo_function = redo_function
	instance.undo_function = undo_function
	instance.is_head = (is_head == nil) and true or is_head
	instance.action_name = action_name or "Unnamed Action"
	return instance
end

_G.Action = Action

-- ==========================================
-- History Module
-- ==========================================

if _G.history then
	return _G.history
end

history = {
	actions_stack = {},
	actions_index = 0,
	-- Notification state: set by undo/redo, consumed by the editor overlay
	notification = nil, -- { text = string, kind = "undo"|"redo", timer = number }
	change_version = 0,
}

if g_world then
	local w = require("ecs").from_ptr(g_world)
	require("sandbox").logs.info(w, "[History Module] history table initialized! Address: " .. tostring(history))
end

---Executes a new action and clears the forward redo history.
---@param action Action
function history.execute(action)
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[History] Executing action: " .. (action.action_name or "Unnamed Action"))

	-- Truncate any forward redo history if we were in an undone state
	for i = #history.actions_stack, history.actions_index + 1, -1 do
		table.remove(history.actions_stack, i)
	end

	table.insert(history.actions_stack, action)
	history.actions_index = #history.actions_stack

	if action.redo_function then
		action.redo_function()
	end
end

---Undoes actions backwards until a transaction head (is_head = true) is processed.
function history.undo()
	if history.actions_index == 0 then
		return
	end

	local world = ecs.from_ptr(g_world)
	local first_name = nil
	while history.actions_index > 0 do
		local action = history.actions_stack[history.actions_index]
		sandbox.logs.info(world, "[History] Undoing action: " .. (action.action_name or "Unnamed Action"))
		if not first_name then first_name = action.action_name end
		if action.undo_function then
			action.undo_function()
		end
		history.actions_index = history.actions_index - 1
		if action.is_head then
			break
		end
	end
	history.notification = { text = "Undo: " .. (first_name or "Action"), kind = "undo", timer = 2.0 }
	history.change_version = history.change_version + 1
end

---Redoes actions forwards until the next transaction group's head (is_head = true) is reached.
function history.redo()
	if history.actions_index >= #history.actions_stack then
		return
	end

	local world = ecs.from_ptr(g_world)
	local is_first_action = true
	local first_name = nil
	while history.actions_index < #history.actions_stack do
		local next_index = history.actions_index + 1
		local next_action = history.actions_stack[next_index]

		-- If we hit another group's head and it's not the first action we are redoing, stop.
		if not is_first_action and next_action.is_head then
			break
		end

		sandbox.logs.info(world, "[History] Redoing action: " .. (next_action.action_name or "Unnamed Action"))
		if not first_name then first_name = next_action.action_name end
		history.actions_index = next_index
		if next_action.redo_function then
			next_action.redo_function()
		end
		is_first_action = false
	end
	history.notification = { text = "Redo: " .. (first_name or "Action"), kind = "redo", timer = 2.0 }
	history.change_version = history.change_version + 1
end

---Clears the action history stack.
function history.clear()
	history.actions_stack = {}
	history.actions_index = 0
end

-- ==========================================
-- Tests
-- ==========================================

---Runs the test suite for the history system to verify correct transaction grouping and undo/redo behavior.
---@return boolean
function history.run_test()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[History Test] ===== STARTING ALL TESTS =====")

	history.clear()

	local trace = {}
	local function log_trace(message)
		table.insert(trace, message)
	end

	-- Test 1: Single actions
	local action1 = Action.new(function()
		log_trace("redo1")
	end, function()
		log_trace("undo1")
	end, true)

	history.execute(action1)
	if #trace == 1 and trace[1] == "redo1" then
		sandbox.logs.info(world, "[History Test] Passed Execute Action 1")
	else
		sandbox.logs.error(world, "[History Test] Failed Execute Action 1")
	end

	trace = {}
	history.undo()
	if #trace == 1 and trace[1] == "undo1" and history.actions_index == 0 then
		sandbox.logs.info(world, "[History Test] Passed Undo Action 1")
	else
		sandbox.logs.error(world, "[History Test] Failed Undo Action 1")
	end

	trace = {}
	history.redo()
	if #trace == 1 and trace[1] == "redo1" and history.actions_index == 1 then
		sandbox.logs.info(world, "[History Test] Passed Redo Action 1")
	else
		sandbox.logs.error(world, "[History Test] Failed Redo Action 1")
	end

	-- Test 2: Action Group (Transaction)
	history.clear()
	trace = {}

	local action_g1 = Action.new(function()
		log_trace("redo_g1")
	end, function()
		log_trace("undo_g1")
	end, true)
	local action_g2 = Action.new(function()
		log_trace("redo_g2")
	end, function()
		log_trace("undo_g2")
	end, false)
	local action_g3 = Action.new(function()
		log_trace("redo_g3")
	end, function()
		log_trace("undo_g3")
	end, false)

	history.execute(action_g1)
	history.execute(action_g2)
	history.execute(action_g3)

	trace = {}
	history.undo() -- should undo g3, g2, g1 and stop because g1 is head
	if
		#trace == 3
		and trace[1] == "undo_g3"
		and trace[2] == "undo_g2"
		and trace[3] == "undo_g1"
		and history.actions_index == 0
	then
		sandbox.logs.info(world, "[History Test] Passed Group Undo")
	else
		sandbox.logs.error(world, "[History Test] Failed Group Undo. Trace: " .. table.concat(trace, ", "))
	end

	trace = {}
	history.redo() -- should redo g1, g2, g3
	if
		#trace == 3
		and trace[1] == "redo_g1"
		and trace[2] == "redo_g2"
		and trace[3] == "redo_g3"
		and history.actions_index == 3
	then
		sandbox.logs.info(world, "[History Test] Passed Group Redo")
	else
		sandbox.logs.error(world, "[History Test] Failed Group Redo. Trace: " .. table.concat(trace, ", "))
	end

	-- Test 3: Multiple Groups
	history.clear()
	trace = {}

	-- Group A
	local act_a1 = Action.new(function()
		log_trace("redo_a1")
	end, function()
		log_trace("undo_a1")
	end, true)
	local act_a2 = Action.new(function()
		log_trace("redo_a2")
	end, function()
		log_trace("undo_a2")
	end, false)
	-- Group B
	local act_b1 = Action.new(function()
		log_trace("redo_b1")
	end, function()
		log_trace("undo_b1")
	end, true)
	local act_b2 = Action.new(function()
		log_trace("redo_b2")
	end, function()
		log_trace("undo_b2")
	end, false)

	history.execute(act_a1)
	history.execute(act_a2)
	history.execute(act_b1)
	history.execute(act_b2)

	trace = {}
	history.undo() -- should undo b2, b1 (stop at b1 which is head)
	if #trace == 2 and trace[1] == "undo_b2" and trace[2] == "undo_b1" and history.actions_index == 2 then
		sandbox.logs.info(world, "[History Test] Passed Partial Group Undo (Group B)")
	else
		sandbox.logs.error(world, "[History Test] Failed Partial Group Undo (Group B)")
	end

	trace = {}
	history.undo() -- should undo a2, a1 (stop at a1 which is head)
	if #trace == 2 and trace[1] == "undo_a2" and trace[2] == "undo_a1" and history.actions_index == 0 then
		sandbox.logs.info(world, "[History Test] Passed Double Group Undo (Group A)")
	else
		sandbox.logs.error(world, "[History Test] Failed Double Group Undo (Group A)")
	end

	trace = {}
	history.redo() -- should redo a1, a2 (stop before b1 because b1 is a head)
	if #trace == 2 and trace[1] == "redo_a1" and trace[2] == "redo_a2" and history.actions_index == 2 then
		sandbox.logs.info(world, "[History Test] Passed Partial Group Redo (Group A)")
	else
		sandbox.logs.error(world, "[History Test] Failed Partial Group Redo (Group A)")
	end

	trace = {}
	history.redo() -- should redo b1, b2
	if #trace == 2 and trace[1] == "redo_b1" and trace[2] == "redo_b2" and history.actions_index == 4 then
		sandbox.logs.info(world, "[History Test] Passed Double Group Redo (Group B)")
	else
		sandbox.logs.error(world, "[History Test] Failed Double Group Redo (Group B)")
	end

	sandbox.logs.info(world, "[History Test] ===== ALL TESTS FINISHED =====")
	return true
end

return history
