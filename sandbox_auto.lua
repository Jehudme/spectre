local ffi = require('ffi')

local sandbox = {}

sandbox.runtime = {}
function sandbox.runtime.run(ecs)
    return ffi.C.sandbox_runtime_run(ecs)
end
function sandbox.runtime.start(ecs)
    return ffi.C.sandbox_runtime_start(ecs)
end
function sandbox.runtime.stop(ecs)
    return ffi.C.sandbox_runtime_stop(ecs)
end
function sandbox.runtime.pause(ecs)
    return ffi.C.sandbox_runtime_pause(ecs)
end
function sandbox.runtime.resume(ecs)
    return ffi.C.sandbox_runtime_resume(ecs)
end

sandbox.application = {}
function sandbox.application.is_running(ecs)
    return ffi.C.sandbox_application_is_running(ecs)
end

sandbox.filesystem = {}
function sandbox.filesystem.mount(ecs, physical_path, virtual_mount_point, read_only)
    return ffi.C.sandbox_filesystem_mount(ecs, physical_path, virtual_mount_point, read_only)
end
function sandbox.filesystem.unmount(ecs, mount_point)
    return ffi.C.sandbox_filesystem_unmount(ecs, mount_point)
end
function sandbox.filesystem.open_read(ecs, virtual_path)
    return ffi.C.sandbox_filesystem_open_read(ecs, virtual_path)
end
function sandbox.filesystem.open_write(ecs, virtual_path, append, force_path)
    return ffi.C.sandbox_filesystem_open_write(ecs, virtual_path, append, force_path)
end
function sandbox.filesystem.read(ecs, handle, buffer, bytes_to_read)
    return ffi.C.sandbox_filesystem_read(ecs, handle, buffer, bytes_to_read)
end
function sandbox.filesystem.write(ecs, handle, buffer, bytes_to_write)
    return ffi.C.sandbox_filesystem_write(ecs, handle, buffer, bytes_to_write)
end
function sandbox.filesystem.eof(ecs, handle)
    return ffi.C.sandbox_filesystem_eof(ecs, handle)
end
function sandbox.filesystem.tell(ecs, handle)
    return ffi.C.sandbox_filesystem_tell(ecs, handle)
end
function sandbox.filesystem.seek(ecs, handle, position)
    return ffi.C.sandbox_filesystem_seek(ecs, handle, position)
end
function sandbox.filesystem.size(ecs, handle)
    return ffi.C.sandbox_filesystem_size(ecs, handle)
end
function sandbox.filesystem.close_handle(ecs, handle)
    return ffi.C.sandbox_filesystem_close_handle(ecs, handle)
end
function sandbox.filesystem.create_file(ecs, virtual_path, force_path)
    return ffi.C.sandbox_filesystem_create_file(ecs, virtual_path, force_path)
end
function sandbox.filesystem.remove_file(ecs, virtual_path)
    return ffi.C.sandbox_filesystem_remove_file(ecs, virtual_path)
end
function sandbox.filesystem.copy(ecs, source_virtual_path, dest_virtual_path, overwrite, force_path)
    return ffi.C.sandbox_filesystem_copy(ecs, source_virtual_path, dest_virtual_path, overwrite, force_path)
end
function sandbox.filesystem.move(ecs, source_virtual_path, dest_virtual_path, overwrite, force_path)
    return ffi.C.sandbox_filesystem_move(ecs, source_virtual_path, dest_virtual_path, overwrite, force_path)
end
function sandbox.filesystem.create_directory(ecs, virtual_path, force_path)
    return ffi.C.sandbox_filesystem_create_directory(ecs, virtual_path, force_path)
end
function sandbox.filesystem.remove_directory(ecs, virtual_path)
    return ffi.C.sandbox_filesystem_remove_directory(ecs, virtual_path)
end
function sandbox.filesystem.exists(ecs, virtual_path)
    return ffi.C.sandbox_filesystem_exists(ecs, virtual_path)
end
function sandbox.filesystem.is_file(ecs, virtual_path)
    return ffi.C.sandbox_filesystem_is_file(ecs, virtual_path)
end
function sandbox.filesystem.is_directory(ecs, virtual_path)
    return ffi.C.sandbox_filesystem_is_directory(ecs, virtual_path)
end
function sandbox.filesystem.is_readonly(ecs, virtual_path)
    return ffi.C.sandbox_filesystem_is_readonly(ecs, virtual_path)
end
function sandbox.filesystem.file_size(ecs, virtual_path)
    return ffi.C.sandbox_filesystem_file_size(ecs, virtual_path)
end
function sandbox.filesystem.last_modified(ecs, virtual_path)
    return ffi.C.sandbox_filesystem_last_modified(ecs, virtual_path)
end
function sandbox.filesystem.list_files(ecs, virtual_path, recursive, out_files, out_count)
    return ffi.C.sandbox_filesystem_list_files(ecs, virtual_path, recursive, out_files, out_count)
end
function sandbox.filesystem.free_file_list(ecs, files, count)
    return ffi.C.sandbox_filesystem_free_file_list(ecs, files, count)
end
function sandbox.filesystem.read_all_bytes(ecs, virtual_path, out_data, out_size)
    return ffi.C.sandbox_filesystem_read_all_bytes(ecs, virtual_path, out_data, out_size)
end
function sandbox.filesystem.free_bytes(ecs, data)
    return ffi.C.sandbox_filesystem_free_bytes(ecs, data)
end

sandbox.configuration = {}
function sandbox.configuration.get_properties(ecs)
    return ffi.C.sandbox_configuration_get_properties(ecs)
end

sandbox.logs = {}
function sandbox.logs.trace(ecs, msg)
    return ffi.C.sandbox_logs_trace(ecs, msg)
end
function sandbox.logs.debug(ecs, msg)
    return ffi.C.sandbox_logs_debug(ecs, msg)
end
function sandbox.logs.info(ecs, msg)
    return ffi.C.sandbox_logs_info(ecs, msg)
end
function sandbox.logs.warn(ecs, msg)
    return ffi.C.sandbox_logs_warn(ecs, msg)
end
function sandbox.logs.error(ecs, msg)
    return ffi.C.sandbox_logs_error(ecs, msg)
end

sandbox.stage = {}
function sandbox.stage.service(info)
    return ffi.C.sandbox_stage_service(info)
end
function sandbox.stage.module(info)
    return ffi.C.sandbox_stage_module(info)
end

sandbox.index = {}
function sandbox.index.library(ecs, library_path)
    return ffi.C.sandbox_index_library(ecs, library_path)
end

sandbox.get = {}
function sandbox.get.bootstrapper(ecs)
    return ffi.C.sandbox_get_bootstrapper(ecs)
end

sandbox.bootstrapper = {}
function sandbox.bootstrapper.activate(bootstrapper, ecs, architecture, name, version_major, version_minor, version_patch)
    return ffi.C.sandbox_bootstrapper_activate(bootstrapper, ecs, architecture, name, version_major, version_minor, version_patch)
end
function sandbox.bootstrapper.activate_string(bootstrapper, ecs, module_str)
    return ffi.C.sandbox_bootstrapper_activate_string(bootstrapper, ecs, module_str)
end
function sandbox.bootstrapper.boot(bootstrapper, ecs)
    return ffi.C.sandbox_bootstrapper_boot(bootstrapper, ecs)
end

return sandbox
