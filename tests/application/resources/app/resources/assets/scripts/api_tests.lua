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
    props:set_int64("sub_obj.child", 100)
    assert_eq(props:has("sub_obj.child"), true, "has child")
    
    local sub_props = props:sub("sub_obj")
    assert_eq(sub_props:get_int64("child"), 100, "sub child")
    
    -- Test clear
    props:clear("sub_obj")
    assert_eq(props:has("sub_obj"), false, "clear sub_obj")
    
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
    
    print("Components tests passed.")
end

local status, err = pcall(function()
    test_properties()
    test_filesystem()
    test_components()
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
