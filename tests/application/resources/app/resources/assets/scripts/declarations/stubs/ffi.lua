---@meta

local ffi = {}

---@class ffi.C
ffi.C = {}

function ffi.cdef(def) end
function ffi.new(ct, ...) end
function ffi.typeof(ct) end
function ffi.cast(ct, init) end
function ffi.metatype(ct, metatable) end
function ffi.gc(cdata, finalizer) end
function ffi.sizeof(ct, ...) end
function ffi.alignof(ct) end
function ffi.offsetof(ct, field) end
function ffi.istype(ct, obj) end
function ffi.errno(newerr) end
function ffi.string(ptr, len) end
function ffi.copy(dst, src, len) end
function ffi.fill(dst, len, c) end
function ffi.abi(param) end
function ffi.os() end
function ffi.arch() end

---@class ecs_header_t
---@field type number
---@field refcount number
---@field mixins any
---@class ecs_vec_t
---@field array any
---@field count number
---@field size number
---@class ecs_sparse_page_t
---@field sparse any
---@field data any
---@class ecs_sparse_t
---@field dense any
---@field pages any
---@field size number
---@field count number
---@field max_id number
---@field allocator any
---@field page_allocator any
---@class ecs_block_allocator_block_t
---@field memory any
---@field next any
---@class ecs_block_allocator_chunk_header_t
---@field next any
---@class ecs_block_allocator_t
---@field data_size number
---@field chunk_size number
---@field chunks_per_block number
---@field block_size number
---@field head any
---@field block_head any
---@class ecs_stack_page_t
---@field data any
---@field next any
---@field sp number
---@field id number
---@class ecs_stack_cursor_t
---@field prev any
---@field page any
---@field sp number
---@field is_free boolean
---@field owner any
---@class ecs_stack_t
---@field first any
---@field tail_page any
---@field tail_cursor any
---@field cursor_count number
---@class ecs_bucket_entry_t
---@field key any
---@field value any
---@field next any
---@class ecs_bucket_t
---@field first any
---@class ecs_map_iter_t
---@field map any
---@field bucket any
---@field entry any
---@field res any
---@field change_count number
---@class ecs_strbuf_list_elem
---@field count number
---@field separator string
---@class ecs_strbuf_t
---@field content string
---@field length number
---@field size number
---@field list_stack any
---@field list_sp number
---@field small_string number
---@class ecs_time_t
---@field sec number
---@field nanosec number
---@class ecs_os_api_t
---@field init_ any
---@field fini_ any
---@field malloc_ any
---@field realloc_ any
---@field calloc_ any
---@field free_ any
---@field strdup_ any
---@field thread_new_ any
---@field thread_join_ any
---@field thread_self_ any
---@field task_new_ any
---@field task_join_ any
---@field ainc_ any
---@field adec_ any
---@field lainc_ any
---@field ladec_ any
---@field mutex_new_ any
---@field mutex_free_ any
---@field mutex_lock_ any
---@field mutex_unlock_ any
---@field cond_new_ any
---@field cond_free_ any
---@field cond_signal_ any
---@field cond_broadcast_ any
---@field cond_wait_ any
---@field sleep_ any
---@field now_ any
---@field get_time_ any
---@field log_ any
---@field abort_ any
---@field dlopen_ any
---@field dlproc_ any
---@field dlclose_ any
---@field module_to_dl_ any
---@field module_to_etc_ any
---@field perf_trace_push_ any
---@field perf_trace_pop_ any
---@field log_level_ number
---@field log_indent_ number
---@field log_last_error_ number
---@field log_last_timestamp_ number
---@field flags_ any
---@field log_out_ any
---@class ecs_term_ref_t
---@field id number
---@field name string
---@class ecs_event_record_t
---@field any any
---@field wildcard any
---@field wildcard_pair any
---@field event_ids any
---@field event number
---@class ecs_table_range_t
---@field table any
---@field offset number
---@field count number
---@class ecs_var_t
---@field range any
---@field entity number
---@class ecs_page_iter_t
---@field offset number
---@field limit number
---@field remaining number
---@class ecs_worker_iter_t
---@field index number
---@field count number
---@class ecs_table_cache_iter_t
---@field cur any
---@field next any
---@field iter_fill boolean
---@field iter_empty boolean
---@class ecs_each_iter_t
---@field it any
---@field ids number
---@field sources number
---@field sizes number
---@field columns number
---@field trs any
---@class ecs_query_op_profile_t
---@field count number
---@class ecs_query_iter_t
---@field vars any
---@field query_vars any
---@field ops any
---@field op_ctx any
---@field written any
---@field group any
---@field tables any
---@field all_tables any
---@field elem any
---@field cur number
---@field all_cur number
---@field profile any
---@field op number
---@field iter_single_group boolean
---@class ecs_commands_t
---@field queue any
---@field stack any
---@field entries any
---@class ecs_suspend_readonly_state_t
---@field is_readonly boolean
---@field is_deferred boolean
---@field cmd_flushing boolean
---@field defer_count number
---@field scope number
---@field with number
---@field cmd_stack any
---@field cmd any
---@field stage any
---@class ecs_table_cache_hdr_t
---@field cr any
---@field table any
---@field prev any
---@field next any
---@class ecs_table_diff_t
---@field added any
---@field removed any
---@field added_flags any
---@field removed_flags any
---@class ecs_table_records_t
---@field array any
---@field count number
---@class ecs_value_t
---@field type number
---@field ptr any
---@class ecs_entity_desc_t
---@field _canary number
---@field id number
---@field parent number
---@field name string
---@field sep string
---@field root_sep string
---@field symbol string
---@field use_low_id boolean
---@field add any
---@field set any
---@field add_expr string
---@class ecs_bulk_desc_t
---@field _canary number
---@field entities any
---@field count number
---@field ids number
---@field data any
---@field table any
---@class ecs_component_desc_t
---@field _canary number
---@field entity number
---@field type any
---@class ecs_query_desc_t
---@field _canary number
---@field terms any
---@field expr string
---@field cache_kind any
---@field flags any
---@field order_by_callback any
---@field order_by_table_callback any
---@field order_by number
---@field group_by number
---@field group_by_callback any
---@field on_group_create any
---@field on_group_delete any
---@field group_by_ctx any
---@field group_by_ctx_free any
---@field ctx any
---@field binding_ctx any
---@field ctx_free any
---@field binding_ctx_free any
---@field entity number
---@class ecs_observer_desc_t
---@field _canary number
---@field entity number
---@field query any
---@field events number
---@field yield_existing boolean
---@field callback any
---@field run any
---@field ctx any
---@field ctx_free any
---@field callback_ctx any
---@field callback_ctx_free any
---@field run_ctx any
---@field run_ctx_free any
---@field last_event_id any
---@field term_index_ number
---@field flags_ any
---@class ecs_event_desc_t
---@field event number
---@field ids any
---@field table any
---@field other_table any
---@field offset number
---@field count number
---@field entity number
---@field param any
---@field const_param any
---@field observable any
---@field flags any
---@class ecs_build_info_t
---@field compiler string
---@field addons string
---@field flags string
---@field version string
---@field version_major number
---@field version_minor number
---@field version_patch number
---@field debug boolean
---@field sanitize boolean
---@field perf_trace boolean
---@class ecs_query_group_info_t
---@field id number
---@field match_count number
---@field table_count number
---@field ctx any
---@class EcsIdentifier
---@field value string
---@field length number
---@field hash number
---@field index_hash number
---@field index any
---@class EcsComponent
---@field size number
---@field alignment number
---@class EcsPoly
---@field poly any
---@class EcsDefaultChildComponent
---@field component number
---@class ecs_entities_t
---@field ids any
---@field count number
---@field alive_count number
---@class ecs_delete_empty_tables_desc_t
---@field clear_generation number
---@field delete_generation number
---@field time_budget_seconds number
---@class ecs_query_count_t
---@field results number
---@field entities number
---@field tables number
---@class ecs_app_desc_t
---@field target_fps number
---@field delta_time number
---@field threads number
---@field frames number
---@field enable_rest boolean
---@field enable_stats boolean
---@field port number
---@field init any
---@field ctx any
---@class EcsTimer
---@field timeout number
---@field time number
---@field overshoot number
---@field fired_count number
---@field active boolean
---@field single_shot boolean
---@class EcsRateFilter
---@field src number
---@field rate number
---@field tick_count number
---@field time_elapsed number
---@class ecs_pipeline_desc_t
---@field entity number
---@field query any
---@class EcsTickSource
---@field tick boolean
---@field time_elapsed number
---@class ecs_system_desc_t
---@field _canary number
---@field entity number
---@field query any
---@field callback any
---@field run any
---@field ctx any
---@field ctx_free any
---@field callback_ctx any
---@field callback_ctx_free any
---@field run_ctx any
---@field run_ctx_free any
---@field interval number
---@field rate number
---@field tick_source number
---@field multi_threaded boolean
---@field immediate boolean
---@class ecs_system_t
---@field hdr any
---@field run any
---@field action any
---@field query any
---@field tick_source number
---@field multi_threaded boolean
---@field immediate boolean
---@field name string
---@field ctx any
---@field callback_ctx any
---@field run_ctx any
---@field ctx_free any
---@field callback_ctx_free any
---@field run_ctx_free any
---@field time_spent number
---@field time_passed number
---@field last_frame number
---@field dtor any
---@class ecs_gauge_t
---@field avg number
---@field min number
---@field max number
---@class ecs_counter_t
---@field rate any
---@field value number
---@class ecs_query_stats_t
---@field first_ number
---@field result_count any
---@field matched_table_count any
---@field matched_entity_count any
---@field last_ number
---@field t number
---@class ecs_system_stats_t
---@field first_ number
---@field time_spent any
---@field last_ number
---@field task boolean
---@field query any
---@class ecs_sync_stats_t
---@field first_ number
---@field time_spent any
---@field commands_enqueued any
---@field last_ number
---@field system_count number
---@field multi_threaded boolean
---@field immediate boolean
---@class ecs_pipeline_stats_t
---@field canary_ number
---@field systems any
---@field sync_points any
---@field t number
---@field system_count number
---@field active_system_count number
---@field rebuild_count number
---@class EcsMetricValue
---@field value number
---@class EcsMetricSource
---@field entity number
---@class ecs_metric_desc_t
---@field _canary number
---@field entity number
---@field member number
---@field dotmember string
---@field id number
---@field targets boolean
---@field kind number
---@field brief string
---@class EcsAlertInstance
---@field message string
---@class EcsAlertsActive
---@field info_count number
---@field warning_count number
---@field error_count number
---@field alerts any
---@class ecs_alert_severity_filter_t
---@field severity number
---@field with number
---@field var string
---@field _var_index number
---@class ecs_alert_desc_t
---@field _canary number
---@field entity number
---@field query any
---@field message string
---@field doc_name string
---@field brief string
---@field severity number
---@field severity_filters any
---@field retain_period number
---@field member number
---@field id number
---@field var string
---@class ecs_from_json_desc_t
---@field name string
---@field expr string
---@field lookup_ctx any
---@field strict boolean
---@class ecs_entity_to_json_desc_t
---@field serialize_entity_id boolean
---@field serialize_doc boolean
---@field serialize_full_paths boolean
---@field serialize_inherited boolean
---@field serialize_values boolean
---@field serialize_builtin boolean
---@field serialize_type_info boolean
---@field serialize_alerts boolean
---@field serialize_refs number
---@field serialize_matches boolean
---@class ecs_iter_to_json_desc_t
---@field serialize_entity_ids boolean
---@field serialize_values boolean
---@field serialize_builtin boolean
---@field serialize_doc boolean
---@field serialize_full_paths boolean
---@field serialize_fields boolean
---@field serialize_inherited boolean
---@field serialize_table boolean
---@field serialize_type_info boolean
---@field serialize_field_info boolean
---@field serialize_query_info boolean
---@field serialize_query_plan boolean
---@field serialize_query_profile boolean
---@field dont_serialize_results boolean
---@field serialize_alerts boolean
---@field serialize_refs number
---@field serialize_matches boolean
---@field query any
---@class ecs_world_to_json_desc_t
---@field serialize_builtin boolean
---@field serialize_modules boolean
---@class ecs_script_var_t
---@field name string
---@field value any
---@field type_info any
---@field sp number
---@field is_const boolean
---@class ecs_script_vars_t
---@field parent any
---@field sp number
---@field var_index any
---@field vars any
---@field world any
---@field stack any
---@field cursor any
---@field allocator any
---@class ecs_script_t
---@field world any
---@field name string
---@field code string
---@class EcsScript
---@field filename string
---@field code string
---@field error string
---@field script any
---@field template_ any
---@class ecs_function_ctx_t
---@field world any
---@field function number
---@field ctx any
---@class ecs_script_parameter_t
---@field name string
---@field type number
---@class EcsScriptConstVar
---@field value any
---@field type_info any
---@class EcsScriptFunction
---@field return_type number
---@field params any
---@field callback any
---@field ctx any
---@class EcsScriptMethod
---@field return_type number
---@field params any
---@field callback any
---@field ctx any
---@class ecs_script_eval_desc_t
---@field vars any
---@field runtime any
---@class ecs_script_eval_result_t
---@field error string
---@class ecs_script_desc_t
---@field entity number
---@field filename string
---@field code string
---@class ecs_expr_eval_desc_t
---@field name string
---@field expr string
---@field vars any
---@field type number
---@field lookup_ctx any
---@field disable_folding boolean
---@field disable_dynamic_variable_binding boolean
---@field allow_unresolved_identifiers boolean
---@field runtime any
---@field script_visitor any
---@class ecs_const_var_desc_t
---@field name string
---@field parent number
---@field type number
---@field value any
---@class ecs_function_desc_t
---@field name string
---@field parent number
---@field params any
---@field return_type number
---@field callback any
---@field ctx any
---@class EcsDocDescription
---@field value string
---@class EcsType
---@field kind any
---@field existing boolean
---@field partial boolean
---@class EcsPrimitive
---@field kind any
---@class EcsMember
---@field type number
---@field count number
---@field unit number
---@field offset number
---@field use_offset boolean
---@class ecs_member_value_range_t
---@field min number
---@field max number
---@class EcsMemberRanges
---@field value any
---@field warning any
---@field error any
---@class ecs_member_t
---@field name string
---@field type number
---@field count number
---@field offset number
---@field unit number
---@field use_offset boolean
---@field range any
---@field error_range any
---@field warning_range any
---@field size number
---@field member number
---@class EcsStruct
---@field members any
---@class ecs_enum_constant_t
---@field name string
---@field value number
---@field value_unsigned number
---@field constant number
---@class EcsEnum
---@field underlying_type number
---@class ecs_bitmask_constant_t
---@field name string
---@field value any
---@field _unused number
---@field constant number
---@class EcsBitmask
---@field dummy_ number
---@class EcsConstants
---@field constants any
---@field ordered_constants any
---@class EcsArray
---@field type number
---@field count number
---@class EcsVector
---@field type number
---@class ecs_serializer_t
---@field ser any
---@field ser any
---@field world any
---@field ctx any
---@class EcsOpaque
---@field as_type number
---@field serialize any
---@field serialize_member any
---@field serialize_element any
---@field dst any
---@field dst string
---@field dst any
---@field dst any
---@field dst any
---@field dst any
---@field dst any
---@field dst any
---@field dst any
---@field dst any
---@field dst any
---@class ecs_unit_translation_t
---@field factor number
---@field power number
---@class EcsUnit
---@field symbol string
---@field prefix number
---@field base number
---@field over number
---@field translation any
---@class EcsUnitPrefix
---@field symbol string
---@field translation any
---@class EcsTypeSerializer
---@field kind any
---@field ops any
---@class ecs_meta_scope_t
---@field type number
---@field ops any
---@field ops_count number
---@field ops_cur number
---@field prev_depth number
---@field ptr any
---@field opaque any
---@field members any
---@field is_collection boolean
---@field is_empty_scope boolean
---@field is_moved_scope boolean
---@field elem number
---@field elem_count number
---@class ecs_meta_cursor_t
---@field world any
---@field scope any
---@field depth number
---@field valid boolean
---@field is_primitive_scope boolean
---@field lookup_ctx any
---@class ecs_primitive_desc_t
---@field entity number
---@field kind any
---@class ecs_enum_desc_t
---@field entity number
---@field constants any
---@field underlying_type number
---@class ecs_bitmask_desc_t
---@field entity number
---@field constants any
---@class ecs_array_desc_t
---@field entity number
---@field type number
---@field count number
---@class ecs_vector_desc_t
---@field entity number
---@field type number
---@class ecs_struct_desc_t
---@field entity number
---@field members any
---@class ecs_opaque_desc_t
---@field entity number
---@field type any
---@class ecs_unit_desc_t
---@field entity number
---@field symbol string
---@field quantity number
---@field base number
---@field over number
---@field translation any
---@field prefix number
---@class ecs_unit_prefix_desc_t
---@field entity number
---@field symbol string
---@field translation any
---@class ecs_cpp_get_mut_t
---@field ptr any
---@field call_modified boolean
---@class sandbox_application_api_t
---@class sandbox_application_service_t
---@field api any
---@field info any
---@class sandbox_configuration_service_t
---@field api any
---@field info any
---@class sandbox_filesystem_api_t
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@field ecs any
---@class sandbox_filesystem_service_t
---@field api any
---@field info any
---@class sandbox_logs_service_t
---@field api any
---@field info any
---@class sandbox_runtime_api_t
---@class sandbox_runtime_service_t
---@field api any
---@field info any
---@class spectre_serializer_component
---@field world any
---@field world any
---@class spectre_serializer_relation_t
---@field dummy number
---@class spectre_resource_component_t
---@field path string
---@field instance any
---@class spectre_resource_loader_component_t
---@field world any
---@field world any
---@class spectre_disable_rendering_t
---@field dummy number
---@class spectre_state_t
---@field dummy number
---@class spectre_scene_t
---@field dummy number
---@class spectre_state_use_scene_relation_t
---@field layer_index number
---@class spectre_color_t
---@field r number
---@field g number
---@field b number
---@field a number
---@class spectre_renderable_t
---@field dummy number
---@class spectre_2D_transform_component_t
---@field position_x number
---@field position_y number
---@field position_z number
---@field scale_x number
---@field scale_y number
---@field origin_x number
---@field origin_y number
---@field rotation number
---@class spectre_rectange_renderable_t
---@field width number
---@field height number
---@field fill_color any
---@field outline_color any
---@field outline_thickness number
---@class spectre_circle_renderable_t
---@field radius number
---@field fill_color any
---@field outline_color any
---@field outline_thickness number
---@class spectre_polygone_renderable_t
---@field radius number
---@field point_count number
---@field fill_color any
---@field outline_color any
---@field outline_thickness number
---@class spectre_custom_polygone_renderable_t
---@field vertices any
---@field vertex_count number
---@field fill_color any
---@field outline_color any
---@field outline_thickness number
---@class spectre_ligne_renderable_t
---@field position_x1 number
---@field position_y1 number
---@field position_x2 number
---@field position_y2 number
---@field color any
---@field thickness number
---@class spectre_script_t
---@field function_name string
---@field arguments_name string
---@field argument_types any
---@field argument_count number
---@class spectre_use_script_on_enter_relation_t
---@field arguments any
---@field argument_count number
---@class spectre_use_script_on_exit_relation_t
---@field arguments any
---@field argument_count number
---@class spectre_use_script_on_create_relation_t
---@field arguments any
---@field argument_count number
---@class spectre_use_script_on_destroy_relation_t
---@field arguments any
---@field argument_count number
---@class spectre_use_script_on_update_relation_t
---@field arguments any
---@field argument_count number
---@class spectre_use_script_on_render_relation_t
---@field arguments any
---@field argument_count number
---@class spectre_input_state_t
---@field keys_down number
---@field keys_pressed number
---@field keys_released number
---@field mouse_position number
---@field mouse_delta number
---@class spectre_window_component_t
---@field width number
---@field height number
---@field position_x number
---@field position_y number
---@field min_width number
---@field min_height number
---@field max_width number
---@field max_height number
---@field title string
---@field vsync boolean
---@field fullscreen boolean
---@field borderless boolean
---@field resizable boolean
---@field always_on_top boolean
---@field minimized boolean
---@field maximized boolean
---@field visible boolean
---@field focused boolean
---@field hovered boolean
---@field cursor_visible boolean
---@field cursor_locked boolean
---@field native_handle any
---@class spectre_prefab_flag_t
---@field dummy number
---@class spectre_prefabs_api_t
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@class spectre_prefabs_service_t
---@field api any
---@field info any
---@class spectre_renderer_api_t
---@field world any
---@field world any
---@field world any
---@class spectre_renderer_service_t
---@field api any
---@field info any
---@class spectre_resources_api_t
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@class spectre_resources_service_t
---@field api any
---@field info any
---@class spectre_scenes_api_t
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@class spectre_scenes_service_t
---@field api any
---@field info any
---@class spectre_scripts_api_t
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@class spectre_scripts_service_t
---@field api any
---@field info any
---@class spectre_serializer_api_t
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@class spectre_serializer_service_t
---@field api any
---@field info any
---@class spectre_window_api_t
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@field world any
---@class spectre_window_service_t
---@field api any
---@field info any
---@enum ecs_inout_kind_t
ecs_inout_kind_t = {
    EcsInOutDefault = 0,
    EcsInOutNone = 0,
    EcsInOutFilter = 0,
    EcsInOut = 0,
    EcsIn = 0,
    EcsOut = 0,
}

