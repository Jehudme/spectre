local sandbox = require("sandbox")
local imgui = require("imgui")
local ffi = require("ffi")
local ecs = require("ecs")
local spectre = require("spectre")

package.cpath = package.cpath .. ";/home/jehud/.luarocks/lib/lua/5.1/?.so"
local lfs_success, lfs = pcall(require, "lfs")

---@class FileBrowser
local FileBrowser = {}
FileBrowser.__index = FileBrowser

-- ==========================================
-- Utility / File Operations
-- ==========================================

local function resolve_physical(world, virtual_path)
    local physical_path = ffi.new("char*[1]")
    local ptr = (type(world) == "table" and world.ptr) and world.ptr or world
    if ffi.C.sandbox_filesystem_resolve_physical_path(ptr, virtual_path, physical_path) and physical_path[0] ~= nil then
        local p = ffi.string(physical_path[0])
        ffi.C.free(physical_path[0])
        return p
    end
    return nil
end

local function get_file_stats(world, virtual_path)
    if not lfs_success then return { size = 0, time = 0 } end
    local phys = resolve_physical(world, virtual_path)
    if phys then
        local attr = lfs.attributes(phys)
        if attr then return { size = attr.size, time = attr.modification } end
    end
    return { size = 0, time = 0 }
end

local function join_path(base, name)
    if base == "" or base:match("://$") or base:match(":/$") then
        return base .. name
    end
    return base .. "/" .. name
end

local function parent_dir(path)
    local parent = path:match("(.*)/[^/]+/?$")
    if not parent or parent == "" then
        if path:match("://") then return path end
        return "os://"
    end
    if parent:match(":/$") then return parent .. "/" end
    return parent
end

local function get_name(path)
    return path:match("([^/]+)$") or path
end

-- ==========================================
-- Instance Methods
-- ==========================================

function FileBrowser.new(mode, start_path, readonly)
    local instance = setmetatable({}, FileBrowser)
    instance.mode = mode or "file"
    instance.readonly = readonly or false
    
    if not start_path or start_path == "os://" then
        local home = os.getenv("HOME")
        if home then
            instance.current_path = "os:/" .. home
        else
            instance.current_path = "os://"
        end
    else
        instance.current_path = start_path
    end
    instance.is_open = false
    instance.callback = nil
    
    instance.history = { instance.current_path }
    instance.history_index = 1
    
    instance.items = {} -- stores { path=..., name=..., is_dir=..., size=..., time=... }
    instance.selected = {} -- table of paths
    instance.search_query = ffi.new("char[256]")
    instance.clipboard = {}
    instance.clipboard_op = "copy" -- or "cut"
    
    instance.sort_by = "name" -- "name", "size", "time", "type"
    instance.sort_desc = false
    
    instance.rename_active_for = nil
    instance.rename_buffer = ffi.new("char[256]")
    
    instance.new_folder_active = false
    instance.new_folder_buffer = ffi.new("char[256]")
    
    local world = ecs.from_ptr(g_world)
    sandbox.filesystem.mount(world, "/", "os://", false)
    
    return instance
end

function FileBrowser:open(callback)
    self.is_open = true
    self.callback = callback
    self.selected = {}
    self:refresh()
    imgui.OpenPopup("File Browser")
end

function FileBrowser:close()
    self.is_open = false
    imgui.CloseCurrentPopup()
end

function FileBrowser:go_home()
    self:change_directory("os://")
end

function FileBrowser:change_directory(new_path)
    if self.current_path == new_path then return end
    
    -- truncate history if we go back then branch off
    for i = #self.history, self.history_index + 1, -1 do
        table.remove(self.history, i)
    end
    table.insert(self.history, new_path)
    self.history_index = #self.history
    
    self.current_path = new_path
    self.selected = {}
    self:refresh()
end

function FileBrowser:navigate_up()
    self:change_directory(parent_dir(self.current_path))
end

function FileBrowser:forward()
    if self.history_index < #self.history then
        self.history_index = self.history_index + 1
        self.current_path = self.history[self.history_index]
        self.selected = {}
        self:refresh()
    end
end

