local ffi = require('ffi')
local spectre = {}
spectre.ComponentIDs = {}

---@class spectre.Entity
---@field world any
---@field id number
local Entity = {}
Entity.__index = Entity

function spectre.get_component_id(world, name)
    local id = spectre.ComponentIDs[name]
    if not id then
        id = ffi.C.ecs_lookup_symbol(world, name, true, true)
        spectre.ComponentIDs[name] = id
    end
    return id
end

---@param world any
---@param id number
---@return spectre.Entity
function spectre.Entity(world, id)
    return setmetatable({ world = world, id = id }, Entity)
end

---@return ecs_header_t|nil
function Entity:get_ecs_header_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_header_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_header_t*', ptr) end
    return nil
end

function Entity:mark_ecs_header_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_header_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_vec_t|nil
function Entity:get_ecs_vec_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_vec_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_vec_t*', ptr) end
    return nil
end

function Entity:mark_ecs_vec_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_vec_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_sparse_page_t|nil
function Entity:get_ecs_sparse_page_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_sparse_page_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_sparse_page_t*', ptr) end
    return nil
end

function Entity:mark_ecs_sparse_page_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_sparse_page_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_sparse_t|nil
function Entity:get_ecs_sparse_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_sparse_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_sparse_t*', ptr) end
    return nil
end

function Entity:mark_ecs_sparse_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_sparse_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_block_allocator_block_t|nil
function Entity:get_ecs_block_allocator_block_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_block_allocator_block_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_block_allocator_block_t*', ptr) end
    return nil
end

function Entity:mark_ecs_block_allocator_block_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_block_allocator_block_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_block_allocator_chunk_header_t|nil
function Entity:get_ecs_block_allocator_chunk_header_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_block_allocator_chunk_header_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_block_allocator_chunk_header_t*', ptr) end
    return nil
end

function Entity:mark_ecs_block_allocator_chunk_header_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_block_allocator_chunk_header_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_block_allocator_t|nil
function Entity:get_ecs_block_allocator_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_block_allocator_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_block_allocator_t*', ptr) end
    return nil
end

function Entity:mark_ecs_block_allocator_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_block_allocator_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_stack_page_t|nil
function Entity:get_ecs_stack_page_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_stack_page_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_stack_page_t*', ptr) end
    return nil
end

function Entity:mark_ecs_stack_page_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_stack_page_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_stack_cursor_t|nil
function Entity:get_ecs_stack_cursor_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_stack_cursor_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_stack_cursor_t*', ptr) end
    return nil
end

function Entity:mark_ecs_stack_cursor_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_stack_cursor_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_stack_t|nil
function Entity:get_ecs_stack_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_stack_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_stack_t*', ptr) end
    return nil
end

function Entity:mark_ecs_stack_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_stack_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_bucket_entry_t|nil
function Entity:get_ecs_bucket_entry_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_bucket_entry_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_bucket_entry_t*', ptr) end
    return nil
end

function Entity:mark_ecs_bucket_entry_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_bucket_entry_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_bucket_t|nil
function Entity:get_ecs_bucket_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_bucket_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_bucket_t*', ptr) end
    return nil
end

function Entity:mark_ecs_bucket_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_bucket_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_map_iter_t|nil
function Entity:get_ecs_map_iter_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_map_iter_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_map_iter_t*', ptr) end
    return nil
end

function Entity:mark_ecs_map_iter_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_map_iter_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_strbuf_list_elem|nil
function Entity:get_ecs_strbuf_list_elem()
    local comp_id = spectre.get_component_id(self.world, 'ecs_strbuf_list_elem')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_strbuf_list_elem*', ptr) end
    return nil
end

function Entity:mark_ecs_strbuf_list_elem_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_strbuf_list_elem')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_strbuf_t|nil
function Entity:get_ecs_strbuf_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_strbuf_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_strbuf_t*', ptr) end
    return nil
end

function Entity:mark_ecs_strbuf_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_strbuf_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_time_t|nil
function Entity:get_ecs_time_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_time_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_time_t*', ptr) end
    return nil
end

function Entity:mark_ecs_time_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_time_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_os_api_t|nil
function Entity:get_ecs_os_api_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_os_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_os_api_t*', ptr) end
    return nil
end

function Entity:mark_ecs_os_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_os_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_term_ref_t|nil
function Entity:get_ecs_term_ref_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_term_ref_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_term_ref_t*', ptr) end
    return nil
end

