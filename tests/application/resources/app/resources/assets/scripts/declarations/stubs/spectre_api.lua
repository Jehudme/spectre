---@meta

ffi = ffi or {}
ffi.C = ffi.C or {}

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
---@field all_cur any
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
---@field ctx) string
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
---@field ecs_entity_t) any
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
---@field ecs_entity_t) any
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
---@field ctx) string
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
---@field value) any
---@field member) string
---@field world any
---@field ctx any
---@class EcsOpaque
---@field as_type number
---@field serialize any
---@field serialize_member any
---@field serialize_element any
---@field value) any
---@field value) string
---@field value) any
---@field value) any
---@field value) any
---@field value) string
---@field entity) any
---@field id) any
---@field dst) any
---@field dst) any
---@field elem) any
---@field member) string
---@field dst) any
---@field count) any
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
---@field elem_count any
---@class ecs_meta_cursor_t
---@field world any
---@field scope any
---@field depth number
---@field valid boolean
---@field is_primitive_scope boolean
---@field void*) string
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
---@field ecs) any
---@class sandbox_application_service_t
---@field api any
---@field info any
---@class sandbox_configuration_service_t
---@field api any
---@field info any
---@class sandbox_filesystem_api_t
---@field read_only) string
---@field mount_point) string
---@field virtual_path) string
---@field force_path) string
---@field bytes_to_read) any
---@field bytes_to_write) any
---@field handle) any
---@field handle) any
---@field position) any
---@field handle) any
---@field handle) any
---@field force_path) string
---@field virtual_path) string
---@field force_path) string
---@field force_path) string
---@field force_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field out_count) string
---@field count) string
---@field out_size) string
---@field data) any
---@class sandbox_filesystem_service_t
---@field api any
---@field info any
---@class sandbox_logs_service_t
---@field api any
---@field info any
---@class sandbox_runtime_api_t
---@field ecs) any
---@field ecs) any
---@field ecs) any
---@field ecs) any
---@field ecs) any
---@class sandbox_runtime_service_t
---@field api any
---@field info any
---@class spectre_serializer_component
---@field props) any
---@field entity) any
---@class spectre_serializer_relation_t
---@field dummy number
---@class spectre_resource_component_t
---@field path string
---@field instance any
---@class spectre_resource_loader_component_t
---@field resource) any
---@field resource) any
---@class spectre_disable_rendering_t
---@field dummy number
---@class spectre_state_t
---@field dummy number
---@class spectre_scene_t
---@field dummy number
---@class spectre_state_use_scene_relation_t
---@field layer_index number
---@class spectre_color_t
---@field a any
---@class spectre_renderable_t
---@field dummy number
---@class spectre_2D_transform_component_t
---@field position_z any
---@field scale_y any
---@field origin_y any
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
---@field position_y2 any
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
---@field entity) any
---@field props) any
---@field props) string
---@field name) string
---@field entity) any
---@field name) string
---@field props) any
---@field prefab) any
---@field name) string
---@class spectre_prefabs_service_t
---@field api any
---@field info any
---@class spectre_renderer_api_t
---@field props) any
---@field renderer) any
---@field props) any
---@field world) any
---@class spectre_renderer_service_t
---@field api any
---@field info any
---@class spectre_resources_api_t
---@field props) any
---@field resourceEntity) any
---@field loader) string
---@field props) any
---@field type) string
---@field name) string
---@field entity) any
---@field type) string
---@field name) string
---@field resource) any
---@field resourceEntity) any
---@field resourceEntity) any
---@field resourceEntity) any
---@class spectre_resources_service_t
---@field api any
---@field info any
---@class spectre_scenes_api_t
---@field state) any
---@field props) any
---@field scene) any
---@field props) any
---@field props) any
---@field props) any
---@field name) string
---@field name) string
---@field name) string
---@field name) string
---@field entity) any
---@field entity) any
---@field world) any
---@field world) any
---@field state) any
---@field world) any
---@field payload) any
---@class spectre_scenes_service_t
---@field api any
---@field info any
---@class spectre_scripts_api_t
---@field arg_count) string
---@field entity) any
---@field function_name) string
---@field path) string
---@field arg_count) string
---@field entity) any
---@field props) any
---@field entity) any
---@field entity) any
---@field entity) any
---@field entity) any
---@field entity) any
---@class spectre_scripts_service_t
---@field api any
---@field info any
---@class spectre_serializer_api_t
---@field serializer) string
---@field type) string
---@field entity) any
---@field type) string
---@field entity) any
---@field props) any
---@class spectre_serializer_service_t
---@field api any
---@field info any
---@class spectre_window_api_t
---@field props) any
---@field window) any
---@field props) any
---@field world) any
---@field close) any
---@field height) any
---@field y) any
---@field max_height) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field title) string
---@field enabled) any
---@field fullscreen) any
---@field borderless) any
---@field resizable) any
---@field always_on_top) any
---@field world) string
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field visible) any
---@field locked) any
---@field world) any
---@field world) any
---@field world) any
---@field keycode) any
---@field keycode) any
---@field keycode) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
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

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@return nil
function ffi.C.ecs_vec_init(allocator, vec, size, elem_count) end

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@param type_name string
---@return nil
function ffi.C.ecs_vec_init_w_dbg_info(allocator, vec, size, elem_count, type_name) end

---@param vec any
---@param size number
---@return nil
function ffi.C.ecs_vec_init_if(vec, size) end

---@param allocator any
---@param vec any
---@param size number
---@return nil
function ffi.C.ecs_vec_fini(allocator, vec, size) end

---@param allocator any
---@param vec any
---@param size number
---@return any
function ffi.C.ecs_vec_reset(allocator, vec, size) end

---@param vec any
---@return nil
function ffi.C.ecs_vec_clear(vec) end

---@param allocator any
---@param vec any
---@param size number
---@return any
function ffi.C.ecs_vec_append(allocator, vec, size) end

---@param vec any
---@param size number
---@param elem number
---@return nil
function ffi.C.ecs_vec_remove(vec, size, elem) end

---@param v any
---@param size number
---@param index number
---@return nil
function ffi.C.ecs_vec_remove_ordered(v, size, index) end

---@param vec any
---@return nil
function ffi.C.ecs_vec_remove_last(vec) end

---@param allocator any
---@param vec any
---@param size number
---@return any
function ffi.C.ecs_vec_copy(allocator, vec, size) end

---@param allocator any
---@param vec any
---@param size number
---@return any
function ffi.C.ecs_vec_copy_shrink(allocator, vec, size) end

---@param allocator any
---@param vec any
---@param size number
---@return nil
function ffi.C.ecs_vec_reclaim(allocator, vec, size) end

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@return nil
function ffi.C.ecs_vec_set_size(allocator, vec, size, elem_count) end

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@return nil
function ffi.C.ecs_vec_set_min_size(allocator, vec, size, elem_count) end

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@param ti any
---@return nil
function ffi.C.ecs_vec_set_min_size_w_type_info(allocator, vec, size, elem_count, ti) end

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@return nil
function ffi.C.ecs_vec_set_min_count(allocator, vec, size, elem_count) end

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@return nil
function ffi.C.ecs_vec_set_min_count_zeromem(allocator, vec, size, elem_count) end

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@return nil
function ffi.C.ecs_vec_set_count(allocator, vec, size, elem_count) end

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@param ti any
---@return nil
function ffi.C.ecs_vec_set_count_w_type_info(allocator, vec, size, elem_count, ti) end

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@param ti any
---@return nil
function ffi.C.ecs_vec_set_min_count_w_type_info(allocator, vec, size, elem_count, ti) end

---@param allocator any
---@param vec any
---@param size number
---@param elem_count number
---@return any
function ffi.C.ecs_vec_grow(allocator, vec, size, elem_count) end

---@param vec any
---@return number
function ffi.C.ecs_vec_count(vec) end

---@param vec any
---@return number
function ffi.C.ecs_vec_size(vec) end

---@param vec any
---@param size number
---@param index number
---@return any
function ffi.C.ecs_vec_get(vec, size, index) end

---@param vec any
---@return any
function ffi.C.ecs_vec_first(vec) end

---@param vec any
---@param size number
---@return any
function ffi.C.ecs_vec_last(vec, size) end

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

---@param sparse any
---@param elem_size number
---@return nil
function ffi.C.ecs_sparse_init(sparse, elem_size) end

---@param sparse any
---@param elem_size number
---@return any
function ffi.C.ecs_sparse_add(sparse, elem_size) end

---@param sparse any
---@return number
function ffi.C.ecs_sparse_last_id(sparse) end