---@enum ecs_oper_kind_t
ecs_oper_kind_t = {
    EcsAnd = 0,
    EcsOr = 0,
    EcsNot = 0,
    EcsOptional = 0,
    EcsAndFrom = 0,
    EcsOrFrom = 0,
    EcsNotFrom = 0,
}

---@enum ecs_query_cache_kind_t
ecs_query_cache_kind_t = {
    EcsQueryCacheDefault = 0,
    EcsQueryCacheAuto = 0,
    EcsQueryCacheAll = 0,
    EcsQueryCacheNone = 0,
}

---@enum ecs_type_kind_t
ecs_type_kind_t = {
    EcsPrimitiveType = 0,
    EcsBitmaskType = 0,
    EcsEnumType = 0,
    EcsStructType = 0,
    EcsArrayType = 0,
    EcsVectorType = 0,
    EcsOpaqueType = 0,
    EcsTypeKindLast = 0,
}

---@enum ecs_primitive_kind_t
ecs_primitive_kind_t = {
    EcsBool = 0,
    EcsChar = 0,
    EcsByte = 0,
    EcsU8 = 0,
    EcsU16 = 0,
    EcsU32 = 0,
    EcsU64 = 0,
    EcsI8 = 0,
    EcsI16 = 0,
    EcsI32 = 0,
    EcsI64 = 0,
    EcsF32 = 0,
    EcsF64 = 0,
    EcsUPtr = 0,
    EcsIPtr = 0,
    EcsString = 0,
    EcsEntity = 0,
    EcsId = 0,
    EcsPrimitiveKindLast = 0,
}