function Entity:mark_ecs_term_ref_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_term_ref_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_event_record_t|nil
function Entity:get_ecs_event_record_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_event_record_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_event_record_t*', ptr) end
    return nil
end

function Entity:mark_ecs_event_record_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_event_record_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_table_range_t|nil
function Entity:get_ecs_table_range_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_table_range_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_table_range_t*', ptr) end
    return nil
end

function Entity:mark_ecs_table_range_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_table_range_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_var_t|nil
function Entity:get_ecs_var_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_var_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_var_t*', ptr) end
    return nil
end

function Entity:mark_ecs_var_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_var_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_page_iter_t|nil
function Entity:get_ecs_page_iter_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_page_iter_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_page_iter_t*', ptr) end
    return nil
end

function Entity:mark_ecs_page_iter_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_page_iter_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_worker_iter_t|nil
function Entity:get_ecs_worker_iter_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_worker_iter_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_worker_iter_t*', ptr) end
    return nil
end

function Entity:mark_ecs_worker_iter_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_worker_iter_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_table_cache_iter_t|nil
function Entity:get_ecs_table_cache_iter_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_table_cache_iter_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_table_cache_iter_t*', ptr) end
    return nil
end

function Entity:mark_ecs_table_cache_iter_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_table_cache_iter_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_each_iter_t|nil
function Entity:get_ecs_each_iter_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_each_iter_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_each_iter_t*', ptr) end
    return nil
end

function Entity:mark_ecs_each_iter_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_each_iter_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_query_op_profile_t|nil
function Entity:get_ecs_query_op_profile_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_op_profile_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_query_op_profile_t*', ptr) end
    return nil
end

function Entity:mark_ecs_query_op_profile_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_op_profile_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_query_iter_t|nil
function Entity:get_ecs_query_iter_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_iter_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_query_iter_t*', ptr) end
    return nil
end

function Entity:mark_ecs_query_iter_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_iter_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_commands_t|nil
function Entity:get_ecs_commands_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_commands_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_commands_t*', ptr) end
    return nil
end

function Entity:mark_ecs_commands_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_commands_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_suspend_readonly_state_t|nil
function Entity:get_ecs_suspend_readonly_state_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_suspend_readonly_state_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_suspend_readonly_state_t*', ptr) end
    return nil
end

function Entity:mark_ecs_suspend_readonly_state_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_suspend_readonly_state_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_table_cache_hdr_t|nil
function Entity:get_ecs_table_cache_hdr_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_table_cache_hdr_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_table_cache_hdr_t*', ptr) end
    return nil
end

function Entity:mark_ecs_table_cache_hdr_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_table_cache_hdr_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_table_diff_t|nil
function Entity:get_ecs_table_diff_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_table_diff_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_table_diff_t*', ptr) end
    return nil
end

function Entity:mark_ecs_table_diff_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_table_diff_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_table_records_t|nil
function Entity:get_ecs_table_records_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_table_records_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_table_records_t*', ptr) end
    return nil
end

function Entity:mark_ecs_table_records_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_table_records_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_value_t|nil
function Entity:get_ecs_value_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_value_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_value_t*', ptr) end
    return nil
end

function Entity:mark_ecs_value_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_value_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_entity_desc_t|nil
function Entity:get_ecs_entity_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_entity_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_entity_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_entity_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_entity_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_bulk_desc_t|nil
function Entity:get_ecs_bulk_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_bulk_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_bulk_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_bulk_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_bulk_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_component_desc_t|nil
function Entity:get_ecs_component_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_component_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_component_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_component_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_component_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_query_desc_t|nil
function Entity:get_ecs_query_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_query_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_query_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_observer_desc_t|nil
function Entity:get_ecs_observer_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_observer_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_observer_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_observer_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_observer_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_event_desc_t|nil
function Entity:get_ecs_event_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_event_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_event_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_event_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_event_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_build_info_t|nil
function Entity:get_ecs_build_info_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_build_info_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_build_info_t*', ptr) end
    return nil
end

function Entity:mark_ecs_build_info_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_build_info_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_query_group_info_t|nil
function Entity:get_ecs_query_group_info_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_group_info_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_query_group_info_t*', ptr) end
    return nil
end

function Entity:mark_ecs_query_group_info_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_group_info_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsIdentifier|nil
function Entity:get_EcsIdentifier()
    local comp_id = spectre.get_component_id(self.world, 'EcsIdentifier')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsIdentifier*', ptr) end
    return nil
end

