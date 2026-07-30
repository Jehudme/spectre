import os
import glob
import re

base_dir = "/home/jehud/CLionProjects/spectre"
module_files = glob.glob(f"{base_dir}/spectre/source/modules/**/*_module.cpp", recursive=True)

schemas = {
    "Transform2D": '{"members":[{"name":"position_x","type":"float"},{"name":"position_y","type":"float"},{"name":"position_z","type":"float"},{"name":"scale_x","type":"float"},{"name":"scale_y","type":"float"},{"name":"origin_x","type":"float"},{"name":"origin_y","type":"float"},{"name":"rotation","type":"float"}]}',
    "RectangleRenderable": '{"members":[{"name":"width","type":"float"},{"name":"height","type":"float"},{"name":"outline_thickness","type":"float"}]}',
    "CircleRenderable": '{"members":[{"name":"radius","type":"float"},{"name":"outline_thickness","type":"float"}]}',
    "PolygoneRenderable": '{"members":[{"name":"radius","type":"float"},{"name":"point_count","type":"int"},{"name":"outline_thickness","type":"float"}]}',
    "CustomPolygoneRenderable": '{"members":[{"name":"vertex_count","type":"int"},{"name":"outline_thickness","type":"float"}]}',
    "LigneRenderable": '{"members":[{"name":"position_x1","type":"double"},{"name":"position_y1","type":"double"},{"name":"position_x2","type":"double"},{"name":"position_y2","type":"double"},{"name":"thickness","type":"float"}]}',
    "TextureRenderable": '{"members":[{"name":"width","type":"float"},{"name":"height","type":"float"},{"name":"source_x","type":"float"},{"name":"source_y","type":"float"},{"name":"source_width","type":"float"},{"name":"source_height","type":"float"},{"name":"flip_x","type":"bool"},{"name":"flip_y","type":"bool"}]}',
    "TextRenderable": '{"members":[{"name":"content","type":"string"},{"name":"font_size","type":"float"},{"name":"spacing","type":"float"},{"name":"bold","type":"bool"},{"name":"italic","type":"bool"}]}',
    "Material": '{"members":[{"name":"shader_resource_name","type":"string"}]}',
    "StateUseSceneRelation": '{"members":[{"name":"layer_index","type":"int"}]}',
}

def get_schema(name):
    return schemas.get(name, '{"members":[]}')

for filepath in module_files:
    with open(filepath, 'r') as f:
        content = f.read()

    new_content = ""
    lines = content.split('\n')
    changed = False

    for line in lines:
        if 'spectre::modules::components::register_component(m_world,' in line:
            # find args
            m = re.search(r'register_component\(m_world,\s*"([^"]+)",\s*([^,)]+)(?:,\s*([^,)]+))?\);', line)
            if m:
                name = m.group(1)
                reg_fn = m.group(2)
                ser = m.group(3) if m.group(3) else "spectre_serializer_component{}"
                
                schema_json = get_schema(name)
                indent = line[:line.find('spectre::')]
                replacement = f"{indent}{{\n{indent}    sandbox::properties schema;\n{indent}    schema.load(R\"({schema_json})\", sandbox::properties::Format::JSON);\n{indent}    spectre::modules::components::register_component(m_world, \"{name}\", {reg_fn}, {ser}, std::move(schema));\n{indent}}}"
                new_content += replacement + "\n"
                changed = True
                continue
            
            # check if it already has schema argument (if someone added it manually)
            if 'schema' in line:
                pass
        new_content += line + "\n"

    # remove trailing newline
    new_content = new_content[:-1]

    if changed:
        with open(filepath, 'w') as f:
            f.write(new_content)
        print(f"Patched {filepath}")
