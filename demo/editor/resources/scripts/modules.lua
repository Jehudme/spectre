local ecs = require("ecs")
local sandbox = require("sandbox")
local world = ecs.from_ptr(g_world)

_G.modules = _G.modules or {}

local files = sandbox.filesystem.list_files(world, "app://resources/scripts/modules/", false)
for _, path in ipairs(files) do
    if path:match("%.lua$") then
        local module_name = path:match("([^/]+)%.lua$")
        require("modules." .. module_name)
    end
end

return {}
