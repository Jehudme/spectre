import re
import sys

def patch_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # C definitions additions
    content = re.sub(
        r'(bool spectre_components_is_component\(ecs_world_t\* world, ecs_entity_t entity\);)',
        r'\1\n    ecs_entity_t* spectre_components_list_components(ecs_world_t* world, size_t* count);',
        content, count=1
    )
    
    content = re.sub(
        r'(bool spectre_scenes_is_scene\(ecs_world_t\* world, ecs_entity_t entity\);)',
        r'\1\n    ecs_entity_t* spectre_scenes_list_states(ecs_world_t* world, size_t* count);',
        content, count=1
    )
    
    content = re.sub(
        r'(bool spectre_prefabs_is_prefab\(ecs_world_t\* world, ecs_entity_t entity\);)',
        r'\1\n    ecs_entity_t* spectre_prefabs_list_prefabs(ecs_world_t* world, size_t* count);',
        content, count=1
    )
    
    content = re.sub(
        r'(bool spectre_resources_is_resource\(ecs_world_t\* world, ecs_entity_t entity\);)',
        r'\1\n    ecs_entity_t* spectre_resources_list_resources(ecs_world_t* world, size_t* count);\n    ecs_entity_t* spectre_resources_list_resource_loaders(ecs_world_t* world, size_t* count);',
        content, count=1
    )
    
    content = re.sub(
        r'(bool spectre_scripts_is_script\(ecs_world_t\* world, ecs_entity_t entity\);)',
        r'\1\n    ecs_entity_t* spectre_scripts_list_scripts(ecs_world_t* world, size_t* count);',
        content, count=1
    )
    
    content = re.sub(
        r'(ecs_entity_t spectre_window_get_window_entity\(ecs_world_t\* world\);)',
        r'\1\n    void spectre_window_import_configuration(ecs_world_t* world, const char* path);\n    void spectre_window_export_configuration(ecs_world_t* world, const char* path);',
        content, count=1
    )
    
    if "spectre_renderer_import_configuration" not in content:
        content = re.sub(
            r'(ecs_entity_t spectre_renderer_get_renderer_entity\(ecs_world_t\* world\);)',
            r'\1\n    void spectre_renderer_import_configuration(ecs_world_t* world, const char* path);\n    void spectre_renderer_export_configuration(ecs_world_t* world, const char* path);',
            content, count=1
        )
    
    # Lua functions additions
    lua_components = """    list_components = function(world)
        local count = ffi.new("size_t[1]")
        local entities = spectre.api.spectre_components_list_components((type(world) == "table" and world.ptr) and world.ptr or world, count)
        local result = {}
        for i = 0, tonumber(count[0]) - 1 do
            table.insert(result, tonumber(entities[i]))
        end
        return result
    end,
"""
    content = re.sub(r'(spectre\.components = \{\n)', r'\1' + lua_components, content, count=1)
    
    lua_scenes = """    list_states = function(world)
        local count = ffi.new("size_t[1]")
        local entities = spectre.api.spectre_scenes_list_states((type(world) == "table" and world.ptr) and world.ptr or world, count)
        local result = {}
        for i = 0, tonumber(count[0]) - 1 do
            table.insert(result, tonumber(entities[i]))
        end
        return result
    end,
"""
    content = re.sub(r'(spectre\.scenes = \{\n)', r'\1' + lua_scenes, content, count=1)
    
    lua_prefabs = """    list_prefabs = function(world)
        local count = ffi.new("size_t[1]")
        local entities = spectre.api.spectre_prefabs_list_prefabs((type(world) == "table" and world.ptr) and world.ptr or world, count)
        local result = {}
        for i = 0, tonumber(count[0]) - 1 do
            table.insert(result, tonumber(entities[i]))
        end
        return result
    end,
"""
    content = re.sub(r'(spectre\.prefabs = \{\n)', r'\1' + lua_prefabs, content, count=1)
    
    lua_resources = """    list_resources = function(world)
        local count = ffi.new("size_t[1]")
        local entities = spectre.api.spectre_resources_list_resources((type(world) == "table" and world.ptr) and world.ptr or world, count)
        local result = {}
        for i = 0, tonumber(count[0]) - 1 do
            table.insert(result, tonumber(entities[i]))
        end
        return result
    end,
    list_resource_loaders = function(world)
        local count = ffi.new("size_t[1]")
        local entities = spectre.api.spectre_resources_list_resource_loaders((type(world) == "table" and world.ptr) and world.ptr or world, count)
        local result = {}
        for i = 0, tonumber(count[0]) - 1 do
            table.insert(result, tonumber(entities[i]))
        end
        return result
    end,
"""
    content = re.sub(r'(spectre\.resources = \{\n)', r'\1' + lua_resources, content, count=1)

    lua_scripts = """    list_scripts = function(world)
        local count = ffi.new("size_t[1]")
        local entities = spectre.api.spectre_scripts_list_scripts((type(world) == "table" and world.ptr) and world.ptr or world, count)
        local result = {}
        for i = 0, tonumber(count[0]) - 1 do
            table.insert(result, tonumber(entities[i]))
        end
        return result
    end,
"""
    content = re.sub(r'(spectre\.scripts = \{\n)', r'\1' + lua_scripts, content, count=1)

    lua_window = """    import_configuration = function(world, path)
        spectre.api.spectre_window_import_configuration((type(world) == "table" and world.ptr) and world.ptr or world, path)
    end,
    export_configuration = function(world, path)
        spectre.api.spectre_window_export_configuration((type(world) == "table" and world.ptr) and world.ptr or world, path)
    end,
"""
    content = re.sub(r'(spectre\.window = \{\n)', r'\1' + lua_window, content, count=1)

    if "import_configuration =" not in content[content.find("spectre.renderer = {"):]:
        lua_renderer = """    import_configuration = function(world, path)
        spectre.api.spectre_renderer_import_configuration((type(world) == "table" and world.ptr) and world.ptr or world, path)
    end,
    export_configuration = function(world, path)
        spectre.api.spectre_renderer_export_configuration((type(world) == "table" and world.ptr) and world.ptr or world, path)
    end,
"""
        content = re.sub(r'(spectre\.renderer = \{\n)', r'\1' + lua_renderer, content, count=1)
        
    with open(filepath, 'w') as f:
        f.write(content)

patch_file("demo/resources/asteroid/resources/scripts/externals/spectre.lua")
patch_file("demo/resources/editor/resources/scripts/externals/spectre.lua")
