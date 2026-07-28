local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local world = ecs.from_ptr(g_world)

-- Initialize spectre components
spectre.init(world)

Components = {}
Component = {}
Variable = {
	name = nil,
	type = nil,
	value = nil,
}