---@enum ecs_meta_op_kind_t
ecs_meta_op_kind_t = {
    EcsOpPushStruct = 0,
    EcsOpPushArray = 0,
    EcsOpPushVector = 0,
    EcsOpPop = 0,
    EcsOpOpaqueStruct = 0,
    EcsOpOpaqueArray = 0,
    EcsOpOpaqueVector = 0,
    EcsOpForward = 0,
    EcsOpScope = 0,
    EcsOpOpaqueValue = 0,
    EcsOpEnum = 0,
    EcsOpBitmask = 0,
    EcsOpPrimitive = 0,
    EcsOpBool = 0,
    EcsOpChar = 0,
    EcsOpByte = 0,
    EcsOpU8 = 0,
    EcsOpU16 = 0,
    EcsOpU32 = 0,
    EcsOpU64 = 0,
    EcsOpI8 = 0,
    EcsOpI16 = 0,
    EcsOpI32 = 0,
    EcsOpI64 = 0,
    EcsOpF32 = 0,
    EcsOpF64 = 0,
    EcsOpUPtr = 0,
    EcsOpIPtr = 0,
    EcsOpString = 0,
    EcsOpEntity = 0,
    EcsOpId = 0,
    EcsMetaTypeOpKindLast = 0,
}

---@enum spectre_script_argument_type_t
spectre_script_argument_type_t = {
    SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL = 0,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN = 0,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER = 0,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER = 0,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING = 0,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE = 0,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA = 0,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_ENTITY = 0,
}