function Entity:mark_EcsIdentifier_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsIdentifier')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsComponent|nil
function Entity:get_EcsComponent()
    local comp_id = spectre.get_component_id(self.world, 'EcsComponent')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsComponent*', ptr) end
    return nil
end

function Entity:mark_EcsComponent_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsComponent')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsPoly|nil
function Entity:get_EcsPoly()
    local comp_id = spectre.get_component_id(self.world, 'EcsPoly')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsPoly*', ptr) end
    return nil
end

function Entity:mark_EcsPoly_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsPoly')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsDefaultChildComponent|nil
function Entity:get_EcsDefaultChildComponent()
    local comp_id = spectre.get_component_id(self.world, 'EcsDefaultChildComponent')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsDefaultChildComponent*', ptr) end
    return nil
end

function Entity:mark_EcsDefaultChildComponent_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsDefaultChildComponent')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_entities_t|nil
function Entity:get_ecs_entities_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_entities_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_entities_t*', ptr) end
    return nil
end

function Entity:mark_ecs_entities_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_entities_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_delete_empty_tables_desc_t|nil
function Entity:get_ecs_delete_empty_tables_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_delete_empty_tables_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_delete_empty_tables_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_delete_empty_tables_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_delete_empty_tables_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_query_count_t|nil
function Entity:get_ecs_query_count_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_count_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_query_count_t*', ptr) end
    return nil
end

function Entity:mark_ecs_query_count_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_count_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_app_desc_t|nil
function Entity:get_ecs_app_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_app_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_app_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_app_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_app_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsTimer|nil
function Entity:get_EcsTimer()
    local comp_id = spectre.get_component_id(self.world, 'EcsTimer')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsTimer*', ptr) end
    return nil
end

function Entity:mark_EcsTimer_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsTimer')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsRateFilter|nil
function Entity:get_EcsRateFilter()
    local comp_id = spectre.get_component_id(self.world, 'EcsRateFilter')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsRateFilter*', ptr) end
    return nil
end

function Entity:mark_EcsRateFilter_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsRateFilter')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_pipeline_desc_t|nil
function Entity:get_ecs_pipeline_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_pipeline_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_pipeline_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_pipeline_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_pipeline_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsTickSource|nil
function Entity:get_EcsTickSource()
    local comp_id = spectre.get_component_id(self.world, 'EcsTickSource')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsTickSource*', ptr) end
    return nil
end

function Entity:mark_EcsTickSource_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsTickSource')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_system_desc_t|nil
function Entity:get_ecs_system_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_system_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_system_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_system_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_system_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_system_t|nil
function Entity:get_ecs_system_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_system_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_system_t*', ptr) end
    return nil
end

function Entity:mark_ecs_system_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_system_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_gauge_t|nil
function Entity:get_ecs_gauge_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_gauge_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_gauge_t*', ptr) end
    return nil
end

function Entity:mark_ecs_gauge_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_gauge_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_counter_t|nil
function Entity:get_ecs_counter_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_counter_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_counter_t*', ptr) end
    return nil
end

function Entity:mark_ecs_counter_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_counter_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_query_stats_t|nil
function Entity:get_ecs_query_stats_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_stats_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_query_stats_t*', ptr) end
    return nil
end

function Entity:mark_ecs_query_stats_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_query_stats_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_system_stats_t|nil
function Entity:get_ecs_system_stats_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_system_stats_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_system_stats_t*', ptr) end
    return nil
end

function Entity:mark_ecs_system_stats_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_system_stats_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_sync_stats_t|nil
function Entity:get_ecs_sync_stats_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_sync_stats_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_sync_stats_t*', ptr) end
    return nil
end

function Entity:mark_ecs_sync_stats_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_sync_stats_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_pipeline_stats_t|nil
function Entity:get_ecs_pipeline_stats_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_pipeline_stats_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_pipeline_stats_t*', ptr) end
    return nil
end

function Entity:mark_ecs_pipeline_stats_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_pipeline_stats_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsMetricValue|nil
function Entity:get_EcsMetricValue()
    local comp_id = spectre.get_component_id(self.world, 'EcsMetricValue')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsMetricValue*', ptr) end
    return nil
end

function Entity:mark_EcsMetricValue_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsMetricValue')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsMetricSource|nil
function Entity:get_EcsMetricSource()
    local comp_id = spectre.get_component_id(self.world, 'EcsMetricSource')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsMetricSource*', ptr) end
    return nil
end

