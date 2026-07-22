---@meta

---@class ecs
ecs = {}

--- Initializes a new ECS world.
---@return ecs_world_t
function ecs.init() end

--- Initializes a new mini ECS world.
---@return ecs_world_t
function ecs.mini() end

--- Creates a Lua world wrapper from a C pointer.
---@param ptr any
---@return ecs_world_t
function ecs.from_ptr(ptr) end

---@class ecs.Script
ecs.Script = {}

--- Defines an ECS script function.
---@param func function
---@param ... string
---@return any
function ecs.Script.define(func, ...) end

---@class ecs_world_t
local ecs_world_t = {}

--- Checks if the world is finalized.
---@return boolean
function ecs_world_t:is_fini() end

--- Gets world info.
---@return any
function ecs_world_t:info() end

--- Gets world stats.
---@return any
function ecs_world_t:stats() end

--- Quits the world.
function ecs_world_t:quit() end

--- Checks if the world should quit.
---@return boolean
function ecs_world_t:should_quit() end

--- Gets all entities.
---@return table
function ecs_world_t:get_entities() end

--- Creates or gets an entity.
---@param arg1 any
---@param arg2 any
---@param arg3 any
---@return integer
function ecs_world_t:new(arg1, arg2, arg3) end

--- Looks up an entity by name.
---@param name string
---@return integer
function ecs_world_t:lookup(name) end

--- Checks if an entity has a component.
---@param entity integer
---@param component integer
---@return boolean
function ecs_world_t:has(entity, component) end

--- Gets a component from an entity.
---@param entity integer
---@param component integer
---@return any
function ecs_world_t:get(entity, component) end

--- Sets a component for an entity.
---@param entity integer
---@param component integer
---@param value any
function ecs_world_t:set(entity, component, value) end

--- Creates a new struct component.
---@param name string
---@param schema string
---@return integer
function ecs_world_t:new_struct(name, schema) end

--- Binds a C struct to a component ID.
---@param component integer
---@param struct_name string
function ecs_world_t:bind_struct(component, struct_name) end

