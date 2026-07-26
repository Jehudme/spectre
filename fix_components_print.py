import re

path = 'spectre/source/modules/components/components_module.cpp'
with open(path, 'r') as f:
    content = f.read()

deserialize = """
    auto* header = static_cast<spectre_dynamic_component_header_t*>(comp_data);
    header->schema_entity = schema_entity.id();
    sandbox::modules::logs::info(w, "Deserialize: target={}, comp_id={}, schema_entity={}", target_entity, comp_id.id(), schema_entity.id());
"""
content = content.replace('auto* header = static_cast<spectre_dynamic_component_header_t*>(comp_data);\n    header->schema_entity = schema_entity.id();', deserialize)

serialize = """
    const auto* header = static_cast<const spectre_dynamic_component_header_t*>(comp_data);
    sandbox::modules::logs::info(w, "Serialize: target={}, comp_id={}, header->schema_entity={}", target_entity, comp_id.id(), header->schema_entity);
    flecs::entity schema_entity = w.entity(header->schema_entity);
"""
content = content.replace('const auto* header = static_cast<const spectre_dynamic_component_header_t*>(comp_data);\n    flecs::entity schema_entity = w.entity(header->schema_entity);', serialize)

with open(path, 'w') as f:
    f.write(content)