---@param sparse any
---@return number
function ffi.C.ecs_sparse_count(sparse) end

---@param sparse any
---@param elem_size number
---@param index number
---@return any
function ffi.C.ecs_sparse_get_dense(sparse, elem_size, index) end

---@param sparse any
---@param elem_size number
---@param id number
---@return any
function ffi.C.ecs_sparse_get(sparse, elem_size, id) end

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

---@param map any
---@param allocator any
---@return nil
function ffi.C.ecs_map_init(map, allocator) end

---@param map any
---@param allocator any
---@return nil
function ffi.C.ecs_map_init_if(map, allocator) end

---@param map any
---@return nil
function ffi.C.ecs_map_reclaim(map) end

---@param map any
---@return nil
function ffi.C.ecs_map_fini(map) end

---@param map any
---@param key any
---@return any
function ffi.C.ecs_map_get(map, key) end

---@param map any
---@param key any
---@return any
function ffi.C.ecs_map_get_deref_(map, key) end

---@param map any
---@param key any
---@return any
function ffi.C.ecs_map_ensure(map, key) end

---@param map any
---@param elem_size number
---@param key any
---@return any
function ffi.C.ecs_map_ensure_alloc(map, elem_size, key) end

---@param map any
---@param key any
---@param value any
---@return nil
function ffi.C.ecs_map_insert(map, key, value) end

---@param map any
---@param elem_size number
---@param key any
---@return any
function ffi.C.ecs_map_insert_alloc(map, elem_size, key) end

---@param map any
---@param key any
---@return any
function ffi.C.ecs_map_remove(map, key) end

---@param map any
---@param key any
---@return nil
function ffi.C.ecs_map_remove_free(map, key) end

---@param map any
---@return nil
function ffi.C.ecs_map_clear(map) end

---@param map any
---@return any
function ffi.C.ecs_map_iter(map) end

---@param iter any
---@return boolean
function ffi.C.ecs_map_iter_valid(iter) end

---@param iter any
---@return boolean
function ffi.C.ecs_map_next(iter) end

---@param dst any
---@param src any
---@return nil
function ffi.C.ecs_map_copy(dst, src) end

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

---@param buffer any
---@param fmt string
---@return nil
function ffi.C.ecs_strbuf_append(buffer, fmt, ...) end

---@param buffer any
---@param fmt string
---@param args any
---@return nil
function ffi.C.ecs_strbuf_vappend(buffer, fmt, args) end

---@param buffer any
---@param str string
---@return nil
function ffi.C.ecs_strbuf_appendstr(buffer, str) end

---@param buffer any
---@param ch number
---@return nil
function ffi.C.ecs_strbuf_appendch(buffer, ch) end

---@param buffer any
---@param v number
---@return nil
function ffi.C.ecs_strbuf_appendint(buffer, v) end

---@param buffer any
---@param v number
---@param nan_delim number
---@return nil
function ffi.C.ecs_strbuf_appendflt(buffer, v, nan_delim) end

---@param buffer any
---@param v boolean
---@return nil
function ffi.C.ecs_strbuf_appendbool(buffer, v) end

---@param dst_buffer any
---@param src_buffer any
---@return nil
function ffi.C.ecs_strbuf_mergebuff(dst_buffer, src_buffer) end

---@param buffer any
---@param str string
---@param n number
---@return nil
function ffi.C.ecs_strbuf_appendstrn(buffer, str, n) end

---@param buffer any
---@return string
function ffi.C.ecs_strbuf_get(buffer) end

---@param buffer any
---@return string
function ffi.C.ecs_strbuf_get_small(buffer) end

---@param buffer any
---@return nil
function ffi.C.ecs_strbuf_reset(buffer) end

---@param buffer any
---@param list_open string
---@param separator string
---@return nil
function ffi.C.ecs_strbuf_list_push(buffer, list_open, separator) end

---@param buffer any
---@param list_close string
---@return nil
function ffi.C.ecs_strbuf_list_pop(buffer, list_close) end

---@param buffer any
---@return nil
function ffi.C.ecs_strbuf_list_next(buffer) end

---@param buffer any
---@param ch number
---@return nil
function ffi.C.ecs_strbuf_list_appendch(buffer, ch) end

---@param buffer any
---@param fmt string
---@return nil
function ffi.C.ecs_strbuf_list_append(buffer, fmt, ...) end

---@param buffer any
---@param str string
---@return nil
function ffi.C.ecs_strbuf_list_appendstr(buffer, str) end

---@param buffer any
---@param str string
---@param n number
---@return nil
function ffi.C.ecs_strbuf_list_appendstrn(buffer, str, n) end

---@param buffer any
---@return number
function ffi.C.ecs_strbuf_written(buffer) end

---@return nil
function ffi.C.ecs_os_init() end

---@return nil
function ffi.C.ecs_os_fini() end

---@param os_api any
---@return nil
function ffi.C.ecs_os_set_api(os_api) end

---@return any
function ffi.C.ecs_os_get_api() end

---@return nil
function ffi.C.ecs_os_set_api_defaults() end

---@param file string
---@param line number
---@param msg string
---@return nil
function ffi.C.ecs_os_dbg(file, line, msg) end

---@param file string
---@param line number
---@param msg string
---@return nil
function ffi.C.ecs_os_trace(file, line, msg) end

---@param file string
---@param line number
---@param msg string
---@return nil
function ffi.C.ecs_os_warn(file, line, msg) end

---@param file string
---@param line number
---@param msg string
---@return nil
function ffi.C.ecs_os_err(file, line, msg) end

---@param file string
---@param line number
---@param msg string
---@return nil
function ffi.C.ecs_os_fatal(file, line, msg) end

---@param err number
---@return string
function ffi.C.ecs_os_strerror(err) end

---@param str string
---@param value string
---@return nil
function ffi.C.ecs_os_strset(str, value) end

---@param file string
---@param line number
---@param name string
---@return nil
function ffi.C.ecs_os_perf_trace_push_(file, line, name) end

---@param file string
---@param line number
---@param name string
---@return nil
function ffi.C.ecs_os_perf_trace_pop_(file, line, name) end

---@param t number
---@return nil
function ffi.C.ecs_sleepf(t) end

---@param start any
---@return number
function ffi.C.ecs_time_measure(start) end

---@param t1 any
---@param t2 any
---@return any
function ffi.C.ecs_time_sub(t1, t2) end

---@param t any
---@return number
function ffi.C.ecs_time_to_double(t) end

---@param src any
---@param size number
---@return any
function ffi.C.ecs_os_memdup(src, size) end

---@return boolean
function ffi.C.ecs_os_has_heap() end

---@return boolean
function ffi.C.ecs_os_has_threading() end

---@return boolean
function ffi.C.ecs_os_has_task_support() end

---@return boolean
function ffi.C.ecs_os_has_time() end

---@return boolean
function ffi.C.ecs_os_has_logging() end

---@return boolean
function ffi.C.ecs_os_has_dl() end

---@return boolean
function ffi.C.ecs_os_has_modules() end

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
---@param entity number
---@return any
function ffi.C.ecs_record_find(world, entity) end

---@param record any
---@return number
function ffi.C.ecs_record_get_entity(record) end

---@param world any
---@param entity number
---@return any
function ffi.C.ecs_write_begin(world, entity) end

---@param record any
---@return nil
function ffi.C.ecs_write_end(record) end

---@param world any
---@param entity number
---@return any
function ffi.C.ecs_read_begin(world, entity) end

---@param record any
---@return nil
function ffi.C.ecs_read_end(record) end

---@param world any
---@param record any
---@param id number
---@return any
function ffi.C.ecs_record_get_id(world, record, id) end

---@param world any
---@param record any
---@param id number
---@return any
function ffi.C.ecs_record_ensure_id(world, record, id) end

---@param world any
---@param record any
---@param id number
---@return boolean
function ffi.C.ecs_record_has_id(world, record, id) end

---@param record any
---@param column number
---@param size number
---@return any
function ffi.C.ecs_record_get_by_column(record, column, size) end

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

---@return any
function ffi.C.ecs_init() end

---@return any
function ffi.C.ecs_mini() end

---@param argc number
---@param argv string
---@return any
function ffi.C.ecs_init_w_args(argc, argv) end

---@param world any
---@return number
function ffi.C.ecs_fini(world) end

---@param world any
---@return boolean
function ffi.C.ecs_is_fini(world) end

---@param world any
---@param action any
---@param ctx any
---@return nil
function ffi.C.ecs_atfini(world, action, ctx) end

---@param world any
---@return any
function ffi.C.ecs_get_entities(world) end

