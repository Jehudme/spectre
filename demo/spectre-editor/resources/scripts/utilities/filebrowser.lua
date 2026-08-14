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

function FileBrowser.new(mode, start_path)
    local instance = setmetatable({}, FileBrowser)
    instance.mode = mode or "file"
    instance.current_path = start_path or "os://"
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
-- Render
-- ==========================================

function FileBrowser:render()
    if not self.is_open then return end
    
    local w_width = spectre.window.get_width(g_world)
    local w_height = spectre.window.get_height(g_world)
    imgui.SetNextWindowSize(imgui.ImVec2(math.floor(w_width * 0.8), math.floor(w_height * 0.8)), ffi.C.ImGuiCond_FirstUseEver)
    
    if imgui.BeginPopupModal("File Browser", nil, ffi.C.ImGuiWindowFlags_NoSavedSettings) then
        
        -- Top bar
        if imgui.Button("<-") then self:backward() end
        imgui.SameLine()
        if imgui.Button("->") then self:forward() end
        imgui.SameLine()
        if imgui.Button("Home") then self:go_home() end
        imgui.SameLine()
        if imgui.Button("Up") then self:navigate_up() end
        imgui.SameLine()
        imgui.Text("Path: " .. self.current_path)
        
        imgui.SameLine(imgui.GetWindowWidth() - 250)
        imgui.Text("Search:")
        imgui.SameLine()
        imgui.InputText("##search", self.search_query, 256)
        
        imgui.Separator()
        
        -- Action bar
        if imgui.Button("New Folder") then
            self.new_folder_active = true
            ffi.copy(self.new_folder_buffer, "New_Folder")
        end
        imgui.SameLine()
        local sel_count = 0
        local sel_paths = {}
        for k, v in pairs(self.selected) do
            if v then
                sel_count = sel_count + 1
                table.insert(sel_paths, k)
            end
        end
        
        if sel_count > 0 then
            if imgui.Button("Copy") then self:copy_to_clipboard(sel_paths, false) end
            imgui.SameLine()
            if imgui.Button("Cut") then self:copy_to_clipboard(sel_paths, true) end
            imgui.SameLine()
            if imgui.Button("Delete") then self:delete(sel_paths) end
            imgui.SameLine()
            if imgui.Button("Duplicate") then self:duplicate(sel_paths) end
            imgui.SameLine()
        end
        if #self.clipboard > 0 then
            if imgui.Button("Paste") then self:paste(self.current_path) end
            imgui.SameLine()
        end
        
        imgui.Separator()
        
        -- Header for columns
        if imgui.Button("Name##sort") then self.sort_by = "name"; self.sort_desc = not self.sort_desc; self:apply_sort() end
        imgui.SameLine(300)
        if imgui.Button("Size##sort") then self.sort_by = "size"; self.sort_desc = not self.sort_desc; self:apply_sort() end
        imgui.SameLine(400)
        if imgui.Button("Type##sort") then self.sort_by = "type"; self.sort_desc = not self.sort_desc; self:apply_sort() end
        imgui.Separator()
        
        if imgui.BeginChild("BrowserList", imgui.ImVec2(0, -imgui.GetFrameHeightWithSpacing() - 20), true, 0) then
            
            -- New Folder input
            if self.new_folder_active then
                imgui.Text("[DIR]")
                imgui.SameLine()
                if imgui.InputText("##newfolder", self.new_folder_buffer, 256, ffi.C.ImGuiInputTextFlags_EnterReturnsTrue) then
                    local nm = ffi.string(self.new_folder_buffer)
                    if nm ~= "" then self:create_folder(nm) end
                    self.new_folder_active = false
                end
            end
            
            local search_str = ffi.string(self.search_query):lower()
            
            for i, item in ipairs(self.items) do
                if search_str == "" or item.name:lower():find(search_str, 1, true) then
                    
                    if self.rename_active_for == item.path then
                        imgui.Text(item.is_dir and "[DIR]" or "[FILE]")
                        imgui.SameLine()
                        if imgui.InputText("##rename", self.rename_buffer, 256, ffi.C.ImGuiInputTextFlags_EnterReturnsTrue) then
                            local nm = ffi.string(self.rename_buffer)
                            if nm ~= "" then self:commit_rename(item.path, nm) end
                            self.rename_active_for = nil
                        end
                    else
                        local display_name = (item.is_dir and "[DIR] " or "[FILE] ") .. item.name
                        local is_selected = self.selected[item.path] == true
                        
                        if imgui.Selectable(display_name .. "##" .. item.path, is_selected, ffi.C.ImGuiSelectableFlags_AllowDoubleClick) then
                            -- handle multi select with ctrl/shift? simplistic toggle for now
                            if imgui.GetIO().KeyCtrl then
                                self.selected[item.path] = not is_selected
                            else
                                self.selected = { [item.path] = true }
                            end
                            
                            if imgui.IsMouseDoubleClicked(ffi.C.ImGuiMouseButton_Left) then
                                if item.is_dir then
                                    self:change_directory(item.path)
                                else
                                    if self.mode == "file" or self.mode == "both" then
                                        if self.callback then self.callback({item.path}) end
                                        self:close()
                                    end
                                end
                            end
                        end
                        
                        -- Drag & Drop
                        if imgui.BeginDragDropSource() then
                            -- Send the string path
                            imgui.SetDragDropPayload("FILEBROWSER_ITEM", item.path, string.len(item.path) + 1, ffi.C.ImGuiCond_Once)
                            imgui.Text("Move " .. item.name)
                            imgui.EndDragDropSource()
                        end
                        
                        if item.is_dir and imgui.BeginDragDropTarget() then
                            local payload = imgui.AcceptDragDropPayload("FILEBROWSER_ITEM")
                            if payload ~= nil then
                                local src = ffi.string(payload.Data)
                                if src ~= item.path then
                                    local world = ecs.from_ptr(g_world)
                                    sandbox.filesystem.move(world, src, join_path(item.path, get_name(src)), false, true)
                                    self:refresh()
                                end
                            end
                            imgui.EndDragDropTarget()
                        end
                        
                        -- Context menu
                        if imgui.BeginPopupContextItem("Context##" .. item.path) then
                            if imgui.MenuItem("Rename") then
                                self.rename_active_for = item.path
                                ffi.copy(self.rename_buffer, item.name)
                            end
                            if imgui.MenuItem("Delete") then self:delete({item.path}) end
                            if imgui.MenuItem("Duplicate") then self:duplicate({item.path}) end
                            if imgui.MenuItem("Copy") then self:copy_to_clipboard({item.path}, false) end
                            if imgui.MenuItem("Cut") then self:copy_to_clipboard({item.path}, true) end
                            imgui.EndPopup()
                        end
                        
                        -- Draw Size and Type in same line
                        imgui.SameLine(300)
                        imgui.Text(tostring(item.size) .. " B")
                        imgui.SameLine(400)
                        imgui.Text(item.is_dir and "Folder" or "File")
                    end
                end
            end
            
            imgui.EndChild()
        end
        
        -- Root drop target
        if imgui.BeginDragDropTarget() then
            local payload = imgui.AcceptDragDropPayload("FILEBROWSER_ITEM")
            if payload ~= nil then
                local src = ffi.string(payload.Data)
                if parent_dir(src) ~= self.current_path then
                    local world = ecs.from_ptr(g_world)
                    sandbox.filesystem.move(world, src, join_path(self.current_path, get_name(src)), false, true)
                    self:refresh()
                end
            end
            imgui.EndDragDropTarget()
        end
        
        imgui.Separator()
        
        if imgui.Button("Select", imgui.ImVec2(120, 0)) then
            if sel_count > 0 then
                if self.callback then self.callback(sel_paths) end
                self:close()
            elseif self.mode == "directory" then
                if self.callback then self.callback({self.current_path}) end
                self:close()
            end
        end
        
        imgui.SameLine()
        if imgui.Button("Cancel", imgui.ImVec2(120, 0)) then
            self:close()
        end
        
        imgui.EndPopup()
    end
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