---@param result any
---@param allocator any
---@param page_allocator any
---@param size number
---@return nil
function ffi.C.flecs_sparse_init(result, allocator, page_allocator, size) end

---@param sparse any
---@return nil
function ffi.C.flecs_sparse_fini(sparse) end

---@param sparse any
---@return nil
function ffi.C.flecs_sparse_clear(sparse) end

---@param sparse any
---@param elem_size number
---@return any
function ffi.C.flecs_sparse_add(sparse, elem_size) end

---@param sparse any
---@return number
function ffi.C.flecs_sparse_last_id(sparse) end

---@param sparse any
---@return number
function ffi.C.flecs_sparse_new_id(sparse) end

---@param sparse any
---@param size number
---@param id number
---@return boolean
function ffi.C.flecs_sparse_remove(sparse, size, id) end

---@param sparse any
---@param size number
---@param id number
---@return boolean
function ffi.C.flecs_sparse_remove_w_gen(sparse, size, id) end

---@param sparse any
---@param id number
---@return boolean
function ffi.C.flecs_sparse_is_alive(sparse, id) end

---@param sparse any
---@param elem_size number
---@param index number
---@return any
function ffi.C.flecs_sparse_get_dense(sparse, elem_size, index) end

---@param sparse any
---@return number
function ffi.C.flecs_sparse_count(sparse) end

---@param sparse any
---@param id number
---@return boolean
function ffi.C.flecs_sparse_has(sparse, id) end

---@param sparse any
---@param elem_size number
---@param id number
---@return any
function ffi.C.flecs_sparse_get(sparse, elem_size, id) end

---@param sparse any
---@param elem_size number
---@param id number
---@return any
function ffi.C.flecs_sparse_insert(sparse, elem_size, id) end

---@param sparse any
---@param elem_size number
---@param id number
---@param is_new any
---@return any
function ffi.C.flecs_sparse_ensure(sparse, elem_size, id, is_new) end

---@param sparse any
---@param elem_size number
---@param id number
---@return any
function ffi.C.flecs_sparse_ensure_fast(sparse, elem_size, id) end

---@param sparse any
---@return any
function ffi.C.flecs_sparse_ids(sparse) end

---@param sparse any
---@return nil
function ffi.C.flecs_sparse_shrink(sparse) end

---@param ba any
---@param size number
---@return nil
function ffi.C.flecs_ballocator_init(ba, size) end

---@param size number
---@return any
function ffi.C.flecs_ballocator_new(size) end

---@param ba any
---@return nil
function ffi.C.flecs_ballocator_fini(ba) end

---@param ba any
---@return nil
function ffi.C.flecs_ballocator_free(ba) end

---@param allocator any
---@return any
function ffi.C.flecs_balloc(allocator) end

---@param allocator any
---@param type_name string
---@return any
function ffi.C.flecs_balloc_w_dbg_info(allocator, type_name) end

---@param allocator any
---@return any
function ffi.C.flecs_bcalloc(allocator) end

---@param allocator any
---@param type_name string
---@return any
function ffi.C.flecs_bcalloc_w_dbg_info(allocator, type_name) end

---@param allocator any
---@param memory any
---@return nil
function ffi.C.flecs_bfree(allocator, memory) end

---@param allocator any
---@param memory any
---@param type_name string
---@return nil
function ffi.C.flecs_bfree_w_dbg_info(allocator, memory, type_name) end

---@param dst any
---@param src any
---@param memory any
---@return any
function ffi.C.flecs_brealloc(dst, src, memory) end

---@param dst any
---@param src any
---@param memory any
---@param type_name string
---@return any
function ffi.C.flecs_brealloc_w_dbg_info(dst, src, memory, type_name) end

---@param ba any
---@param memory any
---@return any
function ffi.C.flecs_bdup(ba, memory) end

---@param stack any
---@return nil
function ffi.C.flecs_stack_init(stack) end

---@param stack any
---@return nil
function ffi.C.flecs_stack_fini(stack) end

---@param stack any
---@param size number
---@param align number
---@return any
function ffi.C.flecs_stack_alloc(stack, size, align) end

