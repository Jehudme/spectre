import re

path = 'spectre/source/modules/components/components_module.cpp'
with open(path, 'r') as f:
    content = f.read()

replacement = """
    // Add component to target entity
    if (!ecs_is_valid(world, target_entity)) { sandbox::modules::logs::error(w, "target_entity is invalid"); return; }
    
    bool was_deferred = ecs_is_deferred(world);
    if (was_deferred) ecs_defer_suspend(world);
    
    ecs_add_id(world, target_entity, comp_id.id());
    void* comp_data = ecs_get_mut_id(world, target_entity, comp_id.id());
    
    if (!comp_data) { 
        if (was_deferred) ecs_defer_resume(world);
        sandbox::modules::logs::error(w, "comp_data is null"); 
        return; 
    }
    
    // Save a copy of the schema entity in the component header so we can serialize later!
    auto* header = static_cast<spectre_dynamic_component_header_t*>(comp_data);
    header->schema_entity = schema_entity.id();
"""

old_str = """
    // Add component to target entity
    if (!ecs_is_valid(world, target_entity)) { sandbox::modules::logs::error(w, "target_entity is invalid"); return; }
ecs_add_id(world, target_entity, comp_id.id());
void* comp_data = ecs_get_mut_id(world, target_entity, comp_id.id());
    if (!comp_data) { sandbox::modules::logs::error(w, "comp_data is null"); return; }
    
    // Save a copy of the schema entity in the component header so we can serialize later!
    
    auto* header = static_cast<spectre_dynamic_component_header_t*>(comp_data);
    header->schema_entity = schema_entity.id();
"""

content = content.replace(old_str.strip(), replacement.strip())

# We should also resume at the end!
# Wait! I need to replace the ENTIRE rest of the function!
