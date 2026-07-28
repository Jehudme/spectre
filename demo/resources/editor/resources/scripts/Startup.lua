local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
local world = ecs.from_ptr(g_world)

-- Initialize spectre components
spectre.init(world)

Project = {}
MainMenu = {}

function Project.create(directory_path) end
function Project.delete(directory_path) end
function Project.rename(directory_path, new_name) end

function Project.import(directory_path) end
function Project.export(directory_path) end

function MainMenu.on_create() end
function MainMenu.on_update() end
function MainMenu.on_delete() end
