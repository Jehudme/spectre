local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local world = ecs.from_ptr(g_world)

-- Initialize spectre components
spectre.init(world)

ProjectMenu = {}

function ProjectMenu.on_create(scene_id) end

function ProjectMenu.on_update(scene_id) end

function ProjectMenu.on_updat(scene_id) end

function open_project(project_filepath) end

function close_projec() end
