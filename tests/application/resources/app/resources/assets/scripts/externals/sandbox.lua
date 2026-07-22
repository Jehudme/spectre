local ffi = require("ffi")

pcall(function()
    ffi.cdef([[
        void sandbox_logs_trace(void* ecs, const char* msg);
        void sandbox_logs_debug(void* ecs, const char* msg);
        void sandbox_logs_info(void* ecs, const char* msg);
        void sandbox_logs_warn(void* ecs, const char* msg);
        void sandbox_logs_error(void* ecs, const char* msg);
    ]])
end)

local sandbox = {}
sandbox.logs = {}

function sandbox.logs.trace(msg) ffi.C.sandbox_logs_trace(g_world, tostring(msg)) end
function sandbox.logs.debug(msg) ffi.C.sandbox_logs_debug(g_world, tostring(msg)) end
function sandbox.logs.info(msg) ffi.C.sandbox_logs_info(g_world, tostring(msg)) end
function sandbox.logs.warn(msg) ffi.C.sandbox_logs_warn(g_world, tostring(msg)) end
function sandbox.logs.error(msg) ffi.C.sandbox_logs_error(g_world, tostring(msg)) end

return sandbox