function Entity:mark_EcsMetricSource_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsMetricSource')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_metric_desc_t|nil
function Entity:get_ecs_metric_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_metric_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_metric_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_metric_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_metric_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsAlertInstance|nil
function Entity:get_EcsAlertInstance()
    local comp_id = spectre.get_component_id(self.world, 'EcsAlertInstance')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsAlertInstance*', ptr) end
    return nil
end

function Entity:mark_EcsAlertInstance_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsAlertInstance')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsAlertsActive|nil
function Entity:get_EcsAlertsActive()
    local comp_id = spectre.get_component_id(self.world, 'EcsAlertsActive')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsAlertsActive*', ptr) end
    return nil
end

function Entity:mark_EcsAlertsActive_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsAlertsActive')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_alert_severity_filter_t|nil
function Entity:get_ecs_alert_severity_filter_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_alert_severity_filter_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_alert_severity_filter_t*', ptr) end
    return nil
end

function Entity:mark_ecs_alert_severity_filter_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_alert_severity_filter_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_alert_desc_t|nil
function Entity:get_ecs_alert_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_alert_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_alert_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_alert_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_alert_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_from_json_desc_t|nil
function Entity:get_ecs_from_json_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_from_json_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_from_json_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_from_json_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_from_json_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_entity_to_json_desc_t|nil
function Entity:get_ecs_entity_to_json_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_entity_to_json_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_entity_to_json_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_entity_to_json_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_entity_to_json_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_iter_to_json_desc_t|nil
function Entity:get_ecs_iter_to_json_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_iter_to_json_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_iter_to_json_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_iter_to_json_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_iter_to_json_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_world_to_json_desc_t|nil
function Entity:get_ecs_world_to_json_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_world_to_json_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_world_to_json_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_world_to_json_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_world_to_json_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_script_var_t|nil
function Entity:get_ecs_script_var_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_var_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_script_var_t*', ptr) end
    return nil
end

function Entity:mark_ecs_script_var_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_var_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_script_vars_t|nil
function Entity:get_ecs_script_vars_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_vars_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_script_vars_t*', ptr) end
    return nil
end

function Entity:mark_ecs_script_vars_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_vars_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_script_t|nil
function Entity:get_ecs_script_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_script_t*', ptr) end
    return nil
end

function Entity:mark_ecs_script_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsScript|nil
function Entity:get_EcsScript()
    local comp_id = spectre.get_component_id(self.world, 'EcsScript')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsScript*', ptr) end
    return nil
end

function Entity:mark_EcsScript_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsScript')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_function_ctx_t|nil
function Entity:get_ecs_function_ctx_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_function_ctx_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_function_ctx_t*', ptr) end
    return nil
end

function Entity:mark_ecs_function_ctx_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_function_ctx_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_script_parameter_t|nil
function Entity:get_ecs_script_parameter_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_parameter_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_script_parameter_t*', ptr) end
    return nil
end

function Entity:mark_ecs_script_parameter_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_parameter_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsScriptConstVar|nil
function Entity:get_EcsScriptConstVar()
    local comp_id = spectre.get_component_id(self.world, 'EcsScriptConstVar')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsScriptConstVar*', ptr) end
    return nil
end

function Entity:mark_EcsScriptConstVar_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsScriptConstVar')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsScriptFunction|nil
function Entity:get_EcsScriptFunction()
    local comp_id = spectre.get_component_id(self.world, 'EcsScriptFunction')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsScriptFunction*', ptr) end
    return nil
end

function Entity:mark_EcsScriptFunction_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsScriptFunction')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsScriptMethod|nil
function Entity:get_EcsScriptMethod()
    local comp_id = spectre.get_component_id(self.world, 'EcsScriptMethod')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsScriptMethod*', ptr) end
    return nil
end

function Entity:mark_EcsScriptMethod_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsScriptMethod')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_script_eval_desc_t|nil
function Entity:get_ecs_script_eval_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_eval_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_script_eval_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_script_eval_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_eval_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_script_eval_result_t|nil
function Entity:get_ecs_script_eval_result_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_eval_result_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_script_eval_result_t*', ptr) end
    return nil
end

function Entity:mark_ecs_script_eval_result_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_eval_result_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_script_desc_t|nil
function Entity:get_ecs_script_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_script_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_script_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_script_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_expr_eval_desc_t|nil
function Entity:get_ecs_expr_eval_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_expr_eval_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_expr_eval_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_expr_eval_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_expr_eval_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_const_var_desc_t|nil
function Entity:get_ecs_const_var_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_const_var_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_const_var_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_const_var_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_const_var_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_function_desc_t|nil
function Entity:get_ecs_function_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_function_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_function_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_function_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_function_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsDocDescription|nil
function Entity:get_EcsDocDescription()
    local comp_id = spectre.get_component_id(self.world, 'EcsDocDescription')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsDocDescription*', ptr) end
    return nil
