import re

path = 'spectre/source/modules/components/components_module.cpp'
with open(path, 'r') as f:
    content = f.read()

replacement = """
    // Add component to target entity
    if (!ecs_is_valid(world, target_entity)) { sandbox::modules::logs::error(w, "target_entity is invalid"); return; }
    
    // We must use ecs_set_id in case the world is in deferred mode!
    // Allocate a temporary buffer for the component data
    const ecs_type_info_t* ti = ecs_get_type_info(world, comp_id.id());
    if (!ti) { sandbox::modules::logs::error(w, "type info is null"); return; }
    
    size_t comp_size = ti->size;
    void* temp_buffer = calloc(1, comp_size);
    
    auto* header = static_cast<spectre_dynamic_component_header_t*>(temp_buffer);
    header->schema_entity = schema_entity.id();
    sandbox::modules::logs::info(w, "Deserialize: target={}, comp_id={}, schema_entity={}", target_entity, comp_id.id(), schema_entity.id());
    
    uint8_t* base = static_cast<uint8_t*>(temp_buffer) + sizeof(spectre_dynamic_component_header_t);
    
    sandbox::properties input(props, false);
    for (const auto& field : schema->fields) {
        if (field.type == components_module_t::SPECTRE_DYNAMIC_TYPE_INT) {
            int32_t val = input.get<int32_t>(field.name).value_or(0);
            memcpy(base + field.offset, &val, sizeof(int32_t));
        } else if (field.type == components_module_t::SPECTRE_DYNAMIC_TYPE_FLOAT) {
            float val = input.get<float>(field.name).value_or(0.0f);
            memcpy(base + field.offset, &val, sizeof(float));
        } else if (field.type == components_module_t::SPECTRE_DYNAMIC_TYPE_DOUBLE) {
            double val = input.get<double>(field.name).value_or(0.0);
            memcpy(base + field.offset, &val, sizeof(double));
        } else if (field.type == components_module_t::SPECTRE_DYNAMIC_TYPE_STRING) {
            std::string val = input.get<std::string>(field.name).value_or("");
            char** ptr = reinterpret_cast<char**>(base + field.offset);
            if (*ptr) free(*ptr);
            *ptr = strdup(val.c_str());
        }
    }
    
    ecs_set_id(world, target_entity, comp_id.id(), comp_size, temp_buffer);
    free(temp_buffer);
"""

# I will replace the whole function again to be safe!
