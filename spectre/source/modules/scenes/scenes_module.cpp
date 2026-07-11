#include "scenes_module.h"
#include "spectre/services/scenes_service.h"
#include "modules/serializer/serializer_module.h"
#include "modules/scripts/scripts_module.h"
#include "spectre/sdk/scripts.hpp"
#include "sandbox/sdk/logs.hpp"
#include <iostream>

namespace spectre::modules {

    static sandbox_properties_handle_t serialize_state_cb(ecs_world_t* world, ecs_entity_t entity) {
        if (!world || !entity) return {0};
        flecs::world fw(world);
        auto* module = fw.try_get_mut<scenes_module_t>();
        if (!module) return {0};
        sandbox::properties props = module->serialize_state(fw.entity(entity));
        sandbox_properties_handle_t handle = props.get_raw();
        props.release();
        return handle;
    }

    static ecs_entity_t deserialize_state_cb(ecs_world_t* world, sandbox_properties_handle_t props_handle) {
        if (!world) return 0;
        flecs::world fw(world);
        auto* module = fw.try_get_mut<scenes_module_t>();
        if (!module) return 0;
        sandbox::properties props(props_handle, false);
        return module->deserialize_state(std::move(props)).id();
    }

    static sandbox_properties_handle_t serialize_scene_cb(ecs_world_t* world, ecs_entity_t entity) {
        if (!world || !entity) return {0};
        flecs::world fw(world);
        auto* module = fw.try_get_mut<scenes_module_t>();
        if (!module) return {0};
        sandbox::properties props = module->serialize_scene(fw.entity(entity));
        sandbox_properties_handle_t handle = props.get_raw();
        props.release();
        return handle;
    }

    static ecs_entity_t deserialize_scene_cb(ecs_world_t* world, sandbox_properties_handle_t props_handle) {
        if (!world) return 0;
        flecs::world fw(world);
        auto* module = fw.try_get_mut<scenes_module_t>();
        if (!module) return 0;
        sandbox::properties props(props_handle, false);
        return module->deserialize_scene(std::move(props)).id();
    }

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
        m_states_root = m_world.entity("::states");
        m_scenes_root = m_world.entity("::scenes");

        auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>()); // again use sdk alway use sdk no dirreclt call
        if (serializer_mod) {
            spectre_serializer_component state_ser;
            state_ser.serialize = serialize_state_cb;
            state_ser.deserialize = deserialize_state_cb;
            serializer_mod->register_serializer("state", state_ser);

            spectre_serializer_component scene_ser;
            scene_ser.serialize = serialize_scene_cb;
            scene_ser.deserialize = deserialize_scene_cb;
            serializer_mod->register_serializer("scene", scene_ser);
            
            m_scene_serializer = serializer_mod->find_serializer("scene");
            m_state_serializer = serializer_mod->find_serializer("state");
            m_script_args_serializer = serializer_mod->find_serializer("scripts");
            m_entity_serializer = serializer_mod->find_serializer("entity");
        }

        struct spectre_scenes_update_marker_t {};
        m_world.component<spectre_scenes_update_marker_t>();
        m_world.entity("Scenes Update Marker").add<spectre_scenes_update_marker_t>();

        m_world.system<spectre_scenes_update_marker_t>("Scenes OnUpdate")
            .kind(flecs::OnUpdate)
            .each([this](flecs::entity e, spectre_scenes_update_marker_t) {
                flecs::entity current = find_current_state();
                if (current.is_valid() && !current.has<spectre_disable_rendering_t>()) {
                    ::spectre::sdk::scripts::execute_on_update(current);
                    
                    current.children([&](flecs::entity scene) {
                        if (scene.has<spectre_state_use_scene_relation_t>()) {
                            ::spectre::sdk::scripts::execute_on_update(scene);
                            
                            scene.children([&](flecs::entity child) {
                                if (!child.has<spectre_scene_t>() && !child.has<spectre_state_t>()) {
                                    ::spectre::sdk::scripts::execute_on_update(child);
                                }
                            });
                        }
                    });
                }
            });
    }
    
    scenes_module_t::~scenes_module_t() = default;

    sandbox::properties scenes_module_t::serialize_state(flecs::entity state) {
        sandbox::properties props;
        if (!state.is_valid()) return props;
        
        props.set<std::string>("name", state.name().c_str());
        
        // Serialize scripts
        flecs::entity scripts_child = state.lookup("scripts");
        if (scripts_child.is_valid() && m_script_args_serializer.is_valid()) {
            auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
            if (serializer_mod) {
                sandbox::properties scripts_node = serializer_mod->serialize_entity(m_script_args_serializer, scripts_child);
                if (scripts_node.is_valid()) {
                    props.merge("scripts", scripts_node);
                }
            }
        }
        
        // Serialize scenes
        sandbox::properties scenes_arr;
        int layer = 0;
        state.children([&](flecs::entity child) {
            if (is_scene(child)) {
                scenes_arr.set<std::string>(std::to_string(layer++), child.name().c_str());
            }
        });
        
        if (layer > 0) {
            props.merge("scenes", scenes_arr);
        }

        return props;
    }

    flecs::entity scenes_module_t::deserialize_state(sandbox::properties props) {
        if (!props.is_valid()) return flecs::entity::null();
        
        flecs::entity state = m_world.entity();
        std::string name;
        if (props.get<std::string>("name", name)) {
            flecs::entity existing = find_state(name);
            if (existing.is_valid()) {
                state = existing;
            } else {
                state.set_name(name.c_str());
            }
        }
        
        state.add<spectre_state_t>();
        
        if (props.has("scripts") && m_script_args_serializer.is_valid()) {
            auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
            if (serializer_mod) {
                sandbox::properties scripts_node = props.sub("scripts");
                flecs::entity temp_scripts_ent = serializer_mod->deserialize_entity(m_script_args_serializer, scripts_node);
                if (temp_scripts_ent.is_valid()) {
                    temp_scripts_ent.child_of(state);
                    temp_scripts_ent.set_name("scripts");
                    temp_scripts_ent.add(flecs::Prefab);
                }
            }
        }

        if (props.has("scenes")) {
            std::vector<std::string> scene_keys = props.keys("scenes");
            sandbox::properties scenes_node = props.sub("scenes");
            
            for (size_t i = 0; i < scene_keys.size(); ++i) {
                std::string scene_name;
                if (scenes_node.get<std::string>(std::to_string(i), scene_name)) {
                    flecs::entity scene_prefab = find_scene(scene_name);
                    if (scene_prefab.is_valid()) {
                        flecs::entity scene_inst = m_world.prefab().is_a(scene_prefab).child_of(state);
                        scene_inst.set<spectre_state_use_scene_relation_t>({static_cast<int>(i)});
                    }
                }
            }
        }

        return state;
    }

    sandbox::properties scenes_module_t::serialize_scene(flecs::entity scene) {
        sandbox::properties props;
        if (!scene.is_valid()) return props;
        
        props.set<std::string>("name", scene.name().c_str());
        
        // Serialize scripts
        flecs::entity scripts_child = scene.lookup("scripts");
        if (scripts_child.is_valid() && m_script_args_serializer.is_valid()) {
            auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
            if (serializer_mod) {
                sandbox::properties scripts_node = serializer_mod->serialize_entity(m_script_args_serializer, scripts_child);
                if (scripts_node.is_valid()) {
                    props.merge("scripts", scripts_node);
                }
            }
        }

        // Serialize hierarchies
        if (m_entity_serializer.is_valid()) {
            auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
            if (serializer_mod) {
                sandbox::properties hierarchies_arr;
                int child_idx = 0;
                scene.children([&](flecs::entity child) {
                    if (child.name() != "scripts") {
                        sandbox::properties child_props = serializer_mod->serialize_entity(m_entity_serializer, child);
                        if (child_props.is_valid()) {
                            hierarchies_arr.merge(std::to_string(child_idx++), child_props);
                        }
                    }
                });
                if (child_idx > 0) {
                    props.merge("hierarchies", hierarchies_arr);
                }
            }
        }

        return props;
    }

    flecs::entity scenes_module_t::deserialize_scene(sandbox::properties props) {
        if (!props.is_valid()) return flecs::entity::null();
        
        flecs::entity scene = m_world.entity();
        std::string name;
        if (props.get<std::string>("name", name)) {
            flecs::entity existing = find_scene(name);
            if (existing.is_valid()) {
                scene = existing;
            } else {
                scene.set_name(name.c_str());
            }
        }
        
        scene.add<spectre_scene_t>();

        if (props.has("scripts") && m_script_args_serializer.is_valid()) {
            auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
            if (serializer_mod) {
                sandbox::properties scripts_node = props.sub("scripts");
                flecs::entity temp_scripts_ent = serializer_mod->deserialize_entity(m_script_args_serializer, scripts_node);
                if (temp_scripts_ent.is_valid()) {
                    temp_scripts_ent.child_of(scene);
                    temp_scripts_ent.set_name("scripts");
                    temp_scripts_ent.add(flecs::Prefab);
                }
            }
        }

        if (props.has("hierarchies") && m_entity_serializer.is_valid()) {
            auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
            if (serializer_mod) {
                std::vector<std::string> keys = props.keys("hierarchies");
                sandbox::properties hierarchies_node = props.sub("hierarchies");
                
                for (const auto& key : keys) {
                    sandbox::properties child_props = hierarchies_node.sub(key);
                    if (child_props.is_valid()) {
                        flecs::entity child_ent = serializer_mod->deserialize_entity(m_entity_serializer, child_props);
                        if (child_ent.is_valid()) {
                            child_ent.child_of(scene);
                            child_ent.add(flecs::Prefab);
                        }
                    }
                }
            }
        }
        
        return scene;
    }

    void scenes_module_t::register_state(sandbox::properties props) {
        if (!props.is_valid()) return;
        std::string name;
        if (!props.get<std::string>("name", name) || name.empty()) {
            sandbox::modules::logs::error(m_world, "Cannot register state with empty name.");
            return;
        }

        flecs::entity state_prefab = find_state(name);
        if (!state_prefab.is_valid()) {
            state_prefab = m_world.prefab(name.c_str())
                                  .child_of(m_states_root)
                                  .add<spectre_state_t>();
        }
        props.set<std::string>("name", name);
        
        // Clean up old scripts/scenes if we are re-registering
        state_prefab.children([&](flecs::entity child) {
            child.destruct();
        });
        
        deserialize_state(std::move(props));
    }

    void scenes_module_t::register_scene(sandbox::properties props) {
        if (!props.is_valid()) return;
        std::string name;
        if (!props.get<std::string>("name", name) || name.empty()) {
            sandbox::modules::logs::error(m_world, "Cannot register scene with empty name.");
            return;
        }

        flecs::entity scene_prefab = find_scene(name);
        if (!scene_prefab.is_valid()) {
            scene_prefab = m_world.prefab(name.c_str())
                                  .child_of(m_scenes_root)
                                  .add<spectre_scene_t>();
        }
        props.set<std::string>("name", name);
        
        // Clean up old scripts/hierarchies if we are re-registering
        scene_prefab.children([&](flecs::entity child) {
            child.destruct();
        });
        
        deserialize_scene(std::move(props));
    }

    flecs::entity scenes_module_t::find_state(std::string_view name) {
        if (name.empty()) return flecs::entity::null();
        return m_states_root.lookup(std::string(name).c_str());
    }

    flecs::entity scenes_module_t::find_scene(std::string_view name) {
        if (name.empty()) return flecs::entity::null();
        return m_scenes_root.lookup(std::string(name).c_str());
    }

    bool scenes_module_t::has_state(std::string_view name) const {
        if (name.empty()) return false;
        flecs::entity state = m_states_root.lookup(std::string(name).c_str());
        return is_state(state);
    }

    bool scenes_module_t::has_scene(std::string_view name) const {
        if (name.empty()) return false;
        flecs::entity scene = m_scenes_root.lookup(std::string(name).c_str());
        return is_scene(scene);
    }

    bool scenes_module_t::is_state(flecs::entity entity) const {
        return entity.is_valid() && entity.has<spectre_state_t>();
    }

    bool scenes_module_t::is_scene(flecs::entity entity) const {
        return entity.is_valid() && entity.has<spectre_scene_t>();
    }

    flecs::entity scenes_module_t::find_current_state() {
        if (!m_state_stack.empty()) {
            return m_state_stack.top();
        }
        return flecs::entity::null();
    }

    flecs::query<> scenes_module_t::find_current_scenes() {
        // Return a query matching entities with spectre_scene_t that are children of the current state
        flecs::entity current_state = find_current_state();
        if (current_state.is_valid()) {
            return m_world.query_builder<>(m_world.entity()).with(flecs::ChildOf, current_state).with<spectre_scene_t>().build();
        }
        return m_world.query_builder<>(m_world.entity()).with<spectre_scene_t>().with(flecs::ChildOf, (flecs::entity_t)0).build(); // Return empty match if no current state
    }

    void scenes_module_t::push_state(flecs::entity state_prefab) {
        if (!state_prefab.is_valid()) return;

        // Run on_exit on current state
        if (!m_state_stack.empty()) {
            flecs::entity current = m_state_stack.top();
            if (current.is_valid()) {
                execute_recursive(current, [](ecs_world_t* world, ecs_entity_t entity, void* payload) {
                    flecs::world fw(world);
                    ::spectre::sdk::scripts::execute_on_exit(fw.entity(entity));
                }, nullptr);
                current.add<spectre_disable_rendering_t>();
            }
        }

        // Create new state instance
        flecs::entity state_inst = m_world.entity().is_a(state_prefab);
        state_inst.set<spectre_state_context_t>({state_inst});
        m_state_stack.push(state_inst);

        // Run on_enter recursively on the new current_state
        execute_recursive(state_inst, [](ecs_world_t* world, ecs_entity_t entity, void* payload) {
            flecs::world fw(world);
            ::spectre::sdk::scripts::execute_on_enter(fw.entity(entity));
        }, nullptr);
    }

    void scenes_module_t::pop_state() {
        if (m_state_stack.empty()) return;
        
        flecs::entity current = m_state_stack.top();
        if (current.is_valid()) {
            execute_recursive(current, [](ecs_world_t* world, ecs_entity_t entity, void* payload) {
                flecs::world fw(world);
                flecs::entity e = fw.entity(entity);
                ::spectre::sdk::scripts::execute_on_exit(e);
                ::spectre::sdk::scripts::execute_on_destroy(e);
            }, nullptr);
            current.destruct();
        }
        
        m_state_stack.pop();
        
        if (!m_state_stack.empty()) {
            flecs::entity top = m_state_stack.top();
            if (top.is_valid()) {
                top.remove<spectre_disable_rendering_t>();
                execute_recursive(top, [](ecs_world_t* world, ecs_entity_t entity, void* payload) {
                    flecs::world fw(world);
                    ::spectre::sdk::scripts::execute_on_enter(fw.entity(entity));
                }, nullptr);
            }
        }
    }

    void scenes_module_t::execute_recursive(flecs::entity entity, spectre_recursive_callback_t callback, void* payload) {
        if (!entity.is_valid() || !callback) return;
        callback(m_world.c_ptr(), entity.id(), payload);
        entity.children([&](flecs::entity child) {
            if (child.name() != "scripts") { // skip the scripts child container itself
                execute_recursive(child, callback, payload);
            }
        });
    }

    flecs::entity scenes_module_t::create_state(std::string_view name) {
        flecs::entity state_prefab = find_state(name);
        if (!state_prefab.is_valid()) return flecs::entity::null();
        
        flecs::entity inst = m_world.entity().is_a(state_prefab);
        inst.set<spectre_state_context_t>({inst});
        
        ::spectre::sdk::scripts::execute_on_create(inst);
        return inst;
    }

    flecs::entity scenes_module_t::create_scene(std::string_view name) {
        flecs::entity scene_prefab = find_scene(name);
        if (!scene_prefab.is_valid()) return flecs::entity::null();
        
        flecs::entity inst = m_world.entity().is_a(scene_prefab);
        inst.set<spectre_scene_context_t>({inst});
        
        ::spectre::sdk::scripts::execute_on_create(inst);
        return inst;
    }
}
