#include "scenes_module.h"
#include "spectre/services/scenes_service.h"
#include "spectre/sdk/scripts.hpp"
#include "spectre/sdk/serializer.hpp"
#include "spectre/sdk/components.hpp"
#include "sandbox/sdk/logs.hpp"
#include <iostream>

#include "spectre/sdk/scenes.hpp"

namespace spectre::modules {

    static sandbox_properties_handle_t serialize_state_cb(ecs_world_t* world, ecs_entity_t entity_id) {
        return spectre::modules::scenes::serialize_state(flecs::world(world), entity_id);
    }

    static ecs_entity_t deserialize_state_cb(ecs_world_t* world, sandbox_properties_handle_t properties_handle) {
        return spectre::modules::scenes::deserialize_state(flecs::world(world), properties_handle);
    }

    static sandbox_properties_handle_t serialize_scene_cb(ecs_world_t* world, ecs_entity_t entity_id) {
        return spectre::modules::scenes::serialize_scene(flecs::world(world), entity_id);
    }

    static ecs_entity_t deserialize_scene_cb(ecs_world_t* world, sandbox_properties_handle_t properties_handle) {
        return spectre::modules::scenes::deserialize_scene(flecs::world(world), properties_handle);
    }

    // Component Registration Callbacks
    // TODO: Also register the member
    static ecs_entity_t register_scene_component(ecs_world_t* world) { return flecs::world(world).component<spectre_scene_t>().id(); }
    static ecs_entity_t register_state_component(ecs_world_t* world) { return flecs::world(world).component<spectre_state_t>().id(); }
    static ecs_entity_t register_state_use_scene_relation_component(ecs_world_t* world) { return flecs::world(world).component<spectre_state_use_scene_relation_t>().id(); }

    static ecs_entity_t register_state_context_component(ecs_world_t* world) { return flecs::world(world).component<spectre_state_context_t>().id(); }
    static ecs_entity_t register_scene_context_component(ecs_world_t* world) { return flecs::world(world).component<spectre_scene_context_t>().id(); }
    static ecs_entity_t register_disable_rendering_component(ecs_world_t* world) { return flecs::world(world).component<spectre_disable_rendering_t>().id(); }

    SANDBOX_DECLARE_MODULE(scenes_module_t, {
        .name = "scenes",
        .description = "Scenes Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_scenes_service_t_info,
        .requirements = nullptr,
        .requirement_count = 0
    })

    scenes_module_t::scenes_module_t(flecs::world& world) : m_world(world) {
        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Scenes Module] Initializing...");

        m_states_root = m_world.entity("::states");
        m_scenes_root = m_world.entity("::scenes");

        // Register components
        // TODO: Make the serializers for these components
        spectre_serializer_component empty_serializer = {nullptr, nullptr};
        spectre::modules::components::register_component(m_world, "spectre_scene_t", register_scene_component, empty_serializer);
        spectre::modules::components::register_component(m_world, "spectre_state_t", register_state_component, empty_serializer);
        spectre::modules::components::register_component(m_world, "spectre_state_use_scene_relation_t", register_state_use_scene_relation_component, empty_serializer);
        spectre::modules::components::register_component(m_world, "spectre_state_context_t", register_state_context_component, empty_serializer);
        spectre::modules::components::register_component(m_world, "spectre_scene_context_t", register_scene_context_component, empty_serializer);
        spectre::modules::components::register_component(m_world, "spectre_disable_rendering_t", register_disable_rendering_component, empty_serializer);

        spectre_serializer_component state_serializer = {};
        state_serializer.serialize = serialize_state_cb;
        state_serializer.deserialize = deserialize_state_cb;
        spectre::modules::serializer::register_serializer(m_world, "state", &state_serializer);

        spectre_serializer_component scene_serializer = {};
        scene_serializer.serialize = serialize_scene_cb;
        scene_serializer.deserialize = deserialize_scene_cb;
        spectre::modules::serializer::register_serializer(m_world, "scene", &scene_serializer);
        
