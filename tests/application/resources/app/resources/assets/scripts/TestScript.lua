local spectre = require("spectre")
local ffi = require("ffi") 

---@param entity entity
function on_update(entity)
    local ent = spectre.Entity(g_world, entity)
    local transform = ent:get_spectre_2D_transform_component_t()
    if transform then
        transform.position_x = ffi.C.spectre_window_get_mouse_x(g_world)
        transform.position_y = ffi.C.spectre_window_get_mouse_y(g_world)
        ent:mark_spectre_2D_transform_component_t_modified()
    end
end
