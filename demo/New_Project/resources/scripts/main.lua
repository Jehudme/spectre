local ecs = require("ecs")

local MyScript = {}

function MyScript.on_update(entity_id, greeting, number_val)
    sandbox.logs.info(ecs.from_ptr(g_world), string.format("MyScript updating entity %d! Greeting: %s, Number: %f", entity_id, greeting, number_val))
end

function MyScript.on_enter(entity_id, message)
    sandbox.logs.info(ecs.from_ptr(g_world), string.format("Entity %d entered with message: %s", entity_id, message))
end

return {
    on_update = ecs.Script.define(MyScript.on_update, "greeting:string", "number_val:number"),
    on_enter = ecs.Script.define(MyScript.on_enter, "message:string")
}
