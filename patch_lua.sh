LUA_FILES=(
    "demo/resources/asteroid/resources/scripts/externals/spectre.lua"
    "demo/resources/editor/resources/scripts/externals/spectre.lua"
)

for file in "${LUA_FILES[@]}"; do
    sed -i '/bool spectre_components_is_component(ecs_world_t\* world, ecs_entity_t entity);/a \
        ecs_entity_t* spectre_components_list_components(ecs_world_t* world, size_t* count);\n' "$file"

    sed -i '/spectre.components.is_component = function(world, entity)/i \
    list_components = function(world)\n        local count = ffi.new("size_t[1]")\n        local entities = spectre.api.spectre_components_list_components(world, count)\n        local result = {}\n        for i = 0, tonumber(count[0]) - 1 do\n            table.insert(result, tonumber(entities[i]))\n        end\n        return result\n    end,\n' "$file"

    sed -i '/bool spectre_scenes_is_scene(ecs_world_t\* world, ecs_entity_t entity);/a \
        ecs_entity_t* spectre_scenes_list_states(ecs_world_t* world, size_t* count);\n' "$file"

    sed -i '/spectre.scenes.is_scene = function(world, entity)/i \
    list_states = function(world)\n        local count = ffi.new("size_t[1]")\n        local entities = spectre.api.spectre_scenes_list_states(world, count)\n        local result = {}\n        for i = 0, tonumber(count[0]) - 1 do\n            table.insert(result, tonumber(entities[i]))\n        end\n        return result\n    end,\n' "$file"

    sed -i '/bool spectre_prefabs_is_prefab(ecs_world_t\* world, ecs_entity_t entity);/a \
        ecs_entity_t* spectre_prefabs_list_prefabs(ecs_world_t* world, size_t* count);\n' "$file"

    sed -i '/spectre.prefabs.is_prefab = function(world, entity)/i \
    list_prefabs = function(world)\n        local count = ffi.new("size_t[1]")\n        local entities = spectre.api.spectre_prefabs_list_prefabs(world, count)\n        local result = {}\n        for i = 0, tonumber(count[0]) - 1 do\n            table.insert(result, tonumber(entities[i]))\n        end\n        return result\n    end,\n' "$file"

    sed -i '/ecs_entity_t spectre_resources_find_resource/i \
        ecs_entity_t* spectre_resources_list_resources(ecs_world_t* world, size_t* count);\n        ecs_entity_t* spectre_resources_list_resource_loaders(ecs_world_t* world, size_t* count);\n' "$file"

    sed -i '/spectre.resources.find_resource = function(world, name)/i \
    list_resources = function(world)\n        local count = ffi.new("size_t[1]")\n        local entities = spectre.api.spectre_resources_list_resources(world, count)\n        local result = {}\n        for i = 0, tonumber(count[0]) - 1 do\n            table.insert(result, tonumber(entities[i]))\n        end\n        return result\n    end,\n    list_resource_loaders = function(world)\n        local count = ffi.new("size_t[1]")\n        local entities = spectre.api.spectre_resources_list_resource_loaders(world, count)\n        local result = {}\n        for i = 0, tonumber(count[0]) - 1 do\n            table.insert(result, tonumber(entities[i]))\n        end\n        return result\n    end,\n' "$file"

    sed -i '/ecs_entity_t spectre_scripts_find_script(ecs_world_t\* world, const char\* name);/i \
        ecs_entity_t* spectre_scripts_list_scripts(ecs_world_t* world, size_t* count);\n' "$file"

    sed -i '/spectre.scripts.find_script = function(world, name)/i \
    list_scripts = function(world)\n        local count = ffi.new("size_t[1]")\n        local entities = spectre.api.spectre_scripts_list_scripts(world, count)\n        local result = {}\n        for i = 0, tonumber(count[0]) - 1 do\n            table.insert(result, tonumber(entities[i]))\n        end\n        return result\n    end,\n' "$file"

    sed -i '/ecs_entity_t spectre_window_get_window_entity(ecs_world_t\* world);/a \
        void spectre_window_import_configuration(ecs_world_t* world, const char* file_path);\n        void spectre_window_export_configuration(ecs_world_t* world, const char* file_path);\n' "$file"

    sed -i '/spectre.window.get_window_entity = function(world)/i \
    import_configuration = function(world, path)\n        spectre.api.spectre_window_import_configuration(world, path)\n    end,\n    export_configuration = function(world, path)\n        spectre.api.spectre_window_export_configuration(world, path)\n    end,\n' "$file"

    if grep -q "spectre_renderer_import_configuration" "$file"; then
        echo "Renderer config already declared in $file"
    else
        sed -i '/ecs_entity_t spectre_renderer_get_renderer_entity(ecs_world_t\* world);/a \
        void spectre_renderer_import_configuration(ecs_world_t* world, const char* path);\n        void spectre_renderer_export_configuration(ecs_world_t* world, const char* path);\n' "$file"
        
        sed -i '/spectre.renderer.get_renderer_entity = function(world)/i \
    import_configuration = function(world, path)\n        spectre.api.spectre_renderer_import_configuration(world, path)\n    end,\n    export_configuration = function(world, path)\n        spectre.api.spectre_renderer_export_configuration(world, path)\n    end,\n' "$file"
    fi
done
