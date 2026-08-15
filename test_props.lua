local sandbox = require("sandbox")
local props = sandbox.Properties.new()
props:set_string("entities/Main/components/scripts/dummy", "0")
props:clear("entities/Main/components/scripts/dummy")

local list_path = "entities/Main/components/scripts/on_create"
local max_k = 0
local script_path = list_path .. "/" .. max_k

props:set_string(script_path .. "/function", "spawn_asteroid")
props:set_string(script_path .. "/arguments/dummy", "0")
props:clear(script_path .. "/arguments/dummy")
props:set_string(script_path .. "/arguments/position_x", "")

print(props:dump(0))
