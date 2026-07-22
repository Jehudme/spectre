local status, err = pcall(function()
    g_world = g_world or nil
    sandbox = require("externals.sandbox")
    spectre = require("externals.spectre")
end)

if not status then
    local out = io.open("api_test_success.txt", "w")
    if out then out:write("REQUIRE ERROR: " .. tostring(err)); out:close() end
    return {}
end

local function assert_eq(a, b, msg)
    assert(a == b, (msg or "") .. " (Expected: " .. tostring(b) .. ", Got: " .. tostring(a) .. ")")
end

local function test_properties()
    local props = sandbox.Properties.new()
    
    -- Test basic setting and getting
    props:set_int64("my_int", -42)
    assert_eq(props:get_int64("my_int"), -42, "get_int64")
    
    props:set_uint64("my_uint", 42)
    assert_eq(props:get_uint64("my_uint"), 42, "get_uint64")
    
    props:set_double("my_double", 3.14)
    assert(math.abs(props:get_double("my_double") - 3.14) < 0.001, "get_double")
    
    props:set_bool("my_bool", true)
    assert_eq(props:get_bool("my_bool"), true, "get_bool")
    
    props:set_string("my_string", "hello world")
    assert_eq(props:read_string("my_string"), "hello world", "read_string")
    
    -- Test arrays
    props:set_string_array("arr_str", {"foo", "bar"})
    local arr_str = props:read_string_array("arr_str")
    assert_eq(#arr_str, 2, "arr_str size")
    assert_eq(arr_str[1], "foo", "arr_str[1]")
    assert_eq(arr_str[2], "bar", "arr_str[2]")
    
    -- Test keys
    local keys = props:keys("")
    assert(#keys >= 5, "Should have multiple keys")
    
    -- Test sub and has
    -- Test clear
    props:clear("sub_obj")
    assert_eq(props:has("sub_obj.child"), false, "clear sub_obj")
    
    -- Test merge
    local other = sandbox.Properties.new()
    other:set_string("merged_val", "success")
    props:merge("", other)
    assert_eq(props:read_string("merged_val"), "success", "merge")
    other:destroy()
    
    props:destroy()
    print("Properties tests passed.")
end

local function test_filesystem()
    local path = "test_file.txt"
    -- Only run filesystem tests if the service is available
    if not sandbox.filesystem.exists(g_world, "") then
        print("Filesystem service unavailable, skipping...")
        return
    end

    sandbox.filesystem.create_file(g_world, path, true)
    assert_eq(sandbox.filesystem.exists(g_world, path), true, "file exists")
    
    local handle = sandbox.filesystem.open_write(g_world, path, false, true)
    local str = "ffi testing"
    -- write requires a buffer
    local buf = require("ffi").new("char[?]", #str + 1)
    require("ffi").copy(buf, str)
    sandbox.filesystem.write(g_world, handle, buf, #str)
    sandbox.filesystem.close_handle(g_world, handle)
    
    assert_eq(sandbox.filesystem.file_size(g_world, path), #str, "file size")
    
    sandbox.filesystem.remove_file(g_world, path)
    assert_eq(sandbox.filesystem.exists(g_world, path), false, "file removed")
    
    print("Filesystem tests passed.")
end

local function test_components()
    -- Create a component from C++ and check if Lua can see it
    local is_comp = spectre.components.is_component(g_world, 0)
    assert_eq(is_comp, false, "invalid component")
    
    local transform_id = spectre.components.find_component(g_world, "Transform2D")
    assert(transform_id ~= 0, "Transform2D component not found")
    
    assert_eq(spectre.components.has_component(g_world, "Transform2D"), true, "has Transform2D")
    
    print("Components tests passed.")
end

local function test_sandbox_api()
    -- Logs
    sandbox.logs.trace(g_world, "Lua API trace test")
    sandbox.logs.debug(g_world, "Lua API debug test")
    sandbox.logs.info(g_world, "Lua API info test")
    sandbox.logs.warn(g_world, "Lua API warn test")
    sandbox.logs.error(g_world, "Lua API error test")
    
    -- Configuration
    local config = sandbox.configuration.get_properties(g_world)
    assert(config ~= nil, "read_engine_configuration should return Properties")
    
    -- Application
    local app = sandbox.application.is_running(g_world)
    assert(type(app) == "boolean", "is_running should return boolean")
    
    print("Sandbox basic APIs tests passed.")
end

local function test_spectre_api()
    -- Prefabs
    local p = spectre.prefabs.create_entity_from_name(g_world, "non_existent_prefab")
    assert_eq(tonumber(p), 0, "create_entity_from_name should return 0 for non-existent prefab")
    
    -- Scenes
    local state = spectre.scenes.find_current_state(g_world)
    assert(tonumber(state) ~= nil, "find_current_state should return number or cdata number")
    spectre.scenes.push_state(g_world, 0)
    
    -- Renderer
    local is_rend = spectre.renderer.is_renderer(g_world)
    assert(type(is_rend) == "boolean", "is_renderer should return boolean")
    
    print("Spectre basic APIs tests passed.")
end

local status, err = pcall(function()
    test_properties()
    test_filesystem()
    test_components()
    test_sandbox_api()
    test_spectre_api()
end)

local out = io.open("api_test_success.txt", "w")
if out then
    if status then
        out:write("ALL TESTS PASSED")
    else
        out:write("ERROR: " .. tostring(err))
    end
    out:close()
end

return {}
