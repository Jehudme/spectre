import os

def append_properties_wrapper(filename):
    with open(filename, 'a') as f:
        f.write("""
-- ========================================
-- Properties Wrapper Class
-- ========================================
sandbox.Properties = {}
sandbox.Properties.__index = sandbox.Properties

function sandbox.Properties.new()
    local instance = { handle = ffi.C.sandbox_properties_create() }
    setmetatable(instance, sandbox.Properties)
    return instance
end

function sandbox.Properties:destroy()
    local ptr = ffi.new("sandbox_properties_handle_t[1]", self.handle)
    ffi.C.sandbox_properties_destroy(ptr)
end

function sandbox.Properties:load(data, fmt)
    return ffi.C.sandbox_properties_load(self.handle, data, #data, fmt or 0)
end

function sandbox.Properties:dump(fmt)
    local str = ffi.C.sandbox_properties_dump(self.handle, fmt or 0)
    if str ~= nil then
        local res = ffi.string(str)
        ffi.C.sandbox_properties_free_string(str)
        return res
    end
    return nil
end

function sandbox.Properties:clear(path)
    ffi.C.sandbox_properties_clear(self.handle, path)
end

function sandbox.Properties:has(path)
    return ffi.C.sandbox_properties_has(self.handle, path)
end

function sandbox.Properties:sub(path)
    local sub_handle = ffi.C.sandbox_properties_sub(self.handle, path)
    local instance = { handle = sub_handle }
    setmetatable(instance, sandbox.Properties)
    return instance
end

function sandbox.Properties:get_int64(path)
    local out = ffi.new("int64_t[1]")
    if ffi.C.sandbox_properties_get_int64(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

function sandbox.Properties:get_uint64(path)
    local out = ffi.new("uint64_t[1]")
    if ffi.C.sandbox_properties_get_uint64(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

function sandbox.Properties:get_double(path)
    local out = ffi.new("double[1]")
    if ffi.C.sandbox_properties_get_double(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

function sandbox.Properties:get_bool(path)
    local out = ffi.new("bool[1]")
    if ffi.C.sandbox_properties_get_bool(self.handle, path, out) then
        return out[0]
    end
    return nil
end

function sandbox.Properties:read_string(path)
    local res = nil
    local cb = ffi.cast("void (*)(const char*, void*)", function(val, ctx)
        if val ~= nil then
            res = ffi.string(val)
        end
    end)
    ffi.C.sandbox_properties_read_string(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:read_int64_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(int64_t, void*)", function(val, ctx) table.insert(res, tonumber(val)) end)
    ffi.C.sandbox_properties_read_int64_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:read_uint64_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(uint64_t, void*)", function(val, ctx) table.insert(res, tonumber(val)) end)
    ffi.C.sandbox_properties_read_uint64_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:read_double_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(double, void*)", function(val, ctx) table.insert(res, tonumber(val)) end)
    ffi.C.sandbox_properties_read_double_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:read_bool_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(bool, void*)", function(val, ctx) table.insert(res, val) end)
    ffi.C.sandbox_properties_read_bool_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:read_string_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(const char*, void*)", function(val, ctx) 
        if val ~= nil then table.insert(res, ffi.string(val)) end 
    end)
    ffi.C.sandbox_properties_read_string_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:set_int64(path, val)
    ffi.C.sandbox_properties_set_int64(self.handle, path, val)
end

function sandbox.Properties:set_uint64(path, val)
    ffi.C.sandbox_properties_set_uint64(self.handle, path, val)
end

function sandbox.Properties:set_double(path, val)
    ffi.C.sandbox_properties_set_double(self.handle, path, val)
end

function sandbox.Properties:set_bool(path, val)
    ffi.C.sandbox_properties_set_bool(self.handle, path, val)
end

function sandbox.Properties:set_string(path, val)
    ffi.C.sandbox_properties_set_string(self.handle, path, val)
end

function sandbox.Properties:set_int64_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("int64_t[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_int64_array(self.handle, path, c_arr, count)
end

function sandbox.Properties:set_uint64_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("uint64_t[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_uint64_array(self.handle, path, c_arr, count)
end

function sandbox.Properties:set_double_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("double[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_double_array(self.handle, path, c_arr, count)
end

function sandbox.Properties:set_bool_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("bool[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_bool_array(self.handle, path, c_arr, count)
end

function sandbox.Properties:set_string_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("const char*[?]", count)
    for i=1, count do c_arr[i-1] = ffi.cast("const char*", arr[i]) end
    ffi.C.sandbox_properties_set_string_array(self.handle, path, c_arr, count)
end

function sandbox.Properties:keys(path)
    local res = {}
    local cb = ffi.cast("void (*)(const char*, void*)", function(key, ctx) 
        if key ~= nil then table.insert(res, ffi.string(key)) end 
    end)
    ffi.C.sandbox_properties_keys(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:merge(path, other_props)
    ffi.C.sandbox_properties_merge(self.handle, path, other_props.handle)
end

function sandbox.Properties:get_handle()
    return self.handle
end

return sandbox
""")

def replace_properties():
    with open("tests/application/resources/app/resources/assets/scripts/externals/sandbox.lua", "r") as f:
        content = f.read()
    
    idx = content.find("-- Properties Wrapper Class")
    if idx != -1:
        # Also find the ============= above it
        start_idx = content.rfind("-- =", 0, idx)
        if start_idx != -1:
            content = content[:start_idx]
    
    with open("tests/application/resources/app/resources/assets/scripts/externals/sandbox.lua", "w") as f:
        f.write(content.strip())
        
    append_properties_wrapper("tests/application/resources/app/resources/assets/scripts/externals/sandbox.lua")
        
replace_properties()