        m_scene_serializer = m_world.entity(spectre::modules::serializer::find_serializer(m_world, "scene"));
        m_state_serializer = m_world.entity(spectre::modules::serializer::find_serializer(m_world, "state"));
        m_script_args_serializer = m_world.entity(spectre::modules::serializer::find_serializer(m_world, "scripts"));
        m_entity_serializer = m_world.entity(spectre::modules::serializer::find_serializer(m_world, "entity"));

        if (!m_script_args_serializer.is_valid()) {
            sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "[Scenes Module] Could not find 'scripts' serializer. Serialization might be incomplete.");
        }
        if (!m_entity_serializer.is_valid()) {
            sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "[Scenes Module] Could not find 'entity' serializer. Serialization might be incomplete.");
        }

        struct spectre_scenes_update_marker_t {};
        m_world.component<spectre_scenes_update_marker_t>();
        m_world.entity("Scenes Update Marker").add<spectre_scenes_update_marker_t>();

        m_world.system<spectre_scenes_update_marker_t>("Scenes OnUpdate")
            .kind(flecs::OnUpdate)
            .each([this](flecs::entity entity, spectre_scenes_update_marker_t) {
                flecs::entity current_state = find_current_state();
                if (current_state.is_valid() && !current_state.has<spectre_disable_rendering_t>()) {
                    ::spectre::modules::scripts::execute_on_update(current_state);
                    
                    current_state.children([&](flecs::entity scene_entity) {
                        if (scene_entity.has<spectre_state_use_scene_relation_t>()) {
                            ::spectre::modules::scripts::execute_on_update(scene_entity);
                            
                            scene_entity.children([&](flecs::entity child_entity) {
                                if (!child_entity.has<spectre_scene_t>() && !child_entity.has<spectre_state_t>()) {
                                    ::spectre::modules::scripts::execute_on_update(child_entity);
                                }
                            });
                        }
                    });
                }
            });

        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Scenes Module] Initialized successfully.");
    }
    
    scenes_module_t::~scenes_module_t() = default;

    sandbox::properties scenes_module_t::serialize_state(flecs::entity state_entity) {
        sandbox::properties properties;
        if (!state_entity.is_valid()) return properties;
        
        properties.set<std::string>("name", state_entity.name().c_str());
        
        // Serialize scripts
        flecs::entity scripts_child = state_entity.lookup("scripts");
        if (scripts_child.is_valid() && m_script_args_serializer != 0) {
            sandbox_properties_handle_t properties_handle = spectre::modules::serializer::serialize_entity(m_world, m_script_args_serializer, scripts_child.id());
            sandbox::properties scripts_node(properties_handle, true);
            if (scripts_node.is_valid()) {
                properties.merge("scripts", scripts_node);
            }
        }
        
        // Serialize scenes
        sandbox::properties scenes_array;
        int layer_index = 0;
        state_entity.children([&](flecs::entity child_entity) {
            if (is_scene(child_entity)) {
                scenes_array.set<std::string>(std::to_string(layer_index++), child_entity.name().c_str());
            }
        });
        
        if (layer_index > 0) {
            properties.merge("scenes", scenes_array);
        }

        return properties;
    }

    flecs::entity scenes_module_t::deserialize_state(sandbox::properties properties) {
        if (!properties.is_valid()) return flecs::entity::null();
        
        flecs::entity state_entity = m_world.entity();
        std::string state_name;
        if (properties.get<std::string>("name", state_name)) {
            flecs::entity existing_state = find_state(state_name);
            if (existing_state.is_valid()) {
                state_entity = existing_state;
            } else {
                state_entity.set_name(state_name.c_str());
            }
        }
        
        state_entity.add<spectre_state_t>();
        
        if (properties.has("scripts") && m_script_args_serializer != 0) {
            sandbox::properties scripts_node = properties.sub("scripts");
            ecs_entity_t deserialized_id = spectre::modules::serializer::deserialize_entity(m_world, m_script_args_serializer, scripts_node.get_raw());
            flecs::entity temp_scripts_entity = m_world.entity(deserialized_id);
            if (temp_scripts_entity.is_valid()) {
                temp_scripts_entity.child_of(state_entity);
                temp_scripts_entity.set_name("scripts");
                temp_scripts_entity.add(flecs::Prefab);
            }
        }

        if (properties.has("scenes")) {
            std::vector<std::string> scene_keys = properties.keys("scenes");
            sandbox::properties scenes_node = properties.sub("scenes");
            
            for (size_t index = 0; index < scene_keys.size(); ++index) {
                std::string scene_name;
                if (scenes_node.get<std::string>(std::to_string(index), scene_name)) {
                    flecs::entity scene_prefab = find_scene(scene_name);
                    if (scene_prefab.is_valid()) {
                        flecs::entity scene_instance = m_world.prefab().is_a(scene_prefab).child_of(state_entity);
                        scene_instance.set<spectre_state_use_scene_relation_t>({static_cast<int>(index)});
                    } else {
                        sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "[Scenes Module] Referenced scene '{}' not found when deserializing state.", scene_name);
                    }
                }
            }
        }

        return state_entity;
    }

    sandbox::properties scenes_module_t::serialize_scene(flecs::entity scene_entity) {
        sandbox::properties properties;
        if (!scene_entity.is_valid()) return properties;
        
        properties.set<std::string>("name", scene_entity.name().c_str());
        
        // Serialize scripts
        flecs::entity scripts_child = scene_entity.lookup("scripts");
        if (scripts_child.is_valid() && m_script_args_serializer != 0) {
            sandbox_properties_handle_t properties_handle = spectre::modules::serializer::serialize_entity(m_world, m_script_args_serializer, scripts_child.id());
            sandbox::properties scripts_node(properties_handle, true);
            if (scripts_node.is_valid()) {
                properties.merge("scripts", scripts_node);
            }
        }

        // Serialize hierarchies
        if (m_entity_serializer != 0) {
            sandbox::properties hierarchies_array;
            int child_index = 0;
            scene_entity.children([&](flecs::entity child_entity) {
                if (child_entity.name() != "scripts") {
                    sandbox_properties_handle_t properties_handle = spectre::modules::serializer::serialize_entity(m_world, m_entity_serializer, child_entity.id());
                    sandbox::properties child_properties(properties_handle, true);
                    if (child_properties.is_valid()) {
                        hierarchies_array.merge(std::to_string(child_index++), child_properties);
                    }
                }
            });
            if (child_index > 0) {
                properties.merge("hierarchies", hierarchies_array);
            }
        }

        return properties;
    }

    flecs::entity scenes_module_t::deserialize_scene(sandbox::properties properties) {
        if (!properties.is_valid()) return flecs::entity::null();
        
        flecs::entity scene_entity = m_world.entity();
        std::string scene_name;
        if (properties.get<std::string>("name", scene_name)) {
            flecs::entity existing_scene = find_scene(scene_name);
            if (existing_scene.is_valid()) {
                scene_entity = existing_scene;
            } else {
                scene_entity.set_name(scene_name.c_str());
            }
        }
        
        scene_entity.add<spectre_scene_t>();

        if (properties.has("scripts") && m_script_args_serializer != 0) {
            sandbox::properties scripts_node = properties.sub("scripts");
            ecs_entity_t deserialized_id = spectre::modules::serializer::deserialize_entity(m_world, m_script_args_serializer, scripts_node.get_raw());
            flecs::entity temp_scripts_entity = m_world.entity(deserialized_id);
            if (temp_scripts_entity.is_valid()) {
                temp_scripts_entity.child_of(scene_entity);
                temp_scripts_entity.set_name("scripts");
                temp_scripts_entity.add(flecs::Prefab);
            }
        }

        if (properties.has("hierarchies") && m_entity_serializer != 0) {
            std::vector<std::string> keys = properties.keys("hierarchies");
            sandbox::properties hierarchies_node = properties.sub("hierarchies");
            
            for (const auto& key : keys) {
                sandbox::properties child_properties = hierarchies_node.sub(key);
                if (child_properties.is_valid()) {
                    ecs_entity_t deserialized_id = spectre::modules::serializer::deserialize_entity(m_world, m_entity_serializer, child_properties.get_raw());
                    flecs::entity child_entity = m_world.entity(deserialized_id);
                    if (child_entity.is_valid()) {
                        child_entity.child_of(scene_entity);
                        child_entity.add(flecs::Prefab);
                    }
                }
            }
        }
        
        return scene_entity;
    }

    void scenes_module_t::register_state(sandbox::properties properties) {
        if (!properties.is_valid()) return;
        std::string state_name;
        if (!properties.get<std::string>("name", state_name) || state_name.empty()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scenes Module] Cannot register state with empty name.");
            return;
        }

        flecs::entity state_prefab = find_state(state_name);
        if (!state_prefab.is_valid()) {
            state_prefab = m_world.prefab(state_name.c_str())
                                  .child_of(m_states_root)
                                  .add<spectre_state_t>();
        }
        properties.set<std::string>("name", state_name);
        
        // Clean up old scripts/scenes if we are re-registering
        state_prefab.children([&](flecs::entity child_entity) {
            child_entity.destruct();
        });
        
        deserialize_state(std::move(properties));
        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Scenes Module] Registered state '{}'.", state_name);
    }

    void scenes_module_t::register_scene(sandbox::properties properties) {
        if (!properties.is_valid()) return;
        std::string scene_name;
        if (!properties.get<std::string>("name", scene_name) || scene_name.empty()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scenes Module] Cannot register scene with empty name.");
            return;
        }

        flecs::entity scene_prefab = find_scene(scene_name);
        if (!scene_prefab.is_valid()) {
            scene_prefab = m_world.prefab(scene_name.c_str())
                                  .child_of(m_scenes_root)
                                  .add<spectre_scene_t>();
        }
        properties.set<std::string>("name", scene_name);
        
        // Clean up old scripts/hierarchies if we are re-registering
        scene_prefab.children([&](flecs::entity child_entity) {
            child_entity.destruct();
        });
        
        deserialize_scene(std::move(properties));
        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Scenes Module] Registered scene '{}'.", scene_name);
    }

    flecs::entity scenes_module_t::find_state(std::string_view state_name) {
        if (state_name.empty()) return flecs::entity::null();
        return m_states_root.lookup(std::string(state_name).c_str());
    }

    flecs::entity scenes_module_t::find_scene(std::string_view scene_name) {
        if (scene_name.empty()) return flecs::entity::null();
        return m_scenes_root.lookup(std::string(scene_name).c_str());
    }

    bool scenes_module_t::has_state(std::string_view state_name) const {
        if (state_name.empty()) return false;
        flecs::entity state_entity = m_states_root.lookup(std::string(state_name).c_str());
        return is_state(state_entity);
    }

    bool scenes_module_t::has_scene(std::string_view scene_name) const {
        if (scene_name.empty()) return false;
        flecs::entity scene_entity = m_scenes_root.lookup(std::string(scene_name).c_str());
        return is_scene(scene_entity);
    }

    bool scenes_module_t::is_state(flecs::entity entity_to_check) const {
        return entity_to_check.is_valid() && entity_to_check.has<spectre_state_t>();
    }

    bool scenes_module_t::is_scene(flecs::entity entity_to_check) const {
        return entity_to_check.is_valid() && entity_to_check.has<spectre_scene_t>();
    }

    flecs::entity scenes_module_t::find_current_state() {
        if (!m_state_stack.empty()) {
            return m_state_stack.top();
        }
        return flecs::entity::null();
    }

    flecs::query<> scenes_module_t::find_current_scenes() {
        flecs::entity current_state = find_current_state();
        if (current_state.is_valid()) {
            return m_world.query_builder<>(m_world.entity()).with(flecs::ChildOf, current_state).with<spectre_scene_t>().build();
        }
        return m_world.query_builder<>(m_world.entity()).with<spectre_scene_t>().with(flecs::ChildOf, (flecs::entity_t)0).build();
    }

    void scenes_module_t::push_state(flecs::entity state_prefab) {
        if (!state_prefab.is_valid()) return;

        // Run on_exit on current state
        if (!m_state_stack.empty()) {
            flecs::entity current_state = m_state_stack.top();
            if (current_state.is_valid()) {
                execute_recursive(current_state, [](ecs_world_t* world, ecs_entity_t entity_id, void* payload) {
                    flecs::world flecs_world(world);
                    ::spectre::modules::scripts::execute_on_exit(flecs_world.entity(entity_id));
                }, nullptr);
                current_state.add<spectre_disable_rendering_t>();
            }
        }

        // Create new state instance
        flecs::entity state_instance = m_world.entity().is_a(state_prefab);
        state_instance.set<spectre_state_context_t>({state_instance});
        m_state_stack.push(state_instance);

        // Run on_enter recursively on the new current_state
        execute_recursive(state_instance, [](ecs_world_t* world, ecs_entity_t entity_id, void* payload) {
            flecs::world flecs_world(world);
            ::spectre::modules::scripts::execute_on_enter(flecs_world.entity(entity_id));
        }, nullptr);
        
        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Scenes Module] Pushed state '{}'.", state_prefab.name().c_str());
    }

    void scenes_module_t::pop_state() {
        if (m_state_stack.empty()) {
            sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "[Scenes Module] Attempted to pop state from an empty stack.");
            return;
        }
        
        flecs::entity current_state = m_state_stack.top();
        if (current_state.is_valid()) {
            sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Scenes Module] Popping state '{}'.", current_state.name().c_str() ? current_state.name().c_str() : "unknown");
            execute_recursive(current_state, [](ecs_world_t* world, ecs_entity_t entity_id, void* payload) {
                flecs::world flecs_world(world);
                flecs::entity child_entity = flecs_world.entity(entity_id);
                ::spectre::modules::scripts::execute_on_exit(child_entity);
                ::spectre::modules::scripts::execute_on_destroy(child_entity);
            }, nullptr);
            current_state.destruct();
        }
        
        m_state_stack.pop();
        
        if (!m_state_stack.empty()) {
            flecs::entity top_state = m_state_stack.top();
            if (top_state.is_valid()) {
                top_state.remove<spectre_disable_rendering_t>();
                execute_recursive(top_state, [](ecs_world_t* world, ecs_entity_t entity_id, void* payload) {
                    flecs::world flecs_world(world);
                    ::spectre::modules::scripts::execute_on_enter(flecs_world.entity(entity_id));
                }, nullptr);
            }
        }
    }

    void scenes_module_t::execute_recursive(flecs::entity parent_entity, spectre_recursive_callback_t callback, void* payload) {
        if (!parent_entity.is_valid() || !callback) return;
        callback(m_world.c_ptr(), parent_entity.id(), payload);
        parent_entity.children([&](flecs::entity child_entity) {
            if (child_entity.name() != "scripts") { // skip the scripts child container itself
                execute_recursive(child_entity, callback, payload);
            }
        });
    }

    flecs::entity scenes_module_t::create_state(std::string_view state_name) {
        flecs::entity state_prefab = find_state(state_name);
        if (!state_prefab.is_valid()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scenes Module] Cannot create state: prefab '{}' not found.", state_name);
            return flecs::entity::null();
        }
        
        flecs::entity state_instance = m_world.entity().is_a(state_prefab);
        state_instance.set<spectre_state_context_t>({state_instance});
        
        ::spectre::modules::scripts::execute_on_create(state_instance);
        return state_instance;
    }

    flecs::entity scenes_module_t::create_scene(std::string_view scene_name) {
        flecs::entity scene_prefab = find_scene(scene_name);
        if (!scene_prefab.is_valid()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scenes Module] Cannot create scene: prefab '{}' not found.", scene_name);
            return flecs::entity::null();
        }
        
        flecs::entity scene_instance = m_world.entity().is_a(scene_prefab);
        scene_instance.set<spectre_scene_context_t>({scene_instance});
        
        ::spectre::modules::scripts::execute_on_create(scene_instance);
        return scene_instance;
    }
}
