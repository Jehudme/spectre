local ffi = require("ffi")

---@param entity number
function on_update(entity)
    ffi.C.sandbox_logs_info(g_world, "Executing on_update for entity9898: " .. tostring(entity))
end