function FileBrowser:backward()
    if self.history_index > 1 then
        self.history_index = self.history_index - 1
        self.current_path = self.history[self.history_index]
        self.selected = {}
        self:refresh()
    end
end

function FileBrowser:refresh()
    local world = ecs.from_ptr(g_world)
    self.items = {}
    
    local dirs = sandbox.filesystem.list_directories(world, self.current_path, false) or {}
    for i, p in ipairs(dirs) do
        local stats = get_file_stats(world, p)
        table.insert(self.items, { path = p, name = get_name(p), is_dir = true, size = stats.size, time = stats.time })
    end
    
    if self.mode == "file" or self.mode == "both" then
        local files = sandbox.filesystem.list_files(world, self.current_path, false) or {}
        for i, p in ipairs(files) do
            local stats = get_file_stats(world, p)
            table.insert(self.items, { path = p, name = get_name(p), is_dir = false, size = stats.size, time = stats.time })
        end
    end
    
    self:apply_sort()
end

function FileBrowser:apply_sort()
    table.sort(self.items, function(a, b)
        local val_a, val_b
        if self.sort_by == "name" then val_a, val_b = a.name:lower(), b.name:lower()
        elseif self.sort_by == "size" then val_a, val_b = a.size, b.size
        elseif self.sort_by == "time" then val_a, val_b = a.time, b.time
        elseif self.sort_by == "type" then val_a, val_b = (a.is_dir and 0 or 1), (b.is_dir and 0 or 1)
        end
        
        if val_a == val_b then
            return a.name:lower() < b.name:lower()
        end
        if self.sort_desc then
            return val_a > val_b
        end
        return val_a < val_b
    end)
end

function FileBrowser:delete(paths)
    local world = ecs.from_ptr(g_world)
    for _, p in ipairs(paths) do
        sandbox.logs.info(world, "[FileBrowser] Deleting: " .. p)
        if sandbox.filesystem.is_directory(world, p) then
            sandbox.filesystem.remove_directory(world, p)
        else
            sandbox.filesystem.remove_file(world, p)
        end
        self.selected[p] = nil
    end
    self:refresh()
end

function FileBrowser:copy_to_clipboard(paths, is_cut)
    self.clipboard = {}
    for _, p in ipairs(paths) do table.insert(self.clipboard, p) end
    self.clipboard_op = is_cut and "cut" or "copy"
end

function FileBrowser:paste(dest_dir)
    local world = ecs.from_ptr(g_world)
    for _, src in ipairs(self.clipboard) do
        local name = get_name(src)
        local dest = join_path(dest_dir, name)
        
        if self.clipboard_op == "copy" then
            sandbox.logs.info(world, "[FileBrowser] Pasting (Copy): " .. src .. " to " .. dest)
            sandbox.filesystem.copy(world, src, dest, false, true)
        else
            sandbox.logs.info(world, "[FileBrowser] Pasting (Move): " .. src .. " to " .. dest)
            sandbox.filesystem.move(world, src, dest, false, true)
        end
    end
    if self.clipboard_op == "cut" then self.clipboard = {} end
    self:refresh()
end

function FileBrowser:duplicate(paths)
    local world = ecs.from_ptr(g_world)
    for _, src in ipairs(paths) do
        local name = get_name(src)
        local dest = join_path(parent_dir(src), name .. "_copy")
        sandbox.logs.info(world, "[FileBrowser] Duplicating: " .. src .. " to " .. dest)
        sandbox.filesystem.copy(world, src, dest, false, true)
    end
    self:refresh()
end

function FileBrowser:create_folder(name)
    local world = ecs.from_ptr(g_world)
    local dest = join_path(self.current_path, name)
    sandbox.logs.info(world, "[FileBrowser] Creating folder: " .. dest)
    sandbox.filesystem.create_directory(world, dest, false)
    self:refresh()
end

function FileBrowser:commit_rename(old_path, new_name)
    local world = ecs.from_ptr(g_world)
    local dest = join_path(parent_dir(old_path), new_name)
    sandbox.logs.info(world, "[FileBrowser] Renaming: " .. old_path .. " to " .. dest)
    sandbox.filesystem.move(world, old_path, dest, false, true)
    self.selected[old_path] = nil
    self:refresh()
