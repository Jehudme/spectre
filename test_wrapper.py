import re

def generate_wrapper(structs):
    lines = []
    lines.append("local ffi = require('ffi')")
    lines.append("local spectre = {}")
    lines.append("spectre.ComponentIDs = {}")
    lines.append("")
    lines.append("---@class spectre.Entity")
    lines.append("---@field world any")
    lines.append("---@field id number")
    lines.append("local Entity = {}")
    lines.append("Entity.__index = Entity")
    lines.append("")
    lines.append("function spectre.get_component_id(world, name)")
    lines.append("    local id = spectre.ComponentIDs[name]")
    lines.append("    if not id then")
    lines.append("        id = ffi.C.ecs_lookup_symbol(world, name, true, true)")
    lines.append("        spectre.ComponentIDs[name] = id")
    lines.append("    end")
    lines.append("    return id")
    lines.append("end")
    lines.append("")
    lines.append("---@param world any")
    lines.append("---@param id number")
    lines.append("---@return spectre.Entity")
    lines.append("function spectre.Entity(world, id)")
    lines.append("    return setmetatable({ world = world, id = id }, Entity)")
    lines.append("end")
    lines.append("")
    
    for s in structs:
        # getter
        lines.append(f"---@return {s}|nil")
        lines.append(f"function Entity:get_{s}()")
        lines.append(f"    local comp_id = spectre.get_component_id(self.world, '{s}')")
        lines.append(f"    if comp_id == 0 then return nil end")
        lines.append(f"    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)")
        lines.append(f"    if ptr ~= nil then return ffi.cast('{s}*', ptr) end")
        lines.append(f"    return nil")
        lines.append(f"end")
        lines.append("")
        
        # modified marker
        lines.append(f"function Entity:mark_{s}_modified()")
        lines.append(f"    local comp_id = spectre.get_component_id(self.world, '{s}')")
        lines.append(f"    if comp_id ~= 0 then")
        lines.append(f"        ffi.C.ecs_modified_id(self.world, self.id, comp_id)")
        lines.append(f"    end")
        lines.append(f"end")
        lines.append("")

    lines.append("return spectre")
    return "\n".join(lines)

print(generate_wrapper(["spectre_2D_transform_component_t"]))
