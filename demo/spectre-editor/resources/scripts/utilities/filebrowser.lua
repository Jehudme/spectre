local sandbox = require("sandbox")
local imgui = require("imgui")
local ffi = require("ffi")
local ecs = require("ecs")

---@class FileBrowser
---@field mode string "file" or "directory"
---@field current_path string
---@field selected string
---@field is_open boolean
---@field callback function
local FileBrowser = {}
FileBrowser.__index = FileBrowser

-- ========================================
-- Inline Functions (Class Methods)
-- ========================================

--- Creates a new FileBrowser instance
---@param mode string "file" or "directory"
---@param start_path string Optional starting path, defaults to "app://"
---@return FileBrowser
function FileBrowser.new(mode, start_path)
    local instance = setmetatable({}, FileBrowser)
    instance.mode = mode or "file"
    instance.current_path = start_path or "app://"
    instance.selected = ""
    instance.is_open = false
    instance.callback = nil
    
    local world = ecs.from_ptr(g_world)
    if world then
        sandbox.logs.info(world, "[FileBrowser.new] Created FileBrowser in mode: " .. instance.mode .. " at path: " .. instance.current_path)
    end
    return instance
end

--- Opens the file browser popup
---@param callback function Called with the selected path when the user clicks 'Select'
function FileBrowser:open(callback)
    self.is_open = true
    self.callback = callback
    self.selected = ""
    local world = ecs.from_ptr(g_world)
    if world then
        sandbox.logs.info(world, "[FileBrowser:open] Opening FileBrowser at " .. self.current_path)
    end
end

--- Closes the file browser popup
function FileBrowser:close()
    self.is_open = false
    local world = ecs.from_ptr(g_world)
    if world then
        sandbox.logs.info(world, "[FileBrowser:close] Closing FileBrowser")
    end
end

--- Navigates up one directory
function FileBrowser:navigate_up()
    local world = ecs.from_ptr(g_world)
    -- Simplified string manipulation for parent directory
    -- e.g., save://projects/test -> save://projects
    local parent = self.current_path:match("(.*)/[^/]+$")
    if parent and parent ~= "" then
        if world then sandbox.logs.info(world, "[FileBrowser:navigate_up] Navigating from " .. self.current_path .. " to " .. parent) end
        self.current_path = parent
        self.selected = ""
    else
        if world then sandbox.logs.info(world, "[FileBrowser:navigate_up] Cannot navigate up from root path: " .. self.current_path) end
    end
end

-- ========================================
-- Interface / Render
-- ========================================