end

-- ==========================================
-- Render Helpers
-- ==========================================

local function format_size(bytes)
    if bytes == 0 then return "--" end
    if bytes < 1024 then return bytes .. " B" end
    if bytes < 1024 * 1024 then return string.format("%.1f KB", bytes / 1024) end
    return string.format("%.1f MB", bytes / (1024 * 1024))
end

local function get_breadcrumb_parts(path)
    local parts = {}
    -- Extract scheme like "os:/", "save:/", etc.
    local scheme, rest = path:match("^([a-zA-Z]+:/+)(.*)")
    if scheme then
        table.insert(parts, { label = scheme, path = scheme })
        if rest and rest ~= "" then
            local accumulated = scheme
            for segment in rest:gmatch("[^/]+") do
                if accumulated:match(":/+$") then
                    accumulated = accumulated:gsub("/+$", "") .. "/" .. segment
                else
                    accumulated = accumulated .. "/" .. segment
                end
                table.insert(parts, { label = segment, path = accumulated })
            end
        end
    else
        table.insert(parts, { label = path, path = path })
    end
    return parts
end

-- ==========================================
-- Render
-- ==========================================

function FileBrowser:render()
    if not self.is_open then return end

    local w_width  = spectre.window.get_width(g_world)
    local w_height = spectre.window.get_height(g_world)
    local modal_w  = math.max(800, math.floor(w_width  * 0.80))
    local modal_h  = math.max(500, math.floor(w_height * 0.80))

    imgui.SetNextWindowSize(imgui.ImVec2(modal_w, modal_h), ffi.C.ImGuiCond_Always)
    imgui.SetNextWindowPos(
        imgui.ImVec2(math.floor(w_width / 2), math.floor(w_height / 2)),
        ffi.C.ImGuiCond_Always,
        imgui.ImVec2(0.5, 0.5))

    local popup_flags = bit.bor(
        ffi.C.ImGuiWindowFlags_NoSavedSettings,
        ffi.C.ImGuiWindowFlags_NoResize,
        ffi.C.ImGuiWindowFlags_NoMove)

    if imgui.BeginPopupModal("File Browser", nil, popup_flags) then

    -- ---- collect selection ----
    local selected_paths = {}
    local selected_count = 0
    for path, flag in pairs(self.selected) do
        if flag then
            selected_count = selected_count + 1
            table.insert(selected_paths, path)
        end
    end

    -- ===========================================
    -- TOP NAV BAR  (back / forward / up / breadcrumb / search)
    -- ===========================================
    local nav_button_size = imgui.ImVec2(28, 24)

    local back_enabled    = self.history_index > 1
    local forward_enabled = self.history_index < #self.history

    if back_enabled then
        if imgui.Button("<##back", nav_button_size) then self:backward() end
    else
        imgui.SmallButton(" < ") -- visually present but non-functional
    end

    imgui.SameLine(0, 3)

    if forward_enabled then
        if imgui.Button(">##fwd", nav_button_size) then self:forward() end
    else
        imgui.SmallButton(" > ")
    end

    imgui.SameLine(0, 3)
    if imgui.Button("^##up", nav_button_size) then self:navigate_up() end

    imgui.SameLine(0, 10)

    -- Breadcrumb path bar
    local breadcrumb_parts = get_breadcrumb_parts(self.current_path)
    local available_for_breadcrumb = modal_w - 280  -- leave room for search

    imgui.PushItemWidth(available_for_breadcrumb)
    for idx, part in ipairs(breadcrumb_parts) do
        if idx > 1 then
            imgui.SameLine(0, 0)
            imgui.TextDisabled("  >  ")
            imgui.SameLine(0, 0)
        end
        if imgui.SmallButton(part.label .. "##bc" .. idx) then
            self:change_directory(part.path)
        end
    end
    imgui.PopItemWidth()

    -- Search bar flush-right
    local search_label_w = 60
    local search_input_w = 180
    imgui.SameLine(modal_w - search_label_w - search_input_w - 30)
    imgui.Text("Search:")
    imgui.SameLine(0, 4)
    imgui.SetNextItemWidth(search_input_w)
    imgui.InputText("##search", self.search_query, 256)

    imgui.Separator()

    -- ===========================================
    -- ACTION TOOLBAR
    -- ===========================================
    if not self.readonly then
        if imgui.Button("+ New Folder") then
            self.new_folder_active = true
            ffi.copy(self.new_folder_buffer, "New_Folder")
        end

        if selected_count > 0 then
            imgui.SameLine(0, 6)
            if imgui.Button("Copy")      then self:copy_to_clipboard(selected_paths, false) end
            imgui.SameLine(0, 4)
            if imgui.Button("Cut")       then self:copy_to_clipboard(selected_paths, true)  end
            imgui.SameLine(0, 4)
            if imgui.Button("Duplicate") then self:duplicate(selected_paths) end
            imgui.SameLine(0, 4)
            if imgui.Button("Delete")    then self:delete(selected_paths) end
        end

        if #self.clipboard > 0 then
            imgui.SameLine(0, 6)
            local paste_label = self.clipboard_op == "cut" and "Paste (Move)" or "Paste (Copy)"
            if imgui.Button(paste_label) then self:paste(self.current_path) end
        end
    end

    imgui.Separator()

    -- ===========================================
    -- MAIN AREA  (sidebar | file table)
    -- ===========================================
    local sidebar_w        = 160
    local content_area_h   = modal_h - 160   -- reserve top nav + toolbar + bottom bar

    -- --- LEFT SIDEBAR ---
    if imgui.BeginChild("##sidebar", imgui.ImVec2(sidebar_w, content_area_h), true, 0) then

        imgui.TextDisabled("Quick Access")
        imgui.Separator()

        local function sidebar_location(label, path)
            if imgui.Selectable(label, self.current_path == path, 0, imgui.ImVec2(0, 0)) then
                self:change_directory(path)
            end
        end

        local home = os.getenv("HOME")
        if home then sidebar_location("  Home", "os:/" .. home) end
        sidebar_location("  os://", "os://")
        sidebar_location("  save://", "save://")
        sidebar_location("  app://", "app://")
        sidebar_location("  projects://", "projects://")

        imgui.EndChild()
    end

    imgui.SameLine(0, 6)

    -- --- FILE TABLE ---
    if imgui.BeginChild("##filetable", imgui.ImVec2(0, content_area_h), true, 0) then

        -- New Folder inline input (at the top of the list)
        if self.new_folder_active then
            imgui.Text("[DIR]  ")
            imgui.SameLine()
            imgui.SetNextItemWidth(220)
            if imgui.InputText("##newfolder", self.new_folder_buffer, 256,
                               ffi.C.ImGuiInputTextFlags_EnterReturnsTrue) then
                local folder_name = ffi.string(self.new_folder_buffer)
                if folder_name ~= "" then self:create_folder(folder_name) end
                self.new_folder_active = false
            end
            imgui.Separator()
        end

        -- Column header row
        local col_name_w = 380
        local col_type_w = 90
        local col_size_w = 90

        local function sort_header(label, key, width)
            local indicator = ""
            if self.sort_by == key then
                indicator = self.sort_desc and "  v" or "  ^"
            end
            if imgui.SmallButton(label .. indicator .. "##hdr") then
                if self.sort_by == key then
                    self.sort_desc = not self.sort_desc
                else
                    self.sort_by = key
                    self.sort_desc = false
                end
                self:apply_sort()
            end
        end

        sort_header("Name", "name", col_name_w)
        imgui.SameLine(col_name_w)
        sort_header("Type", "type", col_type_w)
        imgui.SameLine(col_name_w + col_type_w)
        sort_header("Size", "size", col_size_w)
        imgui.Separator()

        -- File rows
        local search_str = ffi.string(self.search_query):lower()
        local item_trigger_rename = nil  -- deferred rename trigger

        for _, item in ipairs(self.items) do
            if search_str == "" or item.name:lower():find(search_str, 1, true) then

                local is_selected = self.selected[item.path] == true
                local icon        = item.is_dir and "[D]" or "[F]"
                local type_label  = item.is_dir and "Folder" or "File"

                -- Rename inline input
                if self.rename_active_for == item.path then
                    imgui.Text(icon .. "  ")
                    imgui.SameLine()
                    imgui.SetNextItemWidth(220)
                    if imgui.InputText("##rename_" .. item.path, self.rename_buffer, 256,
                                       ffi.C.ImGuiInputTextFlags_EnterReturnsTrue) then
                        local new_name = ffi.string(self.rename_buffer)
                        if new_name ~= "" then self:commit_rename(item.path, new_name) end
                        self.rename_active_for = nil
                    end


                else
                    -- Selectable row
                    local row_label = icon .. "  " .. item.name .. "##row_" .. item.path
                    if imgui.Selectable(row_label, is_selected,
                                        ffi.C.ImGuiSelectableFlags_AllowDoubleClick,
                                        imgui.ImVec2(col_name_w - 10, 0)) then
                        if imgui.GetIO().KeyCtrl then
                            self.selected[item.path] = not is_selected or nil
                        else
                            self.selected = { [item.path] = true }
                        end

                        if imgui.IsMouseDoubleClicked(ffi.C.ImGuiMouseButton_Left) then
                            if item.is_dir then
                                self:change_directory(item.path)
                            elseif self.mode == "file" or self.mode == "both" then
                                if self.callback then self.callback({ item.path }) end
                                self:close()
                            end
                        end
                    end

                    -- Type column
                    imgui.SameLine(col_name_w)
                    imgui.TextDisabled(type_label)

                    -- Size column
                    imgui.SameLine(col_name_w + col_type_w)
                    imgui.TextDisabled(format_size(item.size))

                    -- Drag & Drop source
                    if not self.readonly and imgui.BeginDragDropSource() then
                        imgui.SetDragDropPayload("FILEBROWSER_ITEM", item.path, #item.path + 1, ffi.C.ImGuiCond_Once)
                        imgui.Text("Move: " .. item.name)
                        imgui.EndDragDropSource()
                    end

                    -- Drag & Drop target (folders only)
                    if not self.readonly and item.is_dir and imgui.BeginDragDropTarget() then
                        local payload = imgui.AcceptDragDropPayload("FILEBROWSER_ITEM")
                        if payload ~= nil then
                            local src_path = ffi.string(payload.Data)
                            if src_path ~= item.path then
                                local drop_world = ecs.from_ptr(g_world)
                                sandbox.filesystem.move(drop_world, src_path, join_path(item.path, get_name(src_path)), false, true)
                                self:refresh()
                            end
                        end
                        imgui.EndDragDropTarget()
                    end

                    -- Context menu
                    if not self.readonly and imgui.BeginPopupContextItem("ctx##" .. item.path) then
                        if imgui.MenuItem("Rename")    then item_trigger_rename = item end
                        if imgui.MenuItem("Duplicate") then self:duplicate({ item.path }) end
                        imgui.Separator()
                        if imgui.MenuItem("Copy")      then self:copy_to_clipboard({ item.path }, false) end
                        if imgui.MenuItem("Cut")       then self:copy_to_clipboard({ item.path }, true)  end
                        imgui.Separator()
                        if imgui.MenuItem("Delete")    then self:delete({ item.path }) end
                        imgui.EndPopup()
                    end
                end
            end
        end

        -- Apply deferred rename trigger (outside context menu scope)
        if item_trigger_rename then
            self.rename_active_for = item_trigger_rename.path
            ffi.copy(self.rename_buffer, item_trigger_rename.name)
        end

        -- Root drop zone (empty space in the table)
        if not self.readonly and imgui.BeginDragDropTarget() then
            local payload = imgui.AcceptDragDropPayload("FILEBROWSER_ITEM")
            if payload ~= nil then
                local src_path = ffi.string(payload.Data)
                if parent_dir(src_path) ~= self.current_path then
                    local drop_world = ecs.from_ptr(g_world)
                    sandbox.filesystem.move(drop_world, src_path, join_path(self.current_path, get_name(src_path)), false, true)
                    self:refresh()
                end
            end
            imgui.EndDragDropTarget()
        end

        imgui.EndChild()
    end

    -- ===========================================
    -- BOTTOM BAR  (status | Select / Cancel)
    -- ===========================================
    imgui.Separator()

    -- Status text
    local status_text
    if selected_count > 0 then
        status_text = selected_count .. " item(s) selected"
    else
        status_text = #self.items .. " item(s)"
    end
    imgui.Text(status_text)
    imgui.SameLine()

    -- Select button (right-aligned)
    local button_w = 110
    local button_gap = 8
    imgui.SetCursorPosX(modal_w - (button_w * 2) - button_gap - 20)

    local can_select = selected_count > 0 or self.mode == "directory"
    if can_select then
        if imgui.Button("Select", imgui.ImVec2(button_w, 0)) then
            if selected_count > 0 then
                if self.callback then self.callback(selected_paths) end
            else
                if self.callback then self.callback({ self.current_path }) end
            end
            self:close()
        end
    else
        imgui.SmallButton("  Select  ") -- shown but inactive
    end

    imgui.SameLine(0, button_gap)
    if imgui.Button("Cancel", imgui.ImVec2(button_w, 0)) then
        self:close()
    end

    imgui.EndPopup()
    end -- end BeginPopupModal
end


-- ==========================================
-- Tests
-- ==========================================

function FileBrowser.run_test()
    local world = ecs.from_ptr(g_world)
    sandbox.logs.info(world, "[FileBrowser Test] ===== STARTING ALL TESTS =====")
    
    local fb = FileBrowser.new("both", "save://projects")
    fb.current_path = "save://projects"
    
    -- Test Folder Creation
    fb:create_folder("BrowserTestFolder")
    local test_dir = "save://projects/BrowserTestFolder"
    local created = sandbox.filesystem.exists(world, test_dir)
    if created then sandbox.logs.info(world, "[FileBrowser Test] Passed Create Folder") else sandbox.logs.error(world, "[FileBrowser Test] Failed Create Folder") end
    
    -- Test Rename
    fb:commit_rename(test_dir, "BrowserTestRenamed")
    local renamed_dir = "save://projects/BrowserTestRenamed"
    local renamed = sandbox.filesystem.exists(world, renamed_dir) and not sandbox.filesystem.exists(world, test_dir)
    if renamed then sandbox.logs.info(world, "[FileBrowser Test] Passed Rename Folder") else sandbox.logs.error(world, "[FileBrowser Test] Failed Rename Folder") end
    
    -- Test Duplicate
    fb:duplicate({renamed_dir})
    local dup_dir = "save://projects/BrowserTestRenamed_copy"
    local duplicated = sandbox.filesystem.exists(world, dup_dir)
    if duplicated then sandbox.logs.info(world, "[FileBrowser Test] Passed Duplicate Folder") else sandbox.logs.error(world, "[FileBrowser Test] Failed Duplicate Folder") end
    
    -- Test Delete
    fb:delete({renamed_dir, dup_dir})
    local deleted = not sandbox.filesystem.exists(world, renamed_dir) and not sandbox.filesystem.exists(world, dup_dir)
    if deleted then sandbox.logs.info(world, "[FileBrowser Test] Passed Delete Multiple Folders") else sandbox.logs.error(world, "[FileBrowser Test] Failed Delete Folders") end
    
    -- Test Navigation
    fb:change_directory("os://")
    fb:change_directory("os://home")
    fb:backward()
    local nav1 = (fb.current_path == "os://")
    fb:forward()
    local nav2 = (fb.current_path == "os://home")
    fb:navigate_up()
    local nav3 = (fb.current_path == "os://")
    if nav1 and nav2 and nav3 then sandbox.logs.info(world, "[FileBrowser Test] Passed Navigation History") else sandbox.logs.error(world, "[FileBrowser Test] Failed Navigation") end
    
    sandbox.logs.info(world, "[FileBrowser Test] ===== ALL TESTS FINISHED =====")
    return true
end

return FileBrowser