---@param world any
---@return any
function ffi.C.ecs_world_get_flags(world) end

---@param world any
---@param delta_time number
---@return number
function ffi.C.ecs_frame_begin(world, delta_time) end

---@param world any
---@return nil
function ffi.C.ecs_frame_end(world) end

---@param world any
---@param action any
---@param ctx any
---@return nil
function ffi.C.ecs_run_post_frame(world, action, ctx) end

---@param world any
---@return nil
function ffi.C.ecs_quit(world) end

---@param world any
---@return boolean
function ffi.C.ecs_should_quit(world) end

---@param world any
---@param enable boolean
---@return nil
function ffi.C.ecs_measure_frame_time(world, enable) end

---@param world any
---@param enable boolean
---@return nil
function ffi.C.ecs_measure_system_time(world, enable) end

---@param world any
---@param fps number
---@return nil
function ffi.C.ecs_set_target_fps(world, fps) end

---@param world any
---@param flags any
---@return nil
function ffi.C.ecs_set_default_query_flags(world, flags) end

---@param world any
---@param multi_threaded boolean
---@return boolean
function ffi.C.ecs_readonly_begin(world, multi_threaded) end

---@param world any
---@return nil
function ffi.C.ecs_readonly_end(world) end

---@param stage any
---@return nil
function ffi.C.ecs_merge(stage) end

---@param world any
---@return boolean
function ffi.C.ecs_defer_begin(world) end

---@param world any
---@return boolean
function ffi.C.ecs_defer_end(world) end

---@param world any
---@return nil
function ffi.C.ecs_defer_suspend(world) end

---@param world any
---@return nil
function ffi.C.ecs_defer_resume(world) end

---@param world any
---@return boolean
function ffi.C.ecs_is_deferred(world) end

---@param world any
---@return boolean
function ffi.C.ecs_is_defer_suspended(world) end

---@param world any
---@param stages number
---@return nil
function ffi.C.ecs_set_stage_count(world, stages) end

---@param world any
---@return number
function ffi.C.ecs_get_stage_count(world) end

---@param world any
---@param stage_id number
---@return any
function ffi.C.ecs_get_stage(world, stage_id) end

---@param world any
---@return boolean
function ffi.C.ecs_stage_is_readonly(world) end

---@param world any
---@return any
function ffi.C.ecs_stage_new(world) end

---@param stage any
---@return nil
function ffi.C.ecs_stage_free(stage) end

---@param world any
---@return number
function ffi.C.ecs_stage_get_id(world) end

---@param world any
---@param ctx any
---@param ctx_free any
---@return nil
function ffi.C.ecs_set_ctx(world, ctx, ctx_free) end

---@param world any
---@param ctx any
---@param ctx_free any
---@return nil
function ffi.C.ecs_set_binding_ctx(world, ctx, ctx_free) end

---@param world any
---@return any
function ffi.C.ecs_get_ctx(world) end

---@param world any
---@return any
function ffi.C.ecs_get_binding_ctx(world) end

---@return any
function ffi.C.ecs_get_build_info() end

---@param world any
---@return any
function ffi.C.ecs_get_world_info(world) end

---@param world any
---@param entity_count number
---@return nil
function ffi.C.ecs_dim(world, entity_count) end

---@param world any
---@return nil
function ffi.C.ecs_shrink(world) end

---@param world any
---@param id_start number
---@param id_end number
---@return nil
function ffi.C.ecs_set_entity_range(world, id_start, id_end) end

---@param world any
---@param enable boolean
---@return boolean
function ffi.C.ecs_enable_range_check(world, enable) end

---@param world any
---@return number
function ffi.C.ecs_get_max_id(world) end

---@param world any
---@param flags any
---@return nil
function ffi.C.ecs_run_aperiodic(world, flags) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_delete_empty_tables(world, desc) end

---@param poly any
---@return any
function ffi.C.ecs_get_world(poly) end

---@param poly any
---@return number
function ffi.C.ecs_get_entity(poly) end

---@param object any
---@param type_ number
---@return boolean
function ffi.C.flecs_poly_is_(object, type_) end

---@param first number
---@param second number
---@return number
function ffi.C.ecs_make_pair(first, second) end

---@param world any
---@param thread_name string
---@return nil
function ffi.C.ecs_exclusive_access_begin(world, thread_name) end

---@param world any
---@param lock_world boolean
---@return nil
function ffi.C.ecs_exclusive_access_end(world, lock_world) end

---@param world any
---@return number
function ffi.C.ecs_new(world) end

---@param world any
---@return number
function ffi.C.ecs_new_low_id(world) end

---@param world any
---@param component number
---@return number
function ffi.C.ecs_new_w_id(world, component) end

---@param world any
---@param table any
---@return number
function ffi.C.ecs_new_w_table(world, table) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_entity_init(world, desc) end

---@param world any
---@param desc any
---@return any
function ffi.C.ecs_bulk_init(world, desc) end

---@param world any
---@param component number
---@param count number
---@return any
function ffi.C.ecs_bulk_new_w_id(world, component, count) end

---@param world any
---@param dst number
---@param src number
---@param copy_value boolean
---@return number
function ffi.C.ecs_clone(world, dst, src, copy_value) end

---@param world any
---@param entity number
---@return nil
function ffi.C.ecs_delete(world, entity) end

---@param world any
---@param component number
---@return nil
function ffi.C.ecs_delete_with(world, component) end

---@param world any
---@param parent number
---@param children any
---@param child_count number
---@return nil
function ffi.C.ecs_set_child_order(world, parent, children, child_count) end

---@param world any
---@param parent number
---@return any
function ffi.C.ecs_get_ordered_children(world, parent) end

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
---@param component number
---@return nil
function ffi.C.ecs_auto_override_id(world, entity, component) end

---@param world any
---@param entity number
---@return nil
function ffi.C.ecs_clear(world, entity) end

---@param world any
---@param component number
---@return nil
function ffi.C.ecs_remove_all(world, component) end

---@param world any
---@param component number
---@return number
function ffi.C.ecs_set_with(world, component) end

---@param world any
---@return number
function ffi.C.ecs_get_with(world) end

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
---@return boolean
function ffi.C.ecs_is_enabled_id(world, entity, component) end

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
---@param size number
---@return any
function ffi.C.ecs_ensure_id(world, entity, component, size) end

---@param world any
---@param entity number
---@param component number
---@return any
function ffi.C.ecs_ref_init_id(world, entity, component) end

---@param world any
---@param ref any
---@param component number
---@return any
function ffi.C.ecs_ref_get_id(world, ref, component) end

---@param world any
---@param ref any
---@return nil
function ffi.C.ecs_ref_update(world, ref) end

---@param world any
---@param entity number
---@param component number
---@param size number
---@param is_new any
---@return any
function ffi.C.ecs_emplace_id(world, entity, component, size, is_new) end

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

---@param e number
---@return number
function ffi.C.ecs_strip_generation(e) end

---@param world any
---@param e number
---@return number
function ffi.C.ecs_get_alive(world, e) end

---@param world any
---@param entity number
---@return nil
function ffi.C.ecs_make_alive(world, entity) end

---@param world any
---@param component number
---@return nil
function ffi.C.ecs_make_alive_id(world, component) end

---@param world any
---@param entity number
---@return boolean
function ffi.C.ecs_exists(world, entity) end

---@param world any
---@param entity number
---@return nil
function ffi.C.ecs_set_version(world, entity) end

---@param entity number
---@return number
function ffi.C.ecs_get_version(entity) end

---@param world any
---@param entity number
---@return any
function ffi.C.ecs_get_type(world, entity) end

---@param world any
---@param entity number
---@return any
function ffi.C.ecs_get_table(world, entity) end

---@param world any
---@param type_ any
---@return string
function ffi.C.ecs_type_str(world, type_) end

---@param world any
---@param table any
---@return string
function ffi.C.ecs_table_str(world, table) end

---@param world any
---@param entity number
---@return string
function ffi.C.ecs_entity_str(world, entity) end

---@param world any
---@param entity number
---@param component number
---@return boolean
function ffi.C.ecs_has_id(world, entity, component) end

---@param world any
---@param entity number
---@param component number
---@return boolean
function ffi.C.ecs_owns_id(world, entity, component) end

---@param world any
---@param entity number
---@param rel number
---@param index number
---@return number
function ffi.C.ecs_get_target(world, entity, rel, index) end

---@param world any
---@param entity number
---@return number
function ffi.C.ecs_get_parent(world, entity) end

---@param world any
---@param entity number
---@param rel number
---@param component number
---@return number
function ffi.C.ecs_get_target_for_id(world, entity, rel, component) end