end

function Entity:mark_EcsDocDescription_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsDocDescription')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsType|nil
function Entity:get_EcsType()
    local comp_id = spectre.get_component_id(self.world, 'EcsType')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsType*', ptr) end
    return nil
end

function Entity:mark_EcsType_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsType')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsPrimitive|nil
function Entity:get_EcsPrimitive()
    local comp_id = spectre.get_component_id(self.world, 'EcsPrimitive')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsPrimitive*', ptr) end
    return nil
end

function Entity:mark_EcsPrimitive_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsPrimitive')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsMember|nil
function Entity:get_EcsMember()
    local comp_id = spectre.get_component_id(self.world, 'EcsMember')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsMember*', ptr) end
    return nil
end

function Entity:mark_EcsMember_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsMember')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_member_value_range_t|nil
function Entity:get_ecs_member_value_range_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_member_value_range_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_member_value_range_t*', ptr) end
    return nil
end

function Entity:mark_ecs_member_value_range_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_member_value_range_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsMemberRanges|nil
function Entity:get_EcsMemberRanges()
    local comp_id = spectre.get_component_id(self.world, 'EcsMemberRanges')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsMemberRanges*', ptr) end
    return nil
end

function Entity:mark_EcsMemberRanges_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsMemberRanges')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_member_t|nil
function Entity:get_ecs_member_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_member_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_member_t*', ptr) end
    return nil
end

function Entity:mark_ecs_member_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_member_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsStruct|nil
function Entity:get_EcsStruct()
    local comp_id = spectre.get_component_id(self.world, 'EcsStruct')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsStruct*', ptr) end
    return nil
end

function Entity:mark_EcsStruct_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsStruct')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_enum_constant_t|nil
function Entity:get_ecs_enum_constant_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_enum_constant_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_enum_constant_t*', ptr) end
    return nil
end

function Entity:mark_ecs_enum_constant_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_enum_constant_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsEnum|nil
function Entity:get_EcsEnum()
    local comp_id = spectre.get_component_id(self.world, 'EcsEnum')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsEnum*', ptr) end
    return nil
end

function Entity:mark_EcsEnum_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsEnum')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_bitmask_constant_t|nil
function Entity:get_ecs_bitmask_constant_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_bitmask_constant_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_bitmask_constant_t*', ptr) end
    return nil
end

function Entity:mark_ecs_bitmask_constant_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_bitmask_constant_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsBitmask|nil
function Entity:get_EcsBitmask()
    local comp_id = spectre.get_component_id(self.world, 'EcsBitmask')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsBitmask*', ptr) end
    return nil
end

function Entity:mark_EcsBitmask_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsBitmask')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsConstants|nil
function Entity:get_EcsConstants()
    local comp_id = spectre.get_component_id(self.world, 'EcsConstants')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsConstants*', ptr) end
    return nil
end

function Entity:mark_EcsConstants_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsConstants')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsArray|nil
function Entity:get_EcsArray()
    local comp_id = spectre.get_component_id(self.world, 'EcsArray')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsArray*', ptr) end
    return nil
end

function Entity:mark_EcsArray_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsArray')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsVector|nil
function Entity:get_EcsVector()
    local comp_id = spectre.get_component_id(self.world, 'EcsVector')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsVector*', ptr) end
    return nil
end

function Entity:mark_EcsVector_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsVector')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_serializer_t|nil
function Entity:get_ecs_serializer_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_serializer_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_serializer_t*', ptr) end
    return nil
end

function Entity:mark_ecs_serializer_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_serializer_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsOpaque|nil
function Entity:get_EcsOpaque()
    local comp_id = spectre.get_component_id(self.world, 'EcsOpaque')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsOpaque*', ptr) end
    return nil
end

function Entity:mark_EcsOpaque_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsOpaque')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_unit_translation_t|nil
function Entity:get_ecs_unit_translation_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_unit_translation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_unit_translation_t*', ptr) end
    return nil
end

function Entity:mark_ecs_unit_translation_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_unit_translation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsUnit|nil
function Entity:get_EcsUnit()
    local comp_id = spectre.get_component_id(self.world, 'EcsUnit')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsUnit*', ptr) end
    return nil
