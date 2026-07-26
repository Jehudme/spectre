import re

path = 'spectre/source/modules/components/components_module.cpp'
with open(path, 'r') as f:
    content = f.read()

new_content = content.replace('if (!comp_id.is_valid()) return {0};', 'if (!comp_id.is_valid()) { sandbox::modules::logs::error(w, "Serialize: comp_id is invalid"); return {0}; }')
new_content = new_content.replace('if (!comp_data) return {0};', 'if (!comp_data) { sandbox::modules::logs::error(w, "Serialize: comp_data is null"); return {0}; }')
new_content = new_content.replace('if (!schema_entity.is_valid()) return {0};', 'if (!schema_entity.is_valid()) { sandbox::modules::logs::error(w, "Serialize: schema_entity is invalid"); return {0}; }')
new_content = new_content.replace('if (!schema) return {0};', 'if (!schema) { sandbox::modules::logs::error(w, "Serialize: schema is null"); return {0}; }')

with open(path, 'w') as f:
    f.write(new_content)
