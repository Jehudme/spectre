local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local world = ecs.from_ptr(g_world)

project = {}

-- TODO: Refactor to create 
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

-- TODO: Refactor to
function project.create(name)
    local dest = "projects://" .. name
    local res = sandbox.filesystem.copy(world, "app://templates/new_app", dest, false, true)
    if not res then
        sandbox.logs.error(world, "[project.lua] Failed to create project '" .. name .. "' (copy failed from app://templates/new_app)")
    end
    return res
end

function project.delete(name)
    local target = "projects://" .. name
    local res = sandbox.filesystem.remove_directory(world, target)
    if not res then
        sandbox.logs.error(world, "[project.lua] Failed to delete project '" .. name .. "'")
    end
    return res
end

function project.rename(old_name, new_name)
    local old_dir = "projects://" .. old_name
    local new_dir = "projects://" .. new_name
    local res = sandbox.filesystem.move(world, old_dir, new_dir, false, true)
    if not res then
        sandbox.logs.error(world, "[project.lua] Failed to rename project '" .. old_name .. "' to '" .. new_name .. "'")
    end
    return res
end

function project.list()
    local dirs = sandbox.filesystem.list_directories(world, "projects://", false)
    local result = {}
    
    for _, path in ipairs(dirs) do
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
    
    return result
end

function project.duplicate(original)
    local idx = 1
    local new_name = original .. tostring(idx)
    while sandbox.filesystem.exists(world, "projects://" .. new_name) do
        idx = idx + 1
        new_name = original .. tostring(idx)
    end
    local res = sandbox.filesystem.copy(world, "projects://" .. original, "projects://" .. new_name, false, true)
    if not res then
        sandbox.logs.error(world, "[project.lua] Failed to duplicate project '" .. original .. "'")
    end
    return res
end

function project.mount(name)
    local phys = sandbox.filesystem.resolve_physical_path(world, "projects://" .. name)
    sandbox.filesystem.mount(world, phys, "project://", false)
end

return project