---@param world any
---@param entity number
---@param rel number
---@return number
function ffi.C.ecs_get_depth(world, entity, rel) end

---@param world any
---@param entity number
---@return number
function ffi.C.ecs_count_id(world, entity) end

---@param world any
---@param entity number
---@return string
function ffi.C.ecs_get_name(world, entity) end

---@param world any
---@param entity number
---@return string
function ffi.C.ecs_get_symbol(world, entity) end

---@param world any
---@param entity number
---@param name string
---@return number
function ffi.C.ecs_set_name(world, entity, name) end

---@param world any
---@param entity number
---@param symbol string
---@return number
function ffi.C.ecs_set_symbol(world, entity, symbol) end

---@param world any
---@param entity number
---@param alias string
---@return nil
function ffi.C.ecs_set_alias(world, entity, alias) end

---@param world any
---@param path string
---@return number
function ffi.C.ecs_lookup(world, path) end

---@param world any
---@param parent number
---@param name string
---@return number
function ffi.C.ecs_lookup_child(world, parent, name) end

---@param world any
---@param parent number
---@param path string
---@param sep string
---@param prefix string
---@param recursive boolean
---@return number
function ffi.C.ecs_lookup_path_w_sep(world, parent, path, sep, prefix, recursive) end

---@param world any
---@param symbol string
---@param lookup_as_path boolean
---@param recursive boolean
---@return number
function ffi.C.ecs_lookup_symbol(world, symbol, lookup_as_path, recursive) end

---@param world any
---@param parent number
---@param child number
---@param sep string
---@param prefix string
---@return string
function ffi.C.ecs_get_path_w_sep(world, parent, child, sep, prefix) end

---@param world any
---@param parent number
---@param child number
---@param sep string
---@param prefix string
---@param buf any
---@param escape boolean
---@return nil
function ffi.C.ecs_get_path_w_sep_buf(world, parent, child, sep, prefix, buf, escape) end

---@param world any
---@param parent number
---@param path string
---@param sep string
---@param prefix string
---@return number
function ffi.C.ecs_new_from_path_w_sep(world, parent, path, sep, prefix) end

---@param world any
---@param entity number
---@param parent number
---@param path string
---@param sep string
---@param prefix string
---@return number
function ffi.C.ecs_add_path_w_sep(world, entity, parent, path, sep, prefix) end

---@param world any
---@param scope number
---@return number
function ffi.C.ecs_set_scope(world, scope) end

---@param world any
---@return number
function ffi.C.ecs_get_scope(world) end

---@param world any
---@param prefix string
---@return string
function ffi.C.ecs_set_name_prefix(world, prefix) end

---@param world any
---@param lookup_path any
---@return any
function ffi.C.ecs_set_lookup_path(world, lookup_path) end

---@param world any
---@return any
function ffi.C.ecs_get_lookup_path(world) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_component_init(world, desc) end

---@param world any
---@param component number
---@return any
function ffi.C.ecs_get_type_info(world, component) end

---@param world any
---@param component number
---@param hooks any
---@return nil
function ffi.C.ecs_set_hooks_id(world, component, hooks) end

---@param world any
---@param component number
---@return any
function ffi.C.ecs_get_hooks_id(world, component) end

---@param world any
---@param component number
---@return boolean
function ffi.C.ecs_id_is_tag(world, component) end

---@param world any
---@param component number
---@return boolean
function ffi.C.ecs_id_in_use(world, component) end

---@param world any
---@param component number
---@return number
function ffi.C.ecs_get_typeid(world, component) end

---@param component number
---@param pattern number
---@return boolean
function ffi.C.ecs_id_match(component, pattern) end

---@param component number
---@return boolean
function ffi.C.ecs_id_is_pair(component) end

---@param component number
---@return boolean
function ffi.C.ecs_id_is_wildcard(component) end

---@param component number
---@return boolean
function ffi.C.ecs_id_is_any(component) end

---@param world any
---@param component number
---@return boolean
function ffi.C.ecs_id_is_valid(world, component) end

---@param world any
---@param component number
---@return any
function ffi.C.ecs_id_get_flags(world, component) end

---@param component_flags number
---@return string
function ffi.C.ecs_id_flag_str(component_flags) end

---@param world any
---@param component number
---@return string
function ffi.C.ecs_id_str(world, component) end

---@param world any
---@param component number
---@param buf any
---@return nil
function ffi.C.ecs_id_str_buf(world, component, buf) end

---@param world any
---@param expr string
---@return number
function ffi.C.ecs_id_from_str(world, expr) end

---@param ref any
---@return boolean
function ffi.C.ecs_term_ref_is_set(ref) end

---@param term any
---@return boolean
function ffi.C.ecs_term_is_initialized(term) end

---@param term any
---@return boolean
function ffi.C.ecs_term_match_this(term) end

---@param term any
---@return boolean
function ffi.C.ecs_term_match_0(term) end

---@param world any
---@param term any
---@return string
function ffi.C.ecs_term_str(world, term) end

---@param query any
---@return string
function ffi.C.ecs_query_str(query) end

---@param world any
---@param component number
---@return any
function ffi.C.ecs_each_id(world, component) end

---@param it any
---@return boolean
function ffi.C.ecs_each_next(it) end

---@param world any
---@param parent number
---@return any
function ffi.C.ecs_children(world, parent) end

---@param world any
---@param relationship number
---@param parent number
---@return any
function ffi.C.ecs_children_w_rel(world, relationship, parent) end

---@param it any
---@return boolean
function ffi.C.ecs_children_next(it) end

---@param world any
---@param desc any
---@return any
function ffi.C.ecs_query_init(world, desc) end

---@param query any
---@return nil
function ffi.C.ecs_query_fini(query) end

---@param query any
---@param name string
---@return number
function ffi.C.ecs_query_find_var(query, name) end

---@param query any
---@param var_id number
---@return string
function ffi.C.ecs_query_var_name(query, var_id) end

---@param query any
---@param var_id number
---@return boolean
function ffi.C.ecs_query_var_is_entity(query, var_id) end

---@param world any
---@param query any
---@return any
function ffi.C.ecs_query_iter(world, query) end

---@param it any
---@return boolean
function ffi.C.ecs_query_next(it) end

---@param query any
---@param entity number
---@param it any
---@return boolean
function ffi.C.ecs_query_has(query, entity, it) end

---@param query any
---@param table any
---@param it any
---@return boolean
function ffi.C.ecs_query_has_table(query, table, it) end

---@param query any
---@param range any
---@param it any
---@return boolean
function ffi.C.ecs_query_has_range(query, range, it) end

---@param query any
---@return number
function ffi.C.ecs_query_match_count(query) end

---@param query any
---@return string
function ffi.C.ecs_query_plan(query) end

---@param query any
---@param it any
---@return string
function ffi.C.ecs_query_plan_w_profile(query, it) end

---@param query any
---@param it any
---@param expr string
---@return string
function ffi.C.ecs_query_args_parse(query, it, expr) end

---@param query any
---@return boolean
function ffi.C.ecs_query_changed(query) end

---@param world any
---@param query number
---@return any
function ffi.C.ecs_query_get(world, query) end

---@param it any
---@return nil
function ffi.C.ecs_iter_skip(it) end

---@param it any
---@param group_id number
---@return nil
function ffi.C.ecs_iter_set_group(it, group_id) end

---@param query any
---@param group_id number
---@return any
function ffi.C.ecs_query_get_group_ctx(query, group_id) end

---@param query any
---@param group_id number
---@return any
function ffi.C.ecs_query_get_group_info(query, group_id) end

---@param query any
---@return any
function ffi.C.ecs_query_count(query) end

---@param query any
---@return boolean
function ffi.C.ecs_query_is_true(query) end

---@param query any
---@return any
function ffi.C.ecs_query_get_cache_query(query) end

---@param world any
---@param desc any
---@return nil
function ffi.C.ecs_emit(world, desc) end

