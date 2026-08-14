local sandbox = require("sandbox")
local ecs = require("ecs")
local history = require("utilities.history")

local pages -- Forward declaration for lexical scope function resolution

-- ==========================================
-- Page Class
-- ==========================================

---@class Page
local Page = {}
Page.__index = Page

---Creates a new page instance.
---@return Page
function Page.new()
	local instance = setmetatable({}, Page)
	instance.type = nil
	instance.name = nil
	return instance
end

function Page:on_enter() end
function Page:on_render() end
function Page:on_exit() end

_G.Page = Page

-- ==========================================
-- SwitchPageAction Class
-- ==========================================

---@class SwitchPageAction
local SwitchPageAction = {}
SwitchPageAction.__index = SwitchPageAction

---Creates a switch page action.
---@param old_page Page|nil The page being transitioned away from.
---@param new_page Page The page being transitioned to.
---@return SwitchPageAction
function SwitchPageAction.new(old_page, new_page)
	local instance = setmetatable({}, SwitchPageAction)
	instance.old_page = old_page
	instance.new_page = new_page
	return instance
end

---Executes or redoes the page transition.
function SwitchPageAction:redo()
	if self.old_page and self.old_page.on_exit then
		self.old_page:on_exit()
	end
	pages.current_page = self.new_page
	if self.new_page and self.new_page.on_enter then
		self.new_page:on_enter()
	end
end

---Undoes the page transition.
function SwitchPageAction:undo()
	if self.new_page and self.new_page.on_exit then
		self.new_page:on_exit()
	end
	pages.current_page = self.old_page
	if self.old_page and self.old_page.on_enter then
		self.old_page:on_enter()
	end
end

_G.SwitchPageAction = SwitchPageAction

-- ==========================================
-- Pages Module
-- ==========================================

pages = {
	map = {}, -- map of type table containing a map of name -> Page
	current_page = nil,
	defaults = {},
	actions = {},
}

---Registers a page within a specific category type.
---@param page_type string The category of the page (e.g. "views", "modules").
---@param page_name string The name of the page.
---@param page Page The page instance.
function pages.register(page_type, page_name, page)
	if not pages.map[page_type] then
		pages.map[page_type] = {}
	end
	page.type = page_type
	page.name = page_name
	pages.map[page_type][page_name] = page
end

---Finds a registered page.
---@param page_type string
---@param page_name string
---@return Page|nil
function pages.find(page_type, page_name)
	if not pages.map[page_type] then
		return nil
	end
	return pages.map[page_type][page_name]
end

---Lists all pages, optionally filtered by type.
---@param page_type string? Optional category filter.
---@return Page[] Array of Page instances.
function pages.list_all(page_type)
	local result = {}
	if page_type then
		if pages.map[page_type] then
			for _, page in pairs(pages.map[page_type]) do
				table.insert(result, page)
			end
		end
	else
		for _, sub_map in pairs(pages.map) do
			for _, page in pairs(sub_map) do
				table.insert(result, page)
			end
		end
	end
	return result
end

---Sets the default page for a type.
---@param page_type string
---@param page_name string? If nil, falls back to the first registered page of this type.
function pages.set_default(page_type, page_name)
	if page_name then
		pages.defaults[page_type] = page_name
	else
		if pages.map[page_type] then
			local first_name = next(pages.map[page_type])
			if first_name then
				pages.defaults[page_type] = first_name
			end
		end
	end
end

---Checks if a page or category type exists.
---@param page_type string
---@param page_name string? Optional page name.
---@return boolean
function pages.contain(page_type, page_name)
	if not page_name then
		return pages.map[page_type] ~= nil
	end
	return pages.map[page_type] ~= nil and pages.map[page_type][page_name] ~= nil
end

---Gets the currently active page.
---@return Page|nil
function pages.get_current()
	return pages.current_page
end