end

function Entity:mark_EcsUnit_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsUnit')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsUnitPrefix|nil
function Entity:get_EcsUnitPrefix()
    local comp_id = spectre.get_component_id(self.world, 'EcsUnitPrefix')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsUnitPrefix*', ptr) end
    return nil
end

function Entity:mark_EcsUnitPrefix_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsUnitPrefix')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return EcsTypeSerializer|nil
function Entity:get_EcsTypeSerializer()
    local comp_id = spectre.get_component_id(self.world, 'EcsTypeSerializer')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('EcsTypeSerializer*', ptr) end
    return nil
end

function Entity:mark_EcsTypeSerializer_modified()
    local comp_id = spectre.get_component_id(self.world, 'EcsTypeSerializer')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_meta_scope_t|nil
function Entity:get_ecs_meta_scope_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_meta_scope_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_meta_scope_t*', ptr) end
    return nil
end

function Entity:mark_ecs_meta_scope_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_meta_scope_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_meta_cursor_t|nil
function Entity:get_ecs_meta_cursor_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_meta_cursor_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_meta_cursor_t*', ptr) end
    return nil
end

function Entity:mark_ecs_meta_cursor_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_meta_cursor_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_primitive_desc_t|nil
function Entity:get_ecs_primitive_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_primitive_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_primitive_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_primitive_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_primitive_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_enum_desc_t|nil
function Entity:get_ecs_enum_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_enum_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_enum_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_enum_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_enum_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_bitmask_desc_t|nil
function Entity:get_ecs_bitmask_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_bitmask_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_bitmask_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_bitmask_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_bitmask_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_array_desc_t|nil
function Entity:get_ecs_array_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_array_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_array_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_array_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_array_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_vector_desc_t|nil
function Entity:get_ecs_vector_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_vector_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_vector_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_vector_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_vector_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_struct_desc_t|nil
function Entity:get_ecs_struct_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_struct_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_struct_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_struct_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_struct_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_opaque_desc_t|nil
function Entity:get_ecs_opaque_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_opaque_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_opaque_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_opaque_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_opaque_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_unit_desc_t|nil
function Entity:get_ecs_unit_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_unit_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_unit_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_unit_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_unit_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_unit_prefix_desc_t|nil
function Entity:get_ecs_unit_prefix_desc_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_unit_prefix_desc_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_unit_prefix_desc_t*', ptr) end
    return nil
end

