---@meta
local flecs = {}
local ffi = require('ffi')

flecs.ComponentIDs = {}

function flecs.get_component_id(world, name)
    local id = flecs.ComponentIDs[name]
    if not id then
        id = ffi.C.ecs_lookup_symbol(world, name, true, true)
        flecs.ComponentIDs[name] = id
    end
    return id
end

-- Re-implementation of ECS_PAIR macro for Flecs v4
function flecs.ecs_pair(rel, obj)
    -- Equivalent to ecs_make_pair
    return ffi.C.ecs_make_pair(rel, obj)
end

---@class flecs.Entity
---@field world any
---@field id number
local Entity = {}
Entity.__index = Entity
flecs.EntityMeta = Entity

---@param world any
---@param id number
---@return flecs.Entity
function flecs.Entity(world, id)
    return setmetatable({ world = world, id = id }, Entity)
end

function Entity:get_id()
    return self.id
end

function Entity:get_world()
    return self.world
end

function Entity:add(comp_id)
    ffi.C.ecs_add_id(self.world, self.id, comp_id)
    return self
end

function Entity:remove(comp_id)
    ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    return self
end

function Entity:has(comp_id)
    return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
end

function Entity:add_pair(rel_id, obj_id)
    local pair_id = flecs.ecs_pair(rel_id, obj_id)
    ffi.C.ecs_add_id(self.world, self.id, pair_id)
    return self
end

function Entity:remove_pair(rel_id, obj_id)
    local pair_id = flecs.ecs_pair(rel_id, obj_id)
    ffi.C.ecs_remove_id(self.world, self.id, pair_id)
    return self
end

function Entity:has_pair(rel_id, obj_id)
    local pair_id = flecs.ecs_pair(rel_id, obj_id)
    return ffi.C.ecs_has_id(self.world, self.id, pair_id) == true
end

function Entity:delete()
    ffi.C.ecs_delete(self.world, self.id)
end

function Entity:clear()
    ffi.C.ecs_clear(self.world, self.id)
end

function Entity:is_alive()
    return ffi.C.ecs_is_alive(self.world, self.id) == true
end

function Entity:is_valid()
    return ffi.C.ecs_is_valid(self.world, self.id) == true
end

function Entity:enable()
    ffi.C.ecs_enable(self.world, self.id, true)
    return self
end

function Entity:disable()
    ffi.C.ecs_enable(self.world, self.id, false)
    return self
end

--- Add a parent relationship
function Entity:child_of(parent_id)
    local ecs_child_of = ffi.C.ecs_lookup_symbol(self.world, "EcsChildOf", true, true)
    self:add_pair(ecs_child_of, parent_id)
    return self
end

--- Remove a parent relationship
function Entity:remove_child_of(parent_id)
    local ecs_child_of = ffi.C.ecs_lookup_symbol(self.world, "EcsChildOf", true, true)
    self:remove_pair(ecs_child_of, parent_id)
    return self
end

--- Add an IsA relationship (Prefab)
function Entity:is_a(prefab_id)
    local ecs_is_a = ffi.C.ecs_lookup_symbol(self.world, "EcsIsA", true, true)
    self:add_pair(ecs_is_a, prefab_id)
    return self
end

flecs.EntityClass = Entity

---@class flecs.Query
---@field world any
---@field expr string
local Query = {}
Query.__index = Query

function flecs.Query(world, expr)
    local q = setmetatable({ world = world, expr = expr }, Query)
    return q
end

flecs.QueryClass = Query

return flecs