--- Renders the ImGui interface for the FileBrowser
function FileBrowser:render()
    if not self.is_open then return end

    local world = ecs.from_ptr(g_world)

    -- Define popup ID based on mode
    local popup_id = "File Browser (" .. string.upper(self.mode) .. ")##FileBrowserPopup"

    -- Force open the popup since is_open is true
    if self.is_open then
        imgui.OpenPopup(popup_id)
    end

    local window_size = imgui.ImVec2(600, 400)
    local center_pos = imgui.ImVec2(100, 100)
    
    imgui.SetNextWindowPos(center_pos, ffi.C.ImGuiCond_FirstUseEver)
    imgui.SetNextWindowSize(window_size, ffi.C.ImGuiCond_FirstUseEver)

    if imgui.BeginPopupModal(popup_id, nil, ffi.C.ImGuiWindowFlags_NoSavedSettings) then
        -- Top bar: Path and UP button
        if imgui.Button("Up") then
            self:navigate_up()
        end
        imgui.SameLine()
        imgui.Text("Path: " .. self.current_path)
        imgui.Separator()

        -- Content list
        if imgui.BeginChild("FileBrowserContent", imgui.ImVec2(0, -imgui.GetFrameHeightWithSpacing() - 10), true, 0) then
            -- Note: sandbox.filesystem.list_files natively doesn't split directories from files easily without iterating,
            -- but we will list the entire contents of current_path.
            local files = sandbox.filesystem.list_files(world, self.current_path, false)
            
            if files and #files > 0 then
                for i, path in ipairs(files) do
                    local is_dir = sandbox.filesystem.is_directory(world, path)
                    local name = path:match("([^/]+)$") or path
                    
                    -- Prepend folder icon / marker
                    local display_name = (is_dir and "[DIR] " or "[FILE] ") .. name
                    
                    local is_selected = (self.selected == path)
                    
                    if imgui.Selectable(display_name, is_selected, ffi.C.ImGuiSelectableFlags_AllowDoubleClick) then
                        self.selected = path
                        
                        if imgui.IsMouseDoubleClicked(ffi.C.ImGuiMouseButton_Left) then
                            if is_dir then
                                -- Enter directory
                                sandbox.logs.info(world, "[FileBrowser] Entering directory: " .. path)
                                self.current_path = path
                                self.selected = ""
                            else
                                -- If mode is file, double clicking auto-selects it
                                if self.mode == "file" then
                                    sandbox.logs.info(world, "[FileBrowser] Selected file via double click: " .. path)
                                    if self.callback then self.callback(path) end
                                    self:close()
                                end
                            end
                        end
                    end
                end
            else
                imgui.TextDisabled("Empty directory or invalid path.")
            end
            
            imgui.EndChild()
        end

        imgui.Separator()

        -- Bottom bar: Selection display and Confirm/Cancel
        imgui.Text("Selected: " .. (self.selected ~= "" and self.selected or "None"))
        imgui.SameLine(imgui.GetWindowWidth() - 150)
        
        local can_select = false
        if self.selected ~= "" then
            local is_dir = sandbox.filesystem.is_directory(world, self.selected)
            if self.mode == "directory" and is_dir then
                can_select = true
            elseif self.mode == "file" and not is_dir then
                can_select = true
            end
        end

        if not can_select then
            imgui.BeginDisabled(true)
        end
        if imgui.Button("Select", imgui.ImVec2(60, 0)) then
            sandbox.logs.info(world, "[FileBrowser] Selection confirmed: " .. self.selected)
            if self.callback then self.callback(self.selected) end
            self:close()
        end
        if not can_select then
            imgui.EndDisabled()
        end

        imgui.SameLine()
        if imgui.Button("Cancel", imgui.ImVec2(60, 0)) then
            sandbox.logs.info(world, "[FileBrowser] Selection cancelled.")
            self:close()
        end

        if not self.is_open then
            imgui.CloseCurrentPopup()
        end

        imgui.EndPopup()
    end
end

-- ========================================
-- Tests
-- ========================================

function FileBrowser.test_instantiation()
    local world = ecs.from_ptr(g_world)
    sandbox.logs.info(world, "[Test] Starting FileBrowser test_instantiation")
    
    local browser = FileBrowser.new("directory", "save://projects")
    if not browser or browser.mode ~= "directory" then
        sandbox.logs.error(world, "[Test] Failed FileBrowser test_instantiation")
        return false
    end
    
    sandbox.logs.info(world, "[Test] Passed FileBrowser test_instantiation")
    return true
end

function FileBrowser.test_navigation()
    local world = ecs.from_ptr(g_world)
    sandbox.logs.info(world, "[Test] Starting FileBrowser test_navigation")
    
    local browser = FileBrowser.new("file", "save://projects/test_proj")
    browser:navigate_up()
    
    if browser.current_path ~= "save://projects" then
        sandbox.logs.error(world, "[Test] Failed FileBrowser test_navigation, path is " .. browser.current_path)
        return false
    end
    
    sandbox.logs.info(world, "[Test] Passed FileBrowser test_navigation")
    return true
end

function FileBrowser.test_all()
    local world = ecs.from_ptr(g_world)
    sandbox.logs.info(world, "[Test] ===== STARTING ALL FILEBROWSER TESTS =====")
    
    local all_passed = true
    all_passed = all_passed and FileBrowser.test_instantiation()
    all_passed = all_passed and FileBrowser.test_navigation()
    
    if all_passed then
        sandbox.logs.info(world, "[Test] ===== ALL FILEBROWSER TESTS PASSED =====")
    else
        sandbox.logs.error(world, "[Test] ===== SOME FILEBROWSER TESTS FAILED =====")
    end
end

return FileBrowser
