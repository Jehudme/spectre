local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local world = ecs.from_ptr(g_world)

project = {}

function project.initialize()
    sandbox.logs.info(world, "[project.lua] project.initialize called!")
    local exists = sandbox.filesystem.exists(world, "cache://spectre-projects/")
    sandbox.logs.info(world, "[project.lua] exists before: " .. tostring(exists))
    if not exists then
        local created = sandbox.filesystem.create_directory(world, "cache://spectre-projects/", true)
        sandbox.logs.info(world, "[project.lua] created: " .. tostring(created))
    end
    local cache_phys = sandbox.filesystem.resolve_physical_path(world, "cache://spectre-projects/")
    sandbox.logs.info(world, "[project.lua] cache_phys: " .. tostring(cache_phys))
    sandbox.filesystem.mount(world, cache_phys, "projects://", false)
end

function project.create(name)
    local dest = "projects://" .. name
    return sandbox.filesystem.copy(world, "app://templates/new_app", dest, false, true)
end

function project.delete(name)
    local target = "projects://" .. name
    return sandbox.filesystem.remove_directory(world, target)
end

function project.rename(old_name, new_name)
    local old_dir = "projects://" .. old_name
    local new_dir = "projects://" .. new_name
    return sandbox.filesystem.move(world, old_dir, new_dir, false, true)
end

function project.list()
    local ffi = require("ffi")
    local out_dirs = ffi.new("char**[1]")
    local out_count = ffi.new("size_t[1]")
    
    local success = sandbox.filesystem.list_directories(world, "projects://", false, out_dirs, out_count)
    local result = {}
    
    if success and tonumber(out_count[0]) > 0 then
        for i = 0, tonumber(out_count[0]) - 1 do
            local path = ffi.string(out_dirs[0][i])
            -- Ensure trailing slash is removed for pattern matching
            local clean_path = path
            if string.sub(clean_path, -1) == "/" then
                clean_path = string.sub(clean_path, 1, -2)
            end
            local name = string.match(clean_path, "projects://(.+)")
            if name then
                table.insert(result, name)
            end
        end
        sandbox.filesystem.free_file_list(world, out_dirs[0], out_count[0])
    end
    
    return result
end

function project.duplicate(original)
    local idx = 1
    local new_name = original .. tostring(idx)
    while sandbox.filesystem.exists(world, "projects://" .. new_name) do
        idx = idx + 1
        new_name = original .. tostring(idx)
    end
    return sandbox.filesystem.copy(world, "projects://" .. original, "projects://" .. new_name, false, true)
end

function project.mount(name)
    local phys = sandbox.filesystem.resolve_physical_path(world, "projects://" .. name)
    sandbox.filesystem.mount(world, phys, "project://", false)
end

return project
