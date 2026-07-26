import re

path = 'spectre/source/modules/components/components_module.cpp'
with open(path, 'r') as f:
    content = f.read()

new_content = content.replace('if (!comp_data) return;', 'if (!comp_data) { sandbox::modules::logs::error(w, "comp_data is null"); return; }')
new_content = new_content.replace('if (!schema) return;', 'if (!schema) { sandbox::modules::logs::error(w, "schema is null"); return; }')
new_content = new_content.replace('if (!schema_entity.is_valid()) return;', 'if (!schema_entity.is_valid()) { sandbox::modules::logs::error(w, "schema_entity is invalid"); return; }')
new_content = new_content.replace('if (!comp_id.is_valid()) return;', 'if (!comp_id.is_valid()) { sandbox::modules::logs::error(w, "comp_id is invalid"); return; }')

with open(path, 'w') as f:
    f.write(new_content)