function Entity:mark_ecs_unit_prefix_desc_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_unit_prefix_desc_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return ecs_cpp_get_mut_t|nil
function Entity:get_ecs_cpp_get_mut_t()
    local comp_id = spectre.get_component_id(self.world, 'ecs_cpp_get_mut_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('ecs_cpp_get_mut_t*', ptr) end
    return nil
end

function Entity:mark_ecs_cpp_get_mut_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'ecs_cpp_get_mut_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return sandbox_application_api_t|nil
function Entity:get_sandbox_application_api_t()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_application_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_application_api_t*', ptr) end
    return nil
end

function Entity:mark_sandbox_application_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_application_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return sandbox_application_service_t|nil
function Entity:get_sandbox_application_service_t()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_application_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_application_service_t*', ptr) end
    return nil
end

function Entity:mark_sandbox_application_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_application_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return sandbox_configuration_service_t|nil
function Entity:get_sandbox_configuration_service_t()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_configuration_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_configuration_service_t*', ptr) end
    return nil
end

function Entity:mark_sandbox_configuration_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_configuration_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return sandbox_filesystem_api_t|nil
function Entity:get_sandbox_filesystem_api_t()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_filesystem_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_filesystem_api_t*', ptr) end
    return nil
end

function Entity:mark_sandbox_filesystem_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_filesystem_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return sandbox_filesystem_service_t|nil
function Entity:get_sandbox_filesystem_service_t()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_filesystem_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_filesystem_service_t*', ptr) end
    return nil
end

function Entity:mark_sandbox_filesystem_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_filesystem_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return sandbox_logs_service_t|nil
function Entity:get_sandbox_logs_service_t()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_logs_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_logs_service_t*', ptr) end
    return nil
end

function Entity:mark_sandbox_logs_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_logs_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return sandbox_runtime_api_t|nil
function Entity:get_sandbox_runtime_api_t()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_runtime_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_runtime_api_t*', ptr) end
    return nil
end

function Entity:mark_sandbox_runtime_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_runtime_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return sandbox_runtime_service_t|nil
function Entity:get_sandbox_runtime_service_t()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_runtime_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_runtime_service_t*', ptr) end
    return nil
end

function Entity:mark_sandbox_runtime_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'sandbox_runtime_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_serializer_component|nil
function Entity:get_spectre_serializer_component()
    local comp_id = spectre.get_component_id(self.world, 'spectre_serializer_component')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_serializer_component*', ptr) end
    return nil
end

function Entity:mark_spectre_serializer_component_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_serializer_component')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_serializer_relation_t|nil
function Entity:get_spectre_serializer_relation_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_serializer_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_serializer_relation_t*', ptr) end
    return nil
end

function Entity:mark_spectre_serializer_relation_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_serializer_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_resource_component_t|nil
function Entity:get_spectre_resource_component_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_resource_component_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_resource_component_t*', ptr) end
    return nil
end

function Entity:mark_spectre_resource_component_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_resource_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_resource_loader_component_t|nil
function Entity:get_spectre_resource_loader_component_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_resource_loader_component_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_resource_loader_component_t*', ptr) end
    return nil
end

function Entity:mark_spectre_resource_loader_component_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_resource_loader_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_disable_rendering_t|nil
function Entity:get_spectre_disable_rendering_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_disable_rendering_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_disable_rendering_t*', ptr) end
    return nil
end

function Entity:mark_spectre_disable_rendering_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_disable_rendering_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_state_t|nil
function Entity:get_spectre_state_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_state_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_state_t*', ptr) end
    return nil
end

function Entity:mark_spectre_state_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_state_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_scene_t|nil
function Entity:get_spectre_scene_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_scene_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_scene_t*', ptr) end
    return nil
end

function Entity:mark_spectre_scene_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_scene_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_state_use_scene_relation_t|nil
function Entity:get_spectre_state_use_scene_relation_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_state_use_scene_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_state_use_scene_relation_t*', ptr) end
    return nil
end

function Entity:mark_spectre_state_use_scene_relation_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_state_use_scene_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_color_t|nil
function Entity:get_spectre_color_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_color_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_color_t*', ptr) end
    return nil
end

function Entity:mark_spectre_color_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_color_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_renderable_t|nil
function Entity:get_spectre_renderable_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_renderable_t*', ptr) end
    return nil
end

function Entity:mark_spectre_renderable_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_2D_transform_component_t|nil
function Entity:get_spectre_2D_transform_component_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_2D_transform_component_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_2D_transform_component_t*', ptr) end
    return nil
end

function Entity:mark_spectre_2D_transform_component_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_2D_transform_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_rectange_renderable_t|nil
function Entity:get_spectre_rectange_renderable_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_rectange_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_rectange_renderable_t*', ptr) end
    return nil
end

function Entity:mark_spectre_rectange_renderable_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_rectange_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_circle_renderable_t|nil
function Entity:get_spectre_circle_renderable_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_circle_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_circle_renderable_t*', ptr) end
    return nil
end

function Entity:mark_spectre_circle_renderable_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_circle_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_polygone_renderable_t|nil
function Entity:get_spectre_polygone_renderable_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_polygone_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_polygone_renderable_t*', ptr) end
    return nil
end

function Entity:mark_spectre_polygone_renderable_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_polygone_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_custom_polygone_renderable_t|nil
function Entity:get_spectre_custom_polygone_renderable_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_custom_polygone_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_custom_polygone_renderable_t*', ptr) end
    return nil
end

function Entity:mark_spectre_custom_polygone_renderable_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_custom_polygone_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_ligne_renderable_t|nil
function Entity:get_spectre_ligne_renderable_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_ligne_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_ligne_renderable_t*', ptr) end
    return nil
end

function Entity:mark_spectre_ligne_renderable_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_ligne_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_script_t|nil
function Entity:get_spectre_script_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_script_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_script_t*', ptr) end
    return nil
end

function Entity:mark_spectre_script_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_script_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_use_script_on_enter_relation_t|nil
function Entity:get_spectre_use_script_on_enter_relation_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_enter_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_enter_relation_t*', ptr) end
    return nil
end

function Entity:mark_spectre_use_script_on_enter_relation_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_enter_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_use_script_on_exit_relation_t|nil
function Entity:get_spectre_use_script_on_exit_relation_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_exit_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_exit_relation_t*', ptr) end
    return nil
end

function Entity:mark_spectre_use_script_on_exit_relation_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_exit_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_use_script_on_create_relation_t|nil
function Entity:get_spectre_use_script_on_create_relation_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_create_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_create_relation_t*', ptr) end
    return nil
end

function Entity:mark_spectre_use_script_on_create_relation_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_create_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_use_script_on_destroy_relation_t|nil
function Entity:get_spectre_use_script_on_destroy_relation_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_destroy_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_destroy_relation_t*', ptr) end
    return nil
end

function Entity:mark_spectre_use_script_on_destroy_relation_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_destroy_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_use_script_on_update_relation_t|nil
function Entity:get_spectre_use_script_on_update_relation_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_update_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_update_relation_t*', ptr) end
    return nil
end

function Entity:mark_spectre_use_script_on_update_relation_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_update_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_use_script_on_render_relation_t|nil
function Entity:get_spectre_use_script_on_render_relation_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_render_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_render_relation_t*', ptr) end
    return nil
end

function Entity:mark_spectre_use_script_on_render_relation_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_use_script_on_render_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_input_state_t|nil
function Entity:get_spectre_input_state_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_input_state_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_input_state_t*', ptr) end
    return nil
end

function Entity:mark_spectre_input_state_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_input_state_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_window_component_t|nil
function Entity:get_spectre_window_component_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_window_component_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_window_component_t*', ptr) end
    return nil
end

function Entity:mark_spectre_window_component_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_window_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_prefab_flag_t|nil
function Entity:get_spectre_prefab_flag_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_prefab_flag_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_prefab_flag_t*', ptr) end
    return nil
end

function Entity:mark_spectre_prefab_flag_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_prefab_flag_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_prefabs_api_t|nil
function Entity:get_spectre_prefabs_api_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_prefabs_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_prefabs_api_t*', ptr) end
    return nil
end

function Entity:mark_spectre_prefabs_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_prefabs_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_prefabs_service_t|nil
function Entity:get_spectre_prefabs_service_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_prefabs_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_prefabs_service_t*', ptr) end
    return nil
end

function Entity:mark_spectre_prefabs_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_prefabs_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_renderer_api_t|nil
function Entity:get_spectre_renderer_api_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_renderer_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_renderer_api_t*', ptr) end
    return nil
end

function Entity:mark_spectre_renderer_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_renderer_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_renderer_service_t|nil
function Entity:get_spectre_renderer_service_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_renderer_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_renderer_service_t*', ptr) end
    return nil
end

function Entity:mark_spectre_renderer_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_renderer_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_resources_api_t|nil
function Entity:get_spectre_resources_api_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_resources_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_resources_api_t*', ptr) end
    return nil
end

function Entity:mark_spectre_resources_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_resources_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_resources_service_t|nil
function Entity:get_spectre_resources_service_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_resources_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_resources_service_t*', ptr) end
    return nil
end

function Entity:mark_spectre_resources_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_resources_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_scenes_api_t|nil
function Entity:get_spectre_scenes_api_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_scenes_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_scenes_api_t*', ptr) end
    return nil
end

function Entity:mark_spectre_scenes_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_scenes_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_scenes_service_t|nil
function Entity:get_spectre_scenes_service_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_scenes_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_scenes_service_t*', ptr) end
    return nil
end

function Entity:mark_spectre_scenes_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_scenes_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_scripts_api_t|nil
function Entity:get_spectre_scripts_api_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_scripts_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_scripts_api_t*', ptr) end
    return nil
end

function Entity:mark_spectre_scripts_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_scripts_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_scripts_service_t|nil
function Entity:get_spectre_scripts_service_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_scripts_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_scripts_service_t*', ptr) end
    return nil
end

function Entity:mark_spectre_scripts_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_scripts_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_serializer_api_t|nil
function Entity:get_spectre_serializer_api_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_serializer_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_serializer_api_t*', ptr) end
    return nil
end

function Entity:mark_spectre_serializer_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_serializer_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_serializer_service_t|nil
function Entity:get_spectre_serializer_service_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_serializer_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_serializer_service_t*', ptr) end
    return nil
end

function Entity:mark_spectre_serializer_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_serializer_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_window_api_t|nil
function Entity:get_spectre_window_api_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_window_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_window_api_t*', ptr) end
    return nil
end

function Entity:mark_spectre_window_api_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_window_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_window_service_t|nil
function Entity:get_spectre_window_service_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_window_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_window_service_t*', ptr) end
    return nil
end

function Entity:mark_spectre_window_service_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_window_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

return spectre