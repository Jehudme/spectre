import re

path = 'spectre/source/modules/components/components_module.cpp'
with open(path, 'r') as f:
    content = f.read()

content = content.replace(
    '} else if (field.type == components_module_t::SPECTRE_DYNAMIC_TYPE_STRING) {',
    '} else if (field.type == components_module_t::SPECTRE_DYNAMIC_TYPE_BOOL) {\n            bool val = input.get<bool>(field.name).value_or(false);\n            memcpy(base + field.offset, &val, sizeof(bool));\n        } else if (field.type == components_module_t::SPECTRE_DYNAMIC_TYPE_STRING) {'
)

content = content.replace(
    '} else if (field.type == components_module_t::SPECTRE_DYNAMIC_TYPE_STRING) {',
    '} else if (field.type == components_module_t::SPECTRE_DYNAMIC_TYPE_BOOL) {\n            bool val;\n            memcpy(&val, base + field.offset, sizeof(bool));\n            props.set<bool>(field.name, val);\n        } else if (field.type == components_module_t::SPECTRE_DYNAMIC_TYPE_STRING) {'
)

with open(path, 'w') as f:
    f.write(content)