---Switches the current page to the target page, recording the action in the history stack.
---@param new_page Page|string The page object to switch to, or the type if name is also passed.
---@param page_name string? Optional if page_type was passed as the first parameter.
function pages.actions.switch_page(new_page, page_name)
	local target_page = nil
	if type(new_page) == "string" then
		target_page = pages.find(new_page, page_name)
	else
		target_page = new_page
	end

	if not target_page then
		return
	end
	if pages.current_page == target_page then
		return
	end

	local switch_action = SwitchPageAction.new(pages.current_page, target_page)
	local action = Action.new(
		function()
			switch_action:redo()
		end,
		function()
			switch_action:undo()
		end,
		false, -- is_head = false (not a head transaction)
		"Switch Page to " .. (target_page.name or "Unknown")
	)

	history.execute(action)
end

-- ==========================================
-- Tests
-- ==========================================

---Runs the test suite for the pages system.
---@return boolean
function pages.run_test()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[Pages Test] ===== STARTING ALL TESTS =====")

	local backup_map = pages.map
	local backup_current_page = pages.current_page
	local backup_defaults = pages.defaults

	pages.map = {}
	pages.current_page = nil
	pages.defaults = {}
	history.clear()

	local trace = {}
	local function log_trace(message)
		table.insert(trace, message)
	end

	-- Create dummy pages
	local page_menu = Page.new()
	page_menu.on_enter = function()
		log_trace("enter_menu")
	end
	page_menu.on_exit = function()
		log_trace("exit_menu")
	end

	local page_editor = Page.new()
	page_editor.on_enter = function()
		log_trace("enter_editor")
	end
	page_editor.on_exit = function()
		log_trace("exit_editor")
	end

	-- Test Registration
	pages.register("views", "Menu", page_menu)
	pages.register("views", "Editor", page_editor)

	if pages.contain("views", "Menu") and pages.contain("views", "Editor") then
		sandbox.logs.info(world, "[Pages Test] Passed Page Registration and Contain")
	else
		sandbox.logs.error(world, "[Pages Test] Failed Page Registration")
	end

	-- Test list_all
	local all_views = pages.list_all("views")
	if #all_views == 2 then
		sandbox.logs.info(world, "[Pages Test] Passed List All Views")
	else
		sandbox.logs.error(world, "[Pages Test] Failed List All Views")
	end

	-- Test set_default
	pages.set_default("views", "Menu")
	if pages.defaults["views"] == "Menu" then
		sandbox.logs.info(world, "[Pages Test] Passed Set Default")
	else
		sandbox.logs.error(world, "[Pages Test] Failed Set Default")
	end

	-- Test switch page
	pages.current_page = page_menu
	trace = {}
	pages.actions.switch_page("views", "Editor")

	if pages.current_page == page_editor and #trace == 2 and trace[1] == "exit_menu" and trace[2] == "enter_editor" then
		sandbox.logs.info(world, "[Pages Test] Passed Switch Page Action")
	else
		sandbox.logs.error(
			world,
			"[Pages Test] Failed Switch Page Action. Current: "
				.. tostring(pages.current_page and pages.current_page.name or nil)
		)
	end

	-- Test Undo switch page
	trace = {}
	history.undo()
	if pages.current_page == page_menu and #trace == 2 and trace[1] == "exit_editor" and trace[2] == "enter_menu" then
		sandbox.logs.info(world, "[Pages Test] Passed Undo Switch Page")
	else
		sandbox.logs.error(world, "[Pages Test] Failed Undo Switch Page")
	end

	-- Test Redo switch page
	trace = {}
	history.redo()
	if pages.current_page == page_editor and #trace == 2 and trace[1] == "exit_menu" and trace[2] == "enter_editor" then
		sandbox.logs.info(world, "[Pages Test] Passed Redo Switch Page")
	else
		sandbox.logs.error(world, "[Pages Test] Failed Redo Switch Page")
	end

	sandbox.logs.info(world, "[Pages Test] ===== ALL TESTS FINISHED =====")
	pages.map = backup_map
	pages.current_page = backup_current_page
	pages.defaults = backup_defaults
	return true
end

return pages