---@param world any
---@param desc any
---@return nil
function ffi.C.ecs_enqueue(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_observer_init(world, desc) end

---@param world any
---@param observer number
---@return any
function ffi.C.ecs_observer_get(world, observer) end

---@param it any
---@return boolean
function ffi.C.ecs_iter_next(it) end

---@param it any
---@return nil
function ffi.C.ecs_iter_fini(it) end

---@param it any
---@return number
function ffi.C.ecs_iter_count(it) end

---@param it any
---@return boolean
function ffi.C.ecs_iter_is_true(it) end

---@param it any
---@return number
function ffi.C.ecs_iter_first(it) end

---@param it any
---@param var_id number
---@param entity number
---@return nil
function ffi.C.ecs_iter_set_var(it, var_id, entity) end

---@param it any
---@param var_id number
---@param table any
---@return nil
function ffi.C.ecs_iter_set_var_as_table(it, var_id, table) end

---@param it any
---@param var_id number
---@param range any
---@return nil
function ffi.C.ecs_iter_set_var_as_range(it, var_id, range) end

---@param it any
---@param var_id number
---@return number
function ffi.C.ecs_iter_get_var(it, var_id) end

---@param it any
---@param var_id number
---@return string
function ffi.C.ecs_iter_get_var_name(it, var_id) end

---@param it any
---@return number
function ffi.C.ecs_iter_get_var_count(it) end

---@param it any
---@return any
function ffi.C.ecs_iter_get_vars(it) end

---@param it any
---@param var_id number
---@return any
function ffi.C.ecs_iter_get_var_as_table(it, var_id) end

---@param it any
---@param var_id number
---@return any
function ffi.C.ecs_iter_get_var_as_range(it, var_id) end

---@param it any
---@param var_id number
---@return boolean
function ffi.C.ecs_iter_var_is_constrained(it, var_id) end

---@param it any
---@return number
function ffi.C.ecs_iter_get_group(it) end

---@param it any
---@return boolean
function ffi.C.ecs_iter_changed(it) end

---@param it any
---@return string
function ffi.C.ecs_iter_str(it) end

---@param it any
---@param offset number
---@param limit number
---@return any
function ffi.C.ecs_page_iter(it, offset, limit) end

---@param it any
---@return boolean
function ffi.C.ecs_page_next(it) end

---@param it any
---@param index number
---@param count number
---@return any
function ffi.C.ecs_worker_iter(it, index, count) end

---@param it any
---@return boolean
function ffi.C.ecs_worker_next(it) end

---@param it any
---@param size number
---@param index number
---@return any
function ffi.C.ecs_field_w_size(it, size, index) end

---@param it any
---@param size number
---@param index number
---@param row number
---@return any
function ffi.C.ecs_field_at_w_size(it, size, index, row) end

---@param it any
---@param index number
---@return boolean
function ffi.C.ecs_field_is_readonly(it, index) end

---@param it any
---@param index number
---@return boolean
function ffi.C.ecs_field_is_writeonly(it, index) end

---@param it any
---@param index number
---@return boolean
function ffi.C.ecs_field_is_set(it, index) end

---@param it any
---@param index number
---@return number
function ffi.C.ecs_field_id(it, index) end

---@param it any
---@param index number
---@return number
function ffi.C.ecs_field_column(it, index) end

---@param it any
---@param index number
---@return number
function ffi.C.ecs_field_src(it, index) end

---@param it any
---@param index number
---@return number
function ffi.C.ecs_field_size(it, index) end

---@param it any
---@param index number
---@return boolean
function ffi.C.ecs_field_is_self(it, index) end

---@param table any
---@return any
function ffi.C.ecs_table_get_type(table) end

---@param world any
---@param table any
---@param component number
---@return number
function ffi.C.ecs_table_get_type_index(world, table, component) end

---@param world any
---@param table any
---@param component number
---@return number
function ffi.C.ecs_table_get_column_index(world, table, component) end

---@param table any
---@return number
function ffi.C.ecs_table_column_count(table) end

---@param table any
---@param index number
---@return number
function ffi.C.ecs_table_type_to_column_index(table, index) end

---@param table any
---@param index number
---@return number
function ffi.C.ecs_table_column_to_type_index(table, index) end

---@param table any
---@param index number
---@param offset number
---@return any
function ffi.C.ecs_table_get_column(table, index, offset) end

---@param world any
---@param table any
---@param component number
---@param offset number
---@return any
function ffi.C.ecs_table_get_id(world, table, component, offset) end

---@param table any
---@param index number
---@return number
function ffi.C.ecs_table_get_column_size(table, index) end

---@param table any
---@return number
function ffi.C.ecs_table_count(table) end

---@param table any
---@return number
function ffi.C.ecs_table_size(table) end

---@param table any
---@return any
function ffi.C.ecs_table_entities(table) end

---@param world any
---@param table any
---@param component number
---@return boolean
function ffi.C.ecs_table_has_id(world, table, component) end

---@param world any
---@param table any
---@param relationship number
---@param index number
---@return number
function ffi.C.ecs_table_get_target(world, table, relationship, index) end

---@param world any
---@param table any
---@param rel number
---@return number
function ffi.C.ecs_table_get_depth(world, table, rel) end

---@param world any
---@param table any
---@param component number
---@return any
function ffi.C.ecs_table_add_id(world, table, component) end

---@param world any
---@param ids any
---@param id_count number
---@return any
function ffi.C.ecs_table_find(world, ids, id_count) end

---@param world any
---@param table any
---@param component number
---@return any
function ffi.C.ecs_table_remove_id(world, table, component) end

---@param world any
---@param table any
---@return nil
function ffi.C.ecs_table_lock(world, table) end

---@param world any
---@param table any
---@return nil
function ffi.C.ecs_table_unlock(world, table) end

---@param table any
---@param flags any
---@return boolean
function ffi.C.ecs_table_has_flags(table, flags) end

---@param table any
---@return boolean
function ffi.C.ecs_table_has_traversable(table) end

---@param world any
---@param table any
---@param row_1 number
---@param row_2 number
---@return nil
function ffi.C.ecs_table_swap_rows(world, table, row_1, row_2) end

---@param world any
---@param entity number
---@param record any
---@param table any
---@param added any
---@param removed any
---@return boolean
function ffi.C.ecs_commit(world, entity, record, table, added, removed) end

---@param world any
---@param table any
---@param component number
---@param component_out any
---@return number
function ffi.C.ecs_search(world, table, component, component_out) end

---@param world any
---@param table any
---@param offset number
---@param component number
---@param component_out any
---@return number
function ffi.C.ecs_search_offset(world, table, offset, component, component_out) end

---@param world any
---@param table any
---@param offset number
---@param component number
---@param rel number
---@param flags any
---@param subject_out any
---@param component_out any
---@param tr_out any
---@return number
function ffi.C.ecs_search_relation(world, table, offset, component, rel, flags, subject_out, component_out, tr_out) end

---@param world any
---@param table any
---@return nil
function ffi.C.ecs_table_clear_entities(world, table) end

---@param world any
---@param type_ number
---@param ptr any
---@return number
function ffi.C.ecs_value_init(world, type_, ptr) end

---@param world any
---@param ti any
---@param ptr any
---@return number
function ffi.C.ecs_value_init_w_type_info(world, ti, ptr) end

---@param world any
---@param type_ number
---@return any
function ffi.C.ecs_value_new(world, type_) end

---@param world any
---@param ti any
---@return any
function ffi.C.ecs_value_new_w_type_info(world, ti) end

---@param world any
---@param ti any
---@param ptr any
---@return number
function ffi.C.ecs_value_fini_w_type_info(world, ti, ptr) end

---@param world any
---@param type_ number
---@param ptr any
---@return number
function ffi.C.ecs_value_fini(world, type_, ptr) end

---@param world any
---@param type_ number
---@param ptr any
---@return number
function ffi.C.ecs_value_free(world, type_, ptr) end

---@param world any
---@param ti any
---@param dst any
---@param src any
---@return number
function ffi.C.ecs_value_copy_w_type_info(world, ti, dst, src) end

---@param world any
---@param type_ number
---@param dst any
---@param src any
---@return number
function ffi.C.ecs_value_copy(world, type_, dst, src) end

---@param world any
---@param ti any
---@param dst any
---@param src any
---@return number
function ffi.C.ecs_value_move_w_type_info(world, ti, dst, src) end

---@param world any
---@param type_ number
---@param dst any
---@param src any
---@return number
function ffi.C.ecs_value_move(world, type_, dst, src) end

---@param world any
---@param ti any
---@param dst any
---@param src any
---@return number
function ffi.C.ecs_value_move_ctor_w_type_info(world, ti, dst, src) end

---@param world any
---@param type_ number
---@param dst any
---@param src any
---@return number
function ffi.C.ecs_value_move_ctor(world, type_, dst, src) end

---@param file string
---@param line number
---@param msg string
---@return nil
function ffi.C.ecs_deprecated_(file, line, msg) end

---@param level number
---@return nil
function ffi.C.ecs_log_push_(level) end

---@param level number
---@return nil
function ffi.C.ecs_log_pop_(level) end

---@param level number
---@return boolean
function ffi.C.ecs_should_log(level) end

---@param error_code number
---@return string
function ffi.C.ecs_strerror(error_code) end

---@param level number
---@param file string
---@param line number
---@param fmt string
---@return nil
function ffi.C.ecs_print_(level, file, line, fmt, ...) end

---@param level number
---@param file string
---@param line number
---@param fmt string
---@param args any
---@return nil
function ffi.C.ecs_printv_(level, file, line, fmt, args) end

---@param level number
---@param file string
---@param line number
---@param fmt string
---@return nil
function ffi.C.ecs_log_(level, file, line, fmt, ...) end

---@param level number
---@param file string
---@param line number
---@param fmt string
---@param args any
---@return nil
function ffi.C.ecs_logv_(level, file, line, fmt, args) end

---@param error_code number
---@param file string
---@param line number
---@param fmt string
---@return nil
function ffi.C.ecs_abort_(error_code, file, line, fmt, ...) end

---@param error_code number
---@param condition_str string
---@param file string
---@param line number
---@param fmt string
---@return nil
function ffi.C.ecs_assert_log_(error_code, condition_str, file, line, fmt, ...) end

---@param name string
---@param expr string
---@param column number
---@param fmt string
---@return nil
function ffi.C.ecs_parser_error_(name, expr, column, fmt, ...) end

---@param name string
---@param expr string
---@param column number
---@param fmt string
---@param args any
---@return nil
function ffi.C.ecs_parser_errorv_(name, expr, column, fmt, args) end

---@param name string
---@param expr string
---@param column number
---@param fmt string
---@return nil
function ffi.C.ecs_parser_warning_(name, expr, column, fmt, ...) end

---@param name string
---@param expr string
---@param column number
---@param fmt string
---@param args any
---@return nil
function ffi.C.ecs_parser_warningv_(name, expr, column, fmt, args) end

---@param level number
---@return number
function ffi.C.ecs_log_set_level(level) end

---@return number
function ffi.C.ecs_log_get_level() end

---@param enabled boolean
---@return boolean
function ffi.C.ecs_log_enable_colors(enabled) end

---@param enabled boolean
---@return boolean
function ffi.C.ecs_log_enable_timestamp(enabled) end

---@param enabled boolean
---@return boolean
function ffi.C.ecs_log_enable_timedelta(enabled) end

---@return number
function ffi.C.ecs_log_last_error() end

---@param capture_try boolean
---@return nil
function ffi.C.ecs_log_start_capture(capture_try) end

---@return string
function ffi.C.ecs_log_stop_capture() end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_app_run(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_app_run_frame(world, desc) end

---@param callback any
---@return number
function ffi.C.ecs_app_set_run_action(callback) end

---@param callback any
---@return number
function ffi.C.ecs_app_set_frame_action(callback) end

---@param desc any
---@return any
function ffi.C.ecs_http_server_init(desc) end

---@param server any
---@return nil
function ffi.C.ecs_http_server_fini(server) end

---@param server any
---@return number
function ffi.C.ecs_http_server_start(server) end

---@param server any
---@param delta_time number
---@return nil
function ffi.C.ecs_http_server_dequeue(server, delta_time) end

---@param server any
---@return nil
function ffi.C.ecs_http_server_stop(server) end

---@param srv any
---@param req string
---@param len number
---@param reply_out any
---@return number
function ffi.C.ecs_http_server_http_request(srv, req, len, reply_out) end

---@param srv any
---@param method string
---@param req string
---@param body string
---@param reply_out any
---@return number
function ffi.C.ecs_http_server_request(srv, method, req, body, reply_out) end

---@param srv any
---@return any
function ffi.C.ecs_http_server_ctx(srv) end

---@param req any
---@param name string
---@return string
function ffi.C.ecs_http_get_header(req, name) end

---@param req any
---@param name string
---@return string
function ffi.C.ecs_http_get_param(req, name) end

---@param world any
---@param desc any
---@return any
function ffi.C.ecs_rest_server_init(world, desc) end

---@param srv any
---@return nil
function ffi.C.ecs_rest_server_fini(srv) end

---@param world any
---@return nil
function ffi.C.FlecsRestImport(world) end

---@param world any
---@param tick_source number
---@param timeout number
---@return number
function ffi.C.ecs_set_timeout(world, tick_source, timeout) end

---@param world any
---@param tick_source number
---@return number
function ffi.C.ecs_get_timeout(world, tick_source) end

---@param world any
---@param tick_source number
---@param interval number
---@return number
function ffi.C.ecs_set_interval(world, tick_source, interval) end

---@param world any
---@param tick_source number
---@return number
function ffi.C.ecs_get_interval(world, tick_source) end

---@param world any
---@param tick_source number
---@return nil
function ffi.C.ecs_start_timer(world, tick_source) end

---@param world any
---@param tick_source number
---@return nil
function ffi.C.ecs_stop_timer(world, tick_source) end

---@param world any
---@param tick_source number
---@return nil
function ffi.C.ecs_reset_timer(world, tick_source) end

---@param world any
---@return nil
function ffi.C.ecs_randomize_timers(world) end

---@param world any
---@param tick_source number
---@param rate number
---@param source number
---@return number
function ffi.C.ecs_set_rate(world, tick_source, rate, source) end

---@param world any
---@param system number
---@param tick_source number
---@return nil
function ffi.C.ecs_set_tick_source(world, system, tick_source) end

---@param world any
---@return nil
function ffi.C.FlecsTimerImport(world) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_pipeline_init(world, desc) end

---@param world any
---@param pipeline number
---@return nil
function ffi.C.ecs_set_pipeline(world, pipeline) end

---@param world any
---@return number
function ffi.C.ecs_get_pipeline(world) end

---@param world any
---@param delta_time number
---@return boolean
function ffi.C.ecs_progress(world, delta_time) end

---@param world any
---@param scale number
---@return nil
function ffi.C.ecs_set_time_scale(world, scale) end

---@param world any
---@return nil
function ffi.C.ecs_reset_clock(world) end

---@param world any
---@param pipeline number
---@param delta_time number
---@return nil
function ffi.C.ecs_run_pipeline(world, pipeline, delta_time) end

---@param world any
---@param threads number
---@return nil
function ffi.C.ecs_set_threads(world, threads) end

---@param world any
---@param task_threads number
---@return nil
function ffi.C.ecs_set_task_threads(world, task_threads) end

---@param world any
---@return boolean
function ffi.C.ecs_using_task_threads(world) end

---@param world any
---@return nil
function ffi.C.FlecsPipelineImport(world) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_system_init(world, desc) end

---@param world any
---@param system number
---@return any
function ffi.C.ecs_system_get(world, system) end

---@param world any
---@param system number
---@param delta_time number
---@param param any
---@return number
function ffi.C.ecs_run(world, system, delta_time, param) end

---@param world any
---@param system number
---@param stage_current number
---@param stage_count number
---@param delta_time number
---@param param any
---@return number
function ffi.C.ecs_run_worker(world, system, stage_current, stage_count, delta_time, param) end

---@param world any
---@return nil
function ffi.C.FlecsSystemImport(world) end

---@param world any
---@param stats any
---@return nil
function ffi.C.ecs_world_stats_get(world, stats) end

---@param dst any
---@param src any
---@return nil
function ffi.C.ecs_world_stats_reduce(dst, src) end

---@param stats any
---@param old any
---@param count number
---@return nil
function ffi.C.ecs_world_stats_reduce_last(stats, old, count) end

---@param stats any
---@return nil
function ffi.C.ecs_world_stats_repeat_last(stats) end

---@param dst any
---@param src any
---@return nil
function ffi.C.ecs_world_stats_copy_last(dst, src) end

---@param world any
---@param stats any
---@return nil
function ffi.C.ecs_world_stats_log(world, stats) end

---@param world any
---@param query any
---@param stats any
---@return nil
function ffi.C.ecs_query_stats_get(world, query, stats) end

---@param dst any
---@param src any
---@return nil
function ffi.C.ecs_query_cache_stats_reduce(dst, src) end

---@param stats any
---@param old any
---@param count number
---@return nil
function ffi.C.ecs_query_cache_stats_reduce_last(stats, old, count) end

---@param stats any
---@return nil
function ffi.C.ecs_query_cache_stats_repeat_last(stats) end

---@param dst any
---@param src any
---@return nil
function ffi.C.ecs_query_cache_stats_copy_last(dst, src) end

---@param world any
---@param system number
---@param stats any
---@return boolean
function ffi.C.ecs_system_stats_get(world, system, stats) end

---@param dst any
---@param src any
---@return nil
function ffi.C.ecs_system_stats_reduce(dst, src) end

---@param stats any
---@param old any
---@param count number
---@return nil
function ffi.C.ecs_system_stats_reduce_last(stats, old, count) end

---@param stats any
---@return nil
function ffi.C.ecs_system_stats_repeat_last(stats) end

---@param dst any
---@param src any
---@return nil
function ffi.C.ecs_system_stats_copy_last(dst, src) end

---@param world any
---@param pipeline number
---@param stats any
---@return boolean
function ffi.C.ecs_pipeline_stats_get(world, pipeline, stats) end

---@param stats any
---@return nil
function ffi.C.ecs_pipeline_stats_fini(stats) end

---@param dst any
---@param src any
---@return nil
function ffi.C.ecs_pipeline_stats_reduce(dst, src) end

---@param stats any
---@param old any
---@param count number
---@return nil
function ffi.C.ecs_pipeline_stats_reduce_last(stats, old, count) end

---@param stats any
---@return nil
function ffi.C.ecs_pipeline_stats_repeat_last(stats) end

---@param dst any
---@param src any
---@return nil
function ffi.C.ecs_pipeline_stats_copy_last(dst, src) end

---@param dst any
---@param src any
---@param t_dst number
---@param t_src number
---@return nil
function ffi.C.ecs_metric_reduce(dst, src, t_dst, t_src) end

---@param m any
---@param t number
---@param count number
---@return nil
function ffi.C.ecs_metric_reduce_last(m, t, count) end

---@param m any
---@param dst number
---@param src number
---@return nil
function ffi.C.ecs_metric_copy(m, dst, src) end

---@param world any
---@return any
function ffi.C.ecs_entity_memory_get(world) end

---@param cr any
---@param result any
---@return nil
function ffi.C.ecs_component_record_memory_get(cr, result) end

---@param world any
---@return any
function ffi.C.ecs_component_index_memory_get(world) end

---@param query any
---@param result any
---@return nil
function ffi.C.ecs_query_memory_get(query, result) end

---@param world any
---@return any
function ffi.C.ecs_queries_memory_get(world) end

---@param table any
---@param result any
---@return nil
function ffi.C.ecs_table_component_memory_get(table, result) end

---@param world any
---@return any
function ffi.C.ecs_component_memory_get(world) end

---@param table any
---@param result any
---@return nil
function ffi.C.ecs_table_memory_get(table, result) end

---@param world any
---@return any
function ffi.C.ecs_tables_memory_get(world) end

---@param world any
---@return any
function ffi.C.ecs_table_histogram_get(world) end

---@param world any
---@return any
function ffi.C.ecs_misc_memory_get(world) end

---@param world any
---@return any
function ffi.C.ecs_allocator_memory_get(world) end

---@param world any
---@return number
function ffi.C.ecs_memory_get(world) end

---@param world any
---@return nil
function ffi.C.FlecsStatsImport(world) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_metric_init(world, desc) end

---@param world any
---@return nil
function ffi.C.FlecsMetricsImport(world) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_alert_init(world, desc) end

---@param world any
---@param entity number
---@param alert number
---@return number
function ffi.C.ecs_get_alert_count(world, entity, alert) end

---@param world any
---@param entity number
---@param alert number
---@return number
function ffi.C.ecs_get_alert(world, entity, alert) end

---@param world any
---@return nil
function ffi.C.FlecsAlertsImport(world) end

---@param world any
---@param type_ number
---@param ptr any
---@param json string
---@param desc any
---@return string
function ffi.C.ecs_ptr_from_json(world, type_, ptr, json, desc) end

---@param world any
---@param entity number
---@param json string
---@param desc any
---@return string
function ffi.C.ecs_entity_from_json(world, entity, json, desc) end

---@param world any
---@param json string
---@param desc any
---@return string
function ffi.C.ecs_world_from_json(world, json, desc) end

---@param world any
---@param filename string
---@param desc any
---@return string
function ffi.C.ecs_world_from_json_file(world, filename, desc) end

---@param world any
---@param type_ number
---@param data any
---@param count number
---@return string
function ffi.C.ecs_array_to_json(world, type_, data, count) end

---@param world any
---@param type_ number
---@param data any
---@param count number
---@param buf_out any
---@return number
function ffi.C.ecs_array_to_json_buf(world, type_, data, count, buf_out) end

---@param world any
---@param type_ number
---@param data any
---@return string
function ffi.C.ecs_ptr_to_json(world, type_, data) end

---@param world any
---@param type_ number
---@param data any
---@param buf_out any
---@return number
function ffi.C.ecs_ptr_to_json_buf(world, type_, data, buf_out) end

---@param world any
---@param type_ number
---@return string
function ffi.C.ecs_type_info_to_json(world, type_) end

---@param world any
---@param type_ number
---@param buf_out any
---@return number
function ffi.C.ecs_type_info_to_json_buf(world, type_, buf_out) end

---@param world any
---@param entity number
---@param desc any
---@return string
function ffi.C.ecs_entity_to_json(world, entity, desc) end

---@param world any
---@param entity number
---@param buf_out any
---@param desc any
---@return number
function ffi.C.ecs_entity_to_json_buf(world, entity, buf_out, desc) end

---@param iter any
---@param desc any
---@return string
function ffi.C.ecs_iter_to_json(iter, desc) end

---@param iter any
---@param buf_out any
---@param desc any
---@return number
function ffi.C.ecs_iter_to_json_buf(iter, buf_out, desc) end

---@param world any
---@param desc any
---@return string
function ffi.C.ecs_world_to_json(world, desc) end

---@param world any
---@param buf_out any
---@param desc any
---@return number
function ffi.C.ecs_world_to_json_buf(world, buf_out, desc) end

---@param world any
---@return nil
function ffi.C.FlecsUnitsImport(world) end

---@param world any
---@param name string
---@param code string
---@param desc any
---@param result any
---@return any
function ffi.C.ecs_script_parse(world, name, code, desc, result) end

---@param script any
---@param desc any
---@param result any
---@return number
function ffi.C.ecs_script_eval(script, desc, result) end

---@param script any
---@return nil
function ffi.C.ecs_script_free(script) end

---@param world any
---@param name string
---@param code string
---@param result any
---@return number
function ffi.C.ecs_script_run(world, name, code, result) end

---@param world any
---@param filename string
---@return number
function ffi.C.ecs_script_run_file(world, filename) end

---@return any
function ffi.C.ecs_script_runtime_new() end

---@param runtime any
---@return nil
function ffi.C.ecs_script_runtime_free(runtime) end

---@param script any
---@param buf any
---@param colors boolean
---@return number
function ffi.C.ecs_script_ast_to_buf(script, buf, colors) end

---@param script any
---@param colors boolean
---@return string
function ffi.C.ecs_script_ast_to_str(script, colors) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_script_init(world, desc) end

---@param world any
---@param script number
---@param instance number
---@param code string
---@return number
function ffi.C.ecs_script_update(world, script, instance, code) end

---@param world any
---@param script number
---@param instance number
---@return nil
function ffi.C.ecs_script_clear(world, script, instance) end

---@param world any
---@return any
function ffi.C.ecs_script_vars_init(world) end

---@param vars any
---@return nil
function ffi.C.ecs_script_vars_fini(vars) end

---@param parent any
---@return any
function ffi.C.ecs_script_vars_push(parent) end

---@param vars any
---@return any
function ffi.C.ecs_script_vars_pop(vars) end

---@param vars any
---@param name string
---@return any
function ffi.C.ecs_script_vars_declare(vars, name) end

---@param vars any
---@param name string
---@param type_ number
---@return any
function ffi.C.ecs_script_vars_define_id(vars, name, type_) end

---@param vars any
---@param name string
---@return any
function ffi.C.ecs_script_vars_lookup(vars, name) end

---@param vars any
---@param sp number
---@return any
function ffi.C.ecs_script_vars_from_sp(vars, sp) end

---@param vars any
---@return nil
function ffi.C.ecs_script_vars_print(vars) end

---@param vars any
---@param count number
---@return nil
function ffi.C.ecs_script_vars_set_size(vars, count) end

---@param it any
---@param vars any
---@param offset number
---@return nil
function ffi.C.ecs_script_vars_from_iter(it, vars, offset) end

---@param world any
---@param ptr string
---@param value any
---@param desc any
---@return string
function ffi.C.ecs_expr_run(world, ptr, value, desc) end

---@param world any
---@param expr string
---@param desc any
---@return any
function ffi.C.ecs_expr_parse(world, expr, desc) end

---@param script any
---@param value any
---@param desc any
---@return number
function ffi.C.ecs_expr_eval(script, value, desc) end

---@param world any
---@param str string
---@param vars any
---@return string
function ffi.C.ecs_script_string_interpolate(world, str, vars) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_const_var_init(world, desc) end

---@param world any
---@param var number
---@return any
function ffi.C.ecs_const_var_get(world, var) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_function_init(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_method_init(world, desc) end

---@param world any
---@param type_ number
---@param data any
---@return string
function ffi.C.ecs_ptr_to_expr(world, type_, data) end

---@param world any
---@param type_ number
---@param data any
---@param buf any
---@return number
function ffi.C.ecs_ptr_to_expr_buf(world, type_, data, buf) end

---@param world any
---@param type_ number
---@param data any
---@return string
function ffi.C.ecs_ptr_to_str(world, type_, data) end

---@param world any
---@param type_ number
---@param data any
---@param buf any
---@return number
function ffi.C.ecs_ptr_to_str_buf(world, type_, data, buf) end

---@param world any
---@return nil
function ffi.C.FlecsScriptImport(world) end

---@param world any
---@param entity number
---@param uuid string
---@return nil
function ffi.C.ecs_doc_set_uuid(world, entity, uuid) end

---@param world any
---@param entity number
---@param name string
---@return nil
function ffi.C.ecs_doc_set_name(world, entity, name) end

---@param world any
---@param entity number
---@param description string
---@return nil
function ffi.C.ecs_doc_set_brief(world, entity, description) end

---@param world any
---@param entity number
---@param description string
---@return nil
function ffi.C.ecs_doc_set_detail(world, entity, description) end

---@param world any
---@param entity number
---@param link string
---@return nil
function ffi.C.ecs_doc_set_link(world, entity, link) end

---@param world any
---@param entity number
---@param color string
---@return nil
function ffi.C.ecs_doc_set_color(world, entity, color) end

---@param world any
---@param entity number
---@return string
function ffi.C.ecs_doc_get_uuid(world, entity) end

---@param world any
---@param entity number
---@return string
function ffi.C.ecs_doc_get_name(world, entity) end

---@param world any
---@param entity number
---@return string
function ffi.C.ecs_doc_get_brief(world, entity) end

---@param world any
---@param entity number
---@return string
function ffi.C.ecs_doc_get_detail(world, entity) end

---@param world any
---@param entity number
---@return string
function ffi.C.ecs_doc_get_link(world, entity) end

---@param world any
---@param entity number
---@return string
function ffi.C.ecs_doc_get_color(world, entity) end

---@param world any
---@return nil
function ffi.C.FlecsDocImport(world) end

---@param world any
---@param type_ number
---@return string
function ffi.C.ecs_meta_serializer_to_str(world, type_) end

---@param world any
---@param type_ number
---@param ptr any
---@return any
function ffi.C.ecs_meta_cursor(world, type_, ptr) end

---@param cursor any
---@return any
function ffi.C.ecs_meta_get_ptr(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_next(cursor) end

---@param cursor any
---@param elem number
---@return number
function ffi.C.ecs_meta_elem(cursor, elem) end

---@param cursor any
---@param name string
---@return number
function ffi.C.ecs_meta_member(cursor, name) end

---@param cursor any
---@param name string
---@return number
function ffi.C.ecs_meta_try_member(cursor, name) end

---@param cursor any
---@param name string
---@return number
function ffi.C.ecs_meta_dotmember(cursor, name) end

---@param cursor any
---@param name string
---@return number
function ffi.C.ecs_meta_try_dotmember(cursor, name) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_push(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_pop(cursor) end

---@param cursor any
---@return boolean
function ffi.C.ecs_meta_is_collection(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_get_type(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_get_unit(cursor) end

---@param cursor any
---@return string
function ffi.C.ecs_meta_get_member(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_get_member_id(cursor) end

---@param cursor any
---@param value boolean
---@return number
function ffi.C.ecs_meta_set_bool(cursor, value) end

---@param cursor any
---@param value number
---@return number
function ffi.C.ecs_meta_set_char(cursor, value) end

---@param cursor any
---@param value number
---@return number
function ffi.C.ecs_meta_set_int(cursor, value) end

---@param cursor any
---@param value number
---@return number
function ffi.C.ecs_meta_set_uint(cursor, value) end

---@param cursor any
---@param value number
---@return number
function ffi.C.ecs_meta_set_float(cursor, value) end

---@param cursor any
---@param value string
---@return number
function ffi.C.ecs_meta_set_string(cursor, value) end

---@param cursor any
---@param value string
---@return number
function ffi.C.ecs_meta_set_string_literal(cursor, value) end

---@param cursor any
---@param value number
---@return number
function ffi.C.ecs_meta_set_entity(cursor, value) end

---@param cursor any
---@param value number
---@return number
function ffi.C.ecs_meta_set_id(cursor, value) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_set_null(cursor) end

---@param cursor any
---@param value any
---@return number
function ffi.C.ecs_meta_set_value(cursor, value) end

---@param cursor any
---@return boolean
function ffi.C.ecs_meta_get_bool(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_get_char(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_get_int(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_get_uint(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_get_float(cursor) end

---@param cursor any
---@return string
function ffi.C.ecs_meta_get_string(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_get_entity(cursor) end

---@param cursor any
---@return number
function ffi.C.ecs_meta_get_id(cursor) end

---@param type_kind any
---@param ptr any
---@return number
function ffi.C.ecs_meta_ptr_to_float(type_kind, ptr) end

---@param op any
---@param ptr any
---@return number
function ffi.C.ecs_meta_op_get_elem_count(op, ptr) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_primitive_init(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_enum_init(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_bitmask_init(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_array_init(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_vector_init(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_struct_init(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_opaque_init(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_unit_init(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_unit_prefix_init(world, desc) end

---@param world any
---@param desc any
---@return number
function ffi.C.ecs_quantity_init(world, desc) end

---@param world any
---@return nil
function ffi.C.FlecsMetaImport(world) end

---@param world any
---@param component number
---@param kind any
---@param desc string
---@return number
function ffi.C.ecs_meta_from_desc(world, component, kind, desc) end

---@return nil
function ffi.C.ecs_set_os_api_impl() end

---@param world any
---@param module any
---@param module_name string
---@return number
function ffi.C.ecs_import(world, module, module_name) end

---@param world any
---@param module any
---@param module_name_c string
---@return number
function ffi.C.ecs_import_c(world, module, module_name_c) end

---@param world any
---@param library_name string
---@param module_name string
---@return number
function ffi.C.ecs_import_from_library(world, library_name, module_name) end

---@param world any
---@param c_name string
---@param desc any
---@return number
function ffi.C.ecs_module_init(world, c_name, desc) end

---@param type_name string
---@param func_name string
---@param len number
---@param front_len number
---@return string
function ffi.C.ecs_cpp_get_type_name(type_name, func_name, len, front_len) end

---@param symbol_name string
---@param type_name string
---@param len number
---@return string
function ffi.C.ecs_cpp_get_symbol_name(symbol_name, type_name, len) end

---@param constant_name string
---@param func_name string
---@param len number
---@param back_len number
---@return string
function ffi.C.ecs_cpp_get_constant_name(constant_name, func_name, len, back_len) end

---@param world any
---@param type_name string
---@return string
function ffi.C.ecs_cpp_trim_module(world, type_name) end

---@param world any
---@param id number
---@param ids_index number
---@param name string
---@param cpp_name string
---@param cpp_symbol string
---@param size number
---@param alignment number
---@param is_component boolean
---@param explicit_registration boolean
---@param registered_out any
---@param existing_out any
---@return number
function ffi.C.ecs_cpp_component_register(world, id, ids_index, name, cpp_name, cpp_symbol, size, alignment, is_component, explicit_registration, registered_out, existing_out) end

---@param world any
---@param id number
---@param underlying_type number
---@return nil
function ffi.C.ecs_cpp_enum_init(world, id, underlying_type) end

---@param world any
---@param parent number
---@param id number
---@param name string
---@param value any
---@param value_type number
---@param value_size number
---@return number
function ffi.C.ecs_cpp_enum_constant_register(world, parent, id, name, value, value_type, value_size) end

---@param world any
---@param entity number
---@param component number
---@param new_ptr any
---@param size number
---@return any
function ffi.C.ecs_cpp_set(world, entity, component, new_ptr, size) end

---@param world any
---@param entity number
---@param component number
---@param new_ptr any
---@param size number
---@return any
function ffi.C.ecs_cpp_assign(world, entity, component, new_ptr, size) end

---@param world any
---@param type_ number
---@return any
function ffi.C.ecs_cpp_last_member(world, type_) end

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