---@param stack any
---@param size number
---@param align number
---@return any
function ffi.C.flecs_stack_calloc(stack, size, align) end

---@param ptr any
---@param size number
---@return nil
function ffi.C.flecs_stack_free(ptr, size) end

---@param stack any
---@return nil
function ffi.C.flecs_stack_reset(stack) end

---@param stack any
---@return any
function ffi.C.flecs_stack_get_cursor(stack) end

---@param stack any
---@param cursor any
---@return nil
function ffi.C.flecs_stack_restore_cursor(stack, cursor) end

---@param a any
---@return nil
function ffi.C.flecs_allocator_init(a) end

---@param a any
---@return nil
function ffi.C.flecs_allocator_fini(a) end

---@param a any
---@param size number
---@return any
function ffi.C.flecs_allocator_get(a, size) end

---@param a any
---@param str string
---@return string
function ffi.C.flecs_strdup(a, str) end

---@param a any
---@param str string
---@return nil
function ffi.C.flecs_strfree(a, str) end

---@param a any
---@param size number
---@param src any
---@return any
function ffi.C.flecs_dup(a, size, src) end

---@param c_name string
---@return string
function ffi.C.flecs_module_path_from_c(c_name) end

---@param ptr any
---@param count number
---@param type_info any
---@return nil
function ffi.C.flecs_default_ctor(ptr, count, type_info) end

---@param fmt string
---@param args any
---@return string
function ffi.C.flecs_vasprintf(fmt, args) end

---@param fmt string
---@return string
function ffi.C.flecs_asprintf(fmt, ...) end

---@param out string
---@param in_ number
---@param delimiter number
---@return string
function ffi.C.flecs_chresc(out, in_, delimiter) end

---@param in_ string
---@param out string
---@return string
function ffi.C.flecs_chrparse(in_, out) end

---@param out string
---@param size number
---@param delimiter number
---@param in_ string
---@return number
function ffi.C.flecs_stresc(out, size, delimiter, in_) end

---@param delimiter number
---@param in_ string
---@return string
function ffi.C.flecs_astresc(delimiter, in_) end

---@param ptr string
---@return string
function ffi.C.flecs_parse_ws_eol(ptr) end

---@param ptr string
---@param token string
---@return string
function ffi.C.flecs_parse_digit(ptr, token) end

---@param str string
---@return string
function ffi.C.flecs_to_snake_case(str) end

---@param world any
---@param state any
---@return any
function ffi.C.flecs_suspend_readonly(world, state) end

---@param world any
---@param state any
---@return nil
function ffi.C.flecs_resume_readonly(world, state) end

---@param table any
---@return number
function ffi.C.flecs_table_observed_count(table) end

---@param stream any
---@return nil
function ffi.C.flecs_dump_backtrace(stream) end

---@param poly any
---@return number
function ffi.C.flecs_poly_claim_(poly) end

---@param poly any
---@return number
function ffi.C.flecs_poly_release_(poly) end

---@param poly any
---@return number
function ffi.C.flecs_poly_refcount(poly) end

---@return number
function ffi.C.flecs_component_ids_index_get() end

---@param world any
---@param index number
---@return number
function ffi.C.flecs_component_ids_get(world, index) end

---@param world any
---@param index number
---@return number
function ffi.C.flecs_component_ids_get_alive(world, index) end

---@param world any
---@param index number
---@param id number
---@return nil
function ffi.C.flecs_component_ids_set(world, index, id) end

---@param it any
---@return boolean
function ffi.C.flecs_query_trivial_cached_next(it) end

---@param world any
---@return nil
function ffi.C.flecs_check_exclusive_world_access_write(world) end

---@param world any
---@return nil
function ffi.C.flecs_check_exclusive_world_access_read(world) end

---@param hm any
---@param key_size number
---@param value_size number
---@param hash any
---@param compare any
---@param allocator any
---@return nil
function ffi.C.flecs_hashmap_init_(hm, key_size, value_size, hash, compare, allocator) end

---@param map any
---@return nil
function ffi.C.flecs_hashmap_fini(map) end

---@param map any
---@param key_size number
---@param key any
---@param value_size number
---@return any
function ffi.C.flecs_hashmap_get_(map, key_size, key, value_size) end

---@param map any
---@param key_size number
---@param key any
---@param value_size number
---@return any
function ffi.C.flecs_hashmap_ensure_(map, key_size, key, value_size) end

---@param map any
---@param key_size number
---@param key any
---@param value_size number
---@param value any
---@return nil
function ffi.C.flecs_hashmap_set_(map, key_size, key, value_size, value) end

---@param map any
---@param key_size number
---@param key any
---@param value_size number
---@return nil
function ffi.C.flecs_hashmap_remove_(map, key_size, key, value_size) end

---@param map any
---@param key_size number
---@param key any
---@param value_size number
---@param hash number
---@return nil
function ffi.C.flecs_hashmap_remove_w_hash_(map, key_size, key, value_size, hash) end

---@param map any
---@param hash number
---@return any
function ffi.C.flecs_hashmap_get_bucket(map, hash) end

---@param map any
---@param bucket any
---@param hash number
---@param index number
---@return nil
function ffi.C.flecs_hm_bucket_remove(map, bucket, hash, index) end

---@param dst any
---@param src any
---@return nil
function ffi.C.flecs_hashmap_copy(dst, src) end

---@param map any
---@return any
function ffi.C.flecs_hashmap_iter(map) end

---@param it any
---@param key_size number
---@param key_out any
---@param value_size number
---@return any
function ffi.C.flecs_hashmap_next_(it, key_size, key_out, value_size) end

---@param world any
---@param id number
---@return any
function ffi.C.flecs_components_get(world, id) end

---@param cr any
---@return number
function ffi.C.flecs_component_get_id(cr) end

---@param world any
---@param id number
---@return any
function ffi.C.flecs_component_get_flags(world, id) end

---@param cr any
---@param table any
---@return any
function ffi.C.flecs_component_get_table(cr, table) end

---@param cr any
---@param iter_out any
---@return boolean
function ffi.C.flecs_component_iter(cr, iter_out) end

---@param iter any
---@return any
function ffi.C.flecs_component_next(iter) end

---@param table any
---@return any
function ffi.C.flecs_table_records(table) end

---@param tr any
---@return any
function ffi.C.flecs_table_record_get_component(tr) end

---@param table any
---@return number
function ffi.C.flecs_table_id(table) end

---@param object any
---@param type_ number
---@return boolean
function ffi.C.flecs_poly_is_(object, type_) end

---@param world any
---@param entity number
---@return nil
function ffi.C.ecs_delete(world, entity) end

---@param world any
---@param entity number
---@param component number
---@return nil
function ffi.C.ecs_add_id(world, entity, component) end

---@param world any
---@param entity number
---@param component number
---@return nil
function ffi.C.ecs_remove_id(world, entity, component) end

---@param world any
---@param entity number
---@return nil
function ffi.C.ecs_clear(world, entity) end

---@param world any
---@param entity number
---@param enabled boolean
---@return nil
function ffi.C.ecs_enable(world, entity, enabled) end

---@param world any
---@param entity number
---@param component number
---@param enable boolean
---@return nil
function ffi.C.ecs_enable_id(world, entity, component, enable) end

---@param world any
---@param entity number
---@param component number
---@return any
function ffi.C.ecs_get_id(world, entity, component) end

---@param world any
---@param entity number
---@param component number
---@return any
function ffi.C.ecs_get_mut_id(world, entity, component) end

---@param world any
---@param entity number
---@param component number
---@return nil
function ffi.C.ecs_modified_id(world, entity, component) end

---@param world any
---@param entity number
---@param component number
---@param size number
---@param ptr any
---@return nil
function ffi.C.ecs_set_id(world, entity, component, size, ptr) end

---@param world any
---@param e number
---@return boolean
function ffi.C.ecs_is_valid(world, e) end

---@param world any
---@param e number
---@return boolean
function ffi.C.ecs_is_alive(world, e) end

---@param world any
---@param entity number
---@param component number
---@return boolean
function ffi.C.ecs_has_id(world, entity, component) end

---@param world any
---@param symbol string
---@param lookup_as_path boolean
---@param recursive boolean
---@return number
function ffi.C.ecs_lookup_symbol(world, symbol, lookup_as_path, recursive) end

---@param world any
---@return nil
function ffi.C.FlecsRestImport(world) end

---@param world any
---@return nil
function ffi.C.FlecsTimerImport(world) end

---@param world any
---@return nil
function ffi.C.FlecsPipelineImport(world) end

---@param world any
---@return nil
function ffi.C.FlecsSystemImport(world) end

---@param world any
---@return nil
function ffi.C.FlecsStatsImport(world) end

---@param world any
---@return nil
function ffi.C.FlecsMetricsImport(world) end

---@param world any
---@return nil
function ffi.C.FlecsAlertsImport(world) end

---@param world any
---@return nil
function ffi.C.FlecsUnitsImport(world) end

---@param world any
---@return nil
function ffi.C.FlecsScriptImport(world) end

---@param world any
---@return nil
function ffi.C.FlecsDocImport(world) end

---@param world any
---@return nil
function ffi.C.FlecsMetaImport(world) end

---@param info any
---@return boolean
function ffi.C.sandbox_stage_service(info) end

---@param info any
---@return boolean
function ffi.C.sandbox_stage_module(info) end

---@param ecs any
---@param library_path string
---@return nil
function ffi.C.sandbox_index_library(ecs, library_path) end

---@param ecs any
---@return any
function ffi.C.sandbox_get_bootstrapper(ecs) end

---@param bootstrapper any
---@param ecs any
---@param architecture string
---@param name string
---@param version_major number
---@param version_minor number
---@param version_patch number
---@return boolean
function ffi.C.sandbox_bootstrapper_activate(bootstrapper, ecs, architecture, name, version_major, version_minor, version_patch) end

---@param bootstrapper any
---@param ecs any
---@param module_str string
---@return boolean
function ffi.C.sandbox_bootstrapper_activate_string(bootstrapper, ecs, module_str) end

---@param bootstrapper any
---@param ecs any
---@return boolean
function ffi.C.sandbox_bootstrapper_boot(bootstrapper, ecs) end

---@param ecs any
---@return boolean
function ffi.C.sandbox_application_is_running(ecs) end

---@return any
function ffi.C.sandbox_properties_create() end

---@param props any
---@return nil
function ffi.C.sandbox_properties_destroy(props) end

---@param props any
---@param data string
---@param data_length number
---@param format any
---@return boolean
function ffi.C.sandbox_properties_load(props, data, data_length, format) end

---@param props any
---@param format any
---@return string
function ffi.C.sandbox_properties_dump(props, format) end

---@param str string
---@return nil
function ffi.C.sandbox_properties_free_string(str) end

---@param props any
---@param path_str string
---@return nil
function ffi.C.sandbox_properties_clear(props, path_str) end

---@param props any
---@param path_str string
---@return boolean
function ffi.C.sandbox_properties_has(props, path_str) end

---@param props any
---@param path_str string
---@param other any
---@return nil
function ffi.C.sandbox_properties_merge(props, path_str, other) end

---@param props any
---@param path_str string
---@return any
function ffi.C.sandbox_properties_sub(props, path_str) end

---@param props any
---@param path_str string
---@param out_val any
---@return boolean
function ffi.C.sandbox_properties_get_int64(props, path_str, out_val) end

---@param props any
---@param path_str string
---@param out_val any
---@return boolean
function ffi.C.sandbox_properties_get_uint64(props, path_str, out_val) end

---@param props any
---@param path_str string
---@param out_val any
---@return boolean
function ffi.C.sandbox_properties_get_double(props, path_str, out_val) end

---@param props any
---@param path_str string
---@param out_val any
---@return boolean
function ffi.C.sandbox_properties_get_bool(props, path_str, out_val) end

---@param props any
---@param path_str string
---@param val number
---@return nil
function ffi.C.sandbox_properties_set_int64(props, path_str, val) end

---@param props any
---@param path_str string
---@param val number
---@return nil
function ffi.C.sandbox_properties_set_uint64(props, path_str, val) end

---@param props any
---@param path_str string
---@param val number
---@return nil
function ffi.C.sandbox_properties_set_double(props, path_str, val) end

---@param props any
---@param path_str string
---@param val boolean
---@return nil
function ffi.C.sandbox_properties_set_bool(props, path_str, val) end

---@param props any
---@param path_str string
---@param val string
---@return nil
function ffi.C.sandbox_properties_set_string(props, path_str, val) end

---@param props any
---@param path_str string
---@param values any
---@param count number
---@return nil
function ffi.C.sandbox_properties_set_int64_array(props, path_str, values, count) end

---@param props any
---@param path_str string
---@param values any
---@param count number
---@return nil
function ffi.C.sandbox_properties_set_uint64_array(props, path_str, values, count) end

---@param props any
---@param path_str string
---@param values any
---@param count number
---@return nil
function ffi.C.sandbox_properties_set_double_array(props, path_str, values, count) end

---@param props any
---@param path_str string
---@param values any
---@param count number
---@return nil
function ffi.C.sandbox_properties_set_bool_array(props, path_str, values, count) end

---@param props any
---@param path_str string
---@param values string
---@param count number
---@return nil
function ffi.C.sandbox_properties_set_string_array(props, path_str, values, count) end

---@param ecs any
---@return any
function ffi.C.sandbox_configuration_get_properties(ecs) end

---@param ecs any
---@param physical_path string
---@param virtual_mount_point string
---@param read_only boolean
---@return boolean
function ffi.C.sandbox_filesystem_mount(ecs, physical_path, virtual_mount_point, read_only) end

---@param ecs any
---@param mount_point string
---@return boolean
function ffi.C.sandbox_filesystem_unmount(ecs, mount_point) end

---@param ecs any
---@param virtual_path string
---@return any
function ffi.C.sandbox_filesystem_open_read(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@param append boolean
---@param force_path boolean
---@return any
function ffi.C.sandbox_filesystem_open_write(ecs, virtual_path, append, force_path) end

---@param ecs any
---@param handle any
---@param buffer any
---@param bytes_to_read number
---@return number
function ffi.C.sandbox_filesystem_read(ecs, handle, buffer, bytes_to_read) end

---@param ecs any
---@param handle any
---@param buffer any
---@param bytes_to_write number
---@return number
function ffi.C.sandbox_filesystem_write(ecs, handle, buffer, bytes_to_write) end

---@param ecs any
---@param handle any
---@return boolean
function ffi.C.sandbox_filesystem_eof(ecs, handle) end

---@param ecs any
---@param handle any
---@return number
function ffi.C.sandbox_filesystem_tell(ecs, handle) end

---@param ecs any
---@param handle any
---@param position number
---@return boolean
function ffi.C.sandbox_filesystem_seek(ecs, handle, position) end

---@param ecs any
---@param handle any
---@return number
function ffi.C.sandbox_filesystem_size(ecs, handle) end

---@param ecs any
---@param handle any
---@return nil
function ffi.C.sandbox_filesystem_close_handle(ecs, handle) end

---@param ecs any
---@param virtual_path string
---@param force_path boolean
---@return boolean
function ffi.C.sandbox_filesystem_create_file(ecs, virtual_path, force_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_remove_file(ecs, virtual_path) end

---@param ecs any
---@param source_virtual_path string
---@param dest_virtual_path string
---@param overwrite boolean
---@param force_path boolean
---@return boolean
function ffi.C.sandbox_filesystem_copy(ecs, source_virtual_path, dest_virtual_path, overwrite, force_path) end

---@param ecs any
---@param source_virtual_path string
---@param dest_virtual_path string
---@param overwrite boolean
---@param force_path boolean
---@return boolean
function ffi.C.sandbox_filesystem_move(ecs, source_virtual_path, dest_virtual_path, overwrite, force_path) end

---@param ecs any
---@param virtual_path string
---@param force_path boolean
---@return boolean
function ffi.C.sandbox_filesystem_create_directory(ecs, virtual_path, force_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_remove_directory(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_exists(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_is_file(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_is_directory(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_is_readonly(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return number
function ffi.C.sandbox_filesystem_file_size(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return number
function ffi.C.sandbox_filesystem_last_modified(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@param recursive boolean
---@param out_files string
---@param out_count any
---@return boolean
function ffi.C.sandbox_filesystem_list_files(ecs, virtual_path, recursive, out_files, out_count) end

---@param ecs any
---@param files string
---@param count number
---@return nil
function ffi.C.sandbox_filesystem_free_file_list(ecs, files, count) end

---@param ecs any
---@param virtual_path string
---@param out_data any
---@param out_size any
---@return boolean
function ffi.C.sandbox_filesystem_read_all_bytes(ecs, virtual_path, out_data, out_size) end

---@param ecs any
---@param data any
---@return nil
function ffi.C.sandbox_filesystem_free_bytes(ecs, data) end

---@param ecs any
---@param msg string
---@return nil
function ffi.C.sandbox_logs_trace(ecs, msg) end

---@param ecs any
---@param msg string
---@return nil
function ffi.C.sandbox_logs_debug(ecs, msg) end

---@param ecs any
---@param msg string
---@return nil
function ffi.C.sandbox_logs_info(ecs, msg) end

---@param ecs any
---@param msg string
---@return nil
function ffi.C.sandbox_logs_warn(ecs, msg) end

---@param ecs any
---@param msg string
---@return nil
function ffi.C.sandbox_logs_error(ecs, msg) end

---@param ecs any
---@return nil
function ffi.C.sandbox_runtime_run(ecs) end

---@param ecs any
---@return nil
function ffi.C.sandbox_runtime_start(ecs) end

---@param ecs any
---@return nil
function ffi.C.sandbox_runtime_stop(ecs) end

---@param ecs any
---@return nil
function ffi.C.sandbox_runtime_pause(ecs) end

---@param ecs any
---@return nil
function ffi.C.sandbox_runtime_resume(ecs) end

---@param world any
---@param entity number
---@return any
function ffi.C.spectre_prefabs_serialize_entity(world, entity) end

---@param world any
---@param target number
---@param props any
---@return nil
function ffi.C.spectre_prefabs_deserialize_entity(world, target, props) end

---@param world any
---@param name string
---@param props any
---@return nil
function ffi.C.spectre_prefabs_register_prefab(world, name, props) end

---@param world any
---@param name string
---@return boolean
function ffi.C.spectre_prefabs_has_prefab(world, name) end

---@param world any
---@param entity number
---@return boolean
function ffi.C.spectre_prefabs_is_prefab(world, entity) end

---@param world any
---@param name string
---@return number
function ffi.C.spectre_prefabs_find_prefab(world, name) end

---@param world any
---@param props any
---@return number
function ffi.C.spectre_prefabs_create_entity_from_props(world, props) end

---@param world any
---@param prefab number
---@return number
function ffi.C.spectre_prefabs_create_entity_from_prefab(world, prefab) end

---@param world any
---@param name string
---@return number
function ffi.C.spectre_prefabs_create_entity_from_name(world, name) end

---@param world any
---@param target number
---@param props any
---@return nil
function ffi.C.spectre_renderer_deserialize_renderer(world, target, props) end

---@param world any
---@param renderer number
---@return any
function ffi.C.spectre_renderer_serialize_renderer(world, renderer) end

---@param world any
---@param props any
---@return nil
function ffi.C.spectre_renderer_register_renderer(world, props) end

---@param world any
---@return boolean
function ffi.C.spectre_renderer_is_renderer(world) end

---@param world any
---@param target number
---@param props any
---@return nil
function ffi.C.spectre_resources_deserialize_resource(world, target, props) end

---@param world any
---@param resourceEntity number
---@return any
function ffi.C.spectre_resources_serialize_resource(world, resourceEntity) end

---@param world any
---@param type_ string
---@param loader any
---@return nil
function ffi.C.spectre_resources_register_resource_loader(world, type_, loader) end

---@param world any
---@param props any
---@return nil
function ffi.C.spectre_resources_register_resource(world, props) end

---@param world any
---@param type_ string
---@return boolean
function ffi.C.spectre_resources_has_resource_loader(world, type_) end

---@param world any
---@param name string
---@return boolean
function ffi.C.spectre_resources_has_resource(world, name) end

---@param world any
---@param entity number
---@return boolean
function ffi.C.spectre_resources_is_resource(world, entity) end

---@param world any
---@param type_ string
---@return number
function ffi.C.spectre_resources_find_resource_loader(world, type_) end

---@param world any
---@param name string
---@return number
function ffi.C.spectre_resources_find_resource(world, name) end

---@param world any
---@param resource number
---@return boolean
function ffi.C.spectre_resources_is_resource_loaded(world, resource) end

---@param world any
---@param resourceEntity number
---@return nil
function ffi.C.spectre_resources_load_resource(world, resourceEntity) end

---@param world any
---@param resourceEntity number
---@return nil
function ffi.C.spectre_resources_free_resource(world, resourceEntity) end

---@param world any
---@param resourceEntity number
---@return any
function ffi.C.spectre_resources_get_resource(world, resourceEntity) end

---@param world any
---@param state number
---@return any
function ffi.C.spectre_scenes_serialize_state(world, state) end

---@param world any
---@param target number
---@param props any
---@return nil
function ffi.C.spectre_scenes_deserialize_state(world, target, props) end

---@param world any
---@param scene number
---@return any
function ffi.C.spectre_scenes_serialize_scene(world, scene) end

---@param world any
---@param target number
---@param props any
---@return nil
function ffi.C.spectre_scenes_deserialize_scene(world, target, props) end

---@param world any
---@param props any
---@return nil
function ffi.C.spectre_scenes_register_state(world, props) end

---@param world any
---@param props any
---@return nil
function ffi.C.spectre_scenes_register_scene(world, props) end

---@param world any
---@param name string
---@return number
function ffi.C.spectre_scenes_find_state(world, name) end

---@param world any
---@param name string
---@return number
function ffi.C.spectre_scenes_find_scene(world, name) end

---@param world any
---@param name string
---@return boolean
function ffi.C.spectre_scenes_has_state(world, name) end

---@param world any
---@param name string
---@return boolean
function ffi.C.spectre_scenes_has_scene(world, name) end

---@param world any
---@param entity number
---@return boolean
function ffi.C.spectre_scenes_is_state(world, entity) end

---@param world any
---@param entity number
---@return boolean
function ffi.C.spectre_scenes_is_scene(world, entity) end

---@param world any
---@return number
function ffi.C.spectre_scenes_find_current_state(world) end

---@param world any
---@return any
function ffi.C.spectre_scenes_find_current_scenes(world) end

---@param world any
---@param state number
---@return nil
function ffi.C.spectre_scenes_push_state(world, state) end

---@param world any
---@return nil
function ffi.C.spectre_scenes_pop_state(world) end

---@param world any
---@param entity number
---@param callback any
---@param payload any
---@return nil
function ffi.C.spectre_scenes_execute_recursive(world, entity, callback, payload) end

---@param world any
---@param function_name string
---@param arg_types any
---@param arg_count number
---@return boolean
function ffi.C.spectre_scripts_has_script(world, function_name, arg_types, arg_count) end

---@param world any
---@param entity number
---@return boolean
function ffi.C.spectre_scripts_is_script(world, entity) end

---@param world any
---@param function_name string
---@return number
function ffi.C.spectre_scripts_find_script(world, function_name) end

---@param world any
---@param path string
---@return nil
function ffi.C.spectre_scripts_include_code(world, path) end

---@param world any
---@param function_name string
---@param args any
---@param arg_count number
---@return nil
function ffi.C.spectre_scripts_execute_script(world, function_name, args, arg_count) end

---@param world any
---@param entity number
---@return any
function ffi.C.spectre_scripts_serialize_scripts(world, entity) end

---@param world any
---@param target number
---@param props any
---@return nil
function ffi.C.spectre_scripts_deserialize_scripts(world, target, props) end

---@param world any
---@param entity number
---@return nil
function ffi.C.spectre_scripts_execute_on_create(world, entity) end

---@param world any
---@param entity number
---@return nil
function ffi.C.spectre_scripts_execute_on_destroy(world, entity) end

---@param world any
---@param entity number
---@return nil
function ffi.C.spectre_scripts_execute_on_update(world, entity) end

---@param world any
---@param entity number
---@return nil
function ffi.C.spectre_scripts_execute_on_enter(world, entity) end

---@param world any
---@param entity number
---@return nil
function ffi.C.spectre_scripts_execute_on_exit(world, entity) end

---@param world any
---@param type_ string
---@param serializer any
---@return nil
function ffi.C.spectre_serializer_register_serializer(world, type_, serializer) end

---@param world any
---@param type_ string
---@return boolean
function ffi.C.spectre_serializer_has_serializer(world, type_) end

---@param world any
---@param entity number
---@return boolean
function ffi.C.spectre_serializer_is_serializer(world, entity) end

---@param world any
---@param type_ string
---@return number
function ffi.C.spectre_serializer_find_serializer(world, type_) end

---@param world any
---@param serializer number
---@param entity number
---@return any
function ffi.C.spectre_serializer_serialize_entity(world, serializer, entity) end

---@param world any
---@param serializer number
---@param entity number
---@param props any
---@return nil
function ffi.C.spectre_serializer_deserialize_entity(world, serializer, entity, props) end

---@param world any
---@param target number
---@param props any
---@return nil
function ffi.C.spectre_window_deserialize_window(world, target, props) end

---@param world any
---@param window number
---@return any
function ffi.C.spectre_window_serialize_window(world, window) end

---@param world any
---@param props any
---@return nil
function ffi.C.spectre_window_register_window(world, props) end

---@param world any
---@return boolean
function ffi.C.spectre_window_should_close(world) end

---@param world any
---@param close boolean
---@return nil
function ffi.C.spectre_window_set_should_close(world, close) end

---@param world any
---@param width number
---@param height number
---@return nil
function ffi.C.spectre_window_set_size(world, width, height) end

---@param world any
---@param x number
---@param y number
---@return nil
function ffi.C.spectre_window_set_position(world, x, y) end

---@param world any
---@param min_width number
---@param min_height number
---@param max_width number
---@param max_height number
---@return nil
function ffi.C.spectre_window_set_size_limits(world, min_width, min_height, max_width, max_height) end

---@param world any
---@return number
function ffi.C.spectre_window_get_width(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_height(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_aspect_ratio(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_position_x(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_position_y(world) end

---@param world any
---@param title string
---@return nil
function ffi.C.spectre_window_set_title(world, title) end

---@param world any
---@param enabled boolean
---@return nil
function ffi.C.spectre_window_set_vsync(world, enabled) end

---@param world any
---@param fullscreen boolean
---@return nil
function ffi.C.spectre_window_set_fullscreen(world, fullscreen) end

---@param world any
---@param borderless boolean
---@return nil
function ffi.C.spectre_window_set_borderless(world, borderless) end

---@param world any
---@param resizable boolean
---@return nil
function ffi.C.spectre_window_set_resizable(world, resizable) end

---@param world any
---@param always_on_top boolean
---@return nil
function ffi.C.spectre_window_set_always_on_top(world, always_on_top) end

---@param world any
---@return string
function ffi.C.spectre_window_get_title(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_vsync(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_fullscreen(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_borderless(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_resizable(world) end

---@param world any
---@return nil
function ffi.C.spectre_window_minimize(world) end

---@param world any
---@return nil
function ffi.C.spectre_window_maximize(world) end

---@param world any
---@return nil
function ffi.C.spectre_window_restore(world) end

---@param world any
---@return nil
function ffi.C.spectre_window_show(world) end

---@param world any
---@return nil
function ffi.C.spectre_window_hide(world) end

---@param world any
---@return nil
function ffi.C.spectre_window_request_attention(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_minimized(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_maximized(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_visible(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_focused(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_hovered(world) end

---@param world any
---@param visible boolean
---@return nil
function ffi.C.spectre_window_set_cursor_visible(world, visible) end

---@param world any
---@param locked boolean
---@return nil
function ffi.C.spectre_window_set_cursor_locked(world, locked) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_cursor_visible(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_cursor_locked(world) end

---@param world any
---@return any
function ffi.C.spectre_window_get_native_handle(world) end

---@param world any
---@param keycode number
---@return boolean
function ffi.C.spectre_window_is_key_down(world, keycode) end

---@param world any
---@param keycode number
---@return boolean
function ffi.C.spectre_window_is_key_pressed(world, keycode) end

---@param world any
---@param keycode number
---@return boolean
function ffi.C.spectre_window_is_key_released(world, keycode) end

---@param world any
---@return number
function ffi.C.spectre_window_get_mouse_x(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_mouse_y(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_mouse_delta_x(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_mouse_delta_y(world) end


return ffi
