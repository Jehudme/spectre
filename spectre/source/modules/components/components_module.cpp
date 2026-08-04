
#include "components_module.h"
#include "sandbox/sdk/filesystem.hpp"
#include "sandbox/sdk/logs.hpp"
#include "spectre/components/serializer_component.h"
#include "spectre/sdk/serializer.hpp"
#include <string.h>

namespace spectre::modules {

struct spectre_component_schema_t {
    sandbox::properties schema;
};

struct spectre_dynamic_field_t {
    std::string name;
    spectre_dynamic_type_t type;
    size_t offset;
    size_t count;
};

struct spectre_dynamic_schema_component_t {
    sandbox::properties raw_properties;
    std::vector<spectre_dynamic_field_t> fields;
    size_t total_size;
    size_t alignment;
};

struct spectre_dynamic_component_header_t {
    ecs_entity_t schema_entity;
};

static void generic_dynamic_deserialize(ecs_world_t* world, ecs_entity_t serializer_entity, ecs_entity_t target_entity,
                                        sandbox_properties_handle_t props) {
    flecs::world w(world);
    flecs::entity s(w, serializer_entity);

    flecs::entity comp_id = s.target<spectre_serializer_relation_t>();
    if (!comp_id.is_valid()) {
        sandbox::modules::logs::error(w, "comp_id is invalid");
        return;
    }
    flecs::entity schema_entity = comp_id.target<spectre_serializer_relation_t>();
    if (!schema_entity.is_valid()) {
        sandbox::modules::logs::error(w, "schema_entity is invalid");
        return;
    }

    const auto* schema = schema_entity.try_get<spectre_dynamic_schema_component_t>();
    sandbox::modules::logs::info(w, "Deserialize: Start for target_entity={}, comp_id={}", target_entity, comp_id.id());
    if (!schema) {
        sandbox::modules::logs::error(w, "schema is null");
        return;
    }

    if (!ecs_is_valid(world, target_entity)) {
        sandbox::modules::logs::error(w, "target_entity is invalid");
        return;
    }

    const ecs_type_info_t* ti = ecs_get_type_info(world, comp_id.id());
    if (!ti) {
        sandbox::modules::logs::error(w, "type info is null");
        return;
    }

    sandbox::modules::logs::info(w, "Deserialize: type info size={}", ti->size);
    size_t comp_size = ti->size;
    void* temp_buffer = calloc(1, comp_size);

    auto* header = static_cast<spectre_dynamic_component_header_t*>(temp_buffer);
    header->schema_entity = schema_entity.id();

    uint8_t* base = static_cast<uint8_t*>(temp_buffer) + sizeof(spectre_dynamic_component_header_t);

    sandbox::properties input(props, false);
    for (const auto& field : schema->fields) {
        sandbox::modules::logs::info(w, "Deserialize: processing field {}", field.name);
        if (field.type == SPECTRE_DYNAMIC_TYPE_INT) {
            int32_t val = input.get<int32_t>(field.name).value_or(0);
            memcpy(base + field.offset, &val, sizeof(int32_t));
        } else if (field.type == SPECTRE_DYNAMIC_TYPE_FLOAT) {
            float val = input.get<float>(field.name).value_or(0.0f);
            memcpy(base + field.offset, &val, sizeof(float));
        } else if (field.type == SPECTRE_DYNAMIC_TYPE_DOUBLE) {
            double val = input.get<double>(field.name).value_or(0.0);
            memcpy(base + field.offset, &val, sizeof(double));
        } else if (field.type == SPECTRE_DYNAMIC_TYPE_BOOL) {
            bool val = input.get<bool>(field.name).value_or(false);
            memcpy(base + field.offset, &val, sizeof(bool));
        } else if (field.type == SPECTRE_DYNAMIC_TYPE_STRING) {
            std::string val = input.get<std::string>(field.name).value_or("");
            char** ptr = reinterpret_cast<char**>(base + field.offset);
            if (*ptr)
                free(*ptr);
            *ptr = strdup(val.c_str());
        }
    }

    sandbox::modules::logs::info(w, "Deserialize: Calling ecs_set_id");
    ecs_set_id(world, target_entity, comp_id.id(), comp_size, temp_buffer);
    sandbox::modules::logs::info(w, "Deserialize: freeing temp_buffer");
    free(temp_buffer);
}

static sandbox_properties_handle_t generic_dynamic_serialize(ecs_world_t* world, ecs_entity_t serializer_entity,
                                                             ecs_entity_t target_entity) {
    flecs::world w(world);
    flecs::entity s(w, serializer_entity);

    flecs::entity comp_id = s.target<spectre_serializer_relation_t>();
    if (!comp_id.is_valid()) {
        sandbox::modules::logs::error(w, "Serialize: comp_id is invalid");
        return {0};
    }

    const void* comp_data = ecs_get_id(world, target_entity, comp_id.id());
    if (!comp_data) {
        sandbox::modules::logs::error(w, "Serialize: comp_data is null");
        return {0};
    }

    const auto* header = static_cast<const spectre_dynamic_component_header_t*>(comp_data);
    sandbox::modules::logs::info(w, "Serialize: target={}, comp_id={}, header->schema_entity={}", target_entity,
                                 comp_id.id(), header->schema_entity);
    flecs::entity schema_entity = w.entity(header->schema_entity);

    if (!schema_entity.is_valid()) {
        sandbox::modules::logs::error(w, "Serialize: schema_entity is invalid");
        return {0};
    }

    const auto* schema = schema_entity.try_get<spectre_dynamic_schema_component_t>();
    if (!schema) {
        sandbox::modules::logs::error(w, "Serialize: schema is null");
        return {0};
    }

    sandbox::properties out;
    const uint8_t* base = static_cast<const uint8_t*>(comp_data) + sizeof(spectre_dynamic_component_header_t);

    for (const auto& field : schema->fields) {
        if (field.type == SPECTRE_DYNAMIC_TYPE_INT) {
            int32_t val = *reinterpret_cast<const int32_t*>(base + field.offset);
            out.set(field.name, val);
        } else if (field.type == SPECTRE_DYNAMIC_TYPE_FLOAT) {
            float val = *reinterpret_cast<const float*>(base + field.offset);
            out.set(field.name, val);
        } else if (field.type == SPECTRE_DYNAMIC_TYPE_DOUBLE) {
            double val = *reinterpret_cast<const double*>(base + field.offset);
            out.set(field.name, val);
        } else if (field.type == SPECTRE_DYNAMIC_TYPE_BOOL) {
            bool val = *reinterpret_cast<const bool*>(base + field.offset);
            out.set(field.name, val);
        } else if (field.type == SPECTRE_DYNAMIC_TYPE_STRING) {
            char* val = *reinterpret_cast<char* const*>(base + field.offset);
            if (val)
                out.set(field.name, std::string(val));
        }
    }
    sandbox_properties_handle_t handle = out.get_raw();
    out.release();
    return handle;
}

SANDBOX_DECLARE_MODULE(components_module_t, {.name = "components",
                                             .description = "Components management for the Spectre Engine",
                                             .architecture = "spectre",
                                             .version_major = 1,
                                             .version_minor = 0,
                                             .version_patch = 0,
                                             .service = &spectre_components_service_t_info,
                                             .requirements = {}})

components_module_t::components_module_t(flecs::world& world) : m_world(world) {
    m_world.component<spectre_component_schema_t>("spectre_component_schema_t");
    m_world.component<spectre_dynamic_schema_component_t>("spectre_dynamic_schema_component_t");
    m_world.component<components_module_t::spectre_component_dynamic_flag_t>("spectre_component_dynamic_flag_t");
    m_components_root = m_world.entity("::components");
    sandbox::modules::logs::trace(m_world, "[Components Module] Initializing...");

    sandbox::modules::logs::info(m_world, "[Components Module] Initialized successfully.");
    printf("[Components Module] Constructor finished. Address of this: %p, id is: %lu\n", this, m_world.id<components_module_t>());
}

components_module_t::~components_module_t() {}

void components_module_t::register_component(std::string_view name, spectre_component_registration_fn_t registration_fn,
                                             spectre_serializer_component serializer) {
    if (name.empty()) {
        sandbox::modules::logs::error(m_world, "[Components Module] Cannot register a component with an empty name.");
        return;
    }

    if (has_component(name)) {
        sandbox::modules::logs::trace(m_world, "[Components Module] Component '{}' is already registered. Overriding.",
                                      name.data());
    }

    ecs_entity_t comp_id = registration_fn(m_world.c_ptr());
    flecs::entity comp(m_world, comp_id);
    comp.child_of(m_components_root);
    comp.set<spectre_serializer_component>(serializer);

    if (serializer.serialize != nullptr && serializer.deserialize != nullptr) {
        spectre::modules::serializer::register_serializer(m_world, name.data(), &serializer);
    }

    flecs::entity serializer_entity = m_world.entity("::serializers").lookup(name.data());

    sandbox::modules::logs::trace(m_world, "[Components Module] Registered component '{}'.", name.data());
}

void components_module_t::register_component(std::string_view name, sandbox::properties properties) {
    if (name.empty())
        return;

    auto m_components_properties_root = m_world.entity("::components_properties");

    // 1. Create Schema Entity
    flecs::entity schema_entity = m_components_properties_root.lookup(name.data());
    if (!schema_entity.is_valid()) {
        schema_entity = m_world.entity(name.data()).child_of(m_components_properties_root);
    }

    spectre_dynamic_schema_component_t schema{};
    schema.raw_properties = std::move(properties);
    auto member_keys = schema.raw_properties.keys("members");
    size_t offset = 0;

    for (const auto& key : member_keys) {
        std::string path = "members/" + key;
        auto member_name = schema.raw_properties.get<std::string>(path + "/name").value_or("");
        auto member_type = schema.raw_properties.get<std::string>(path + "/type").value_or("float");

        spectre_dynamic_field_t field;
        field.name = member_name;

        size_t field_size = 0;
        size_t field_align = 0;

        // TODO:Add an pointers option
        if (member_type == "int" || member_type == "i32") {
            field.type = SPECTRE_DYNAMIC_TYPE_INT;
            field_size = sizeof(int32_t);
            field_align = alignof(int32_t);
        } else if (member_type == "float" || member_type == "f32") {
            field.type = SPECTRE_DYNAMIC_TYPE_FLOAT;
            field_size = sizeof(float);
            field_align = alignof(float);
        } else if (member_type == "double" || member_type == "f64") {
            field.type = SPECTRE_DYNAMIC_TYPE_DOUBLE;
            field_size = sizeof(double);
            field_align = alignof(double);
        } else if (member_type == "string") {
            field.type = SPECTRE_DYNAMIC_TYPE_STRING;
            field_size = sizeof(char*);
            field_align = alignof(char*);
        } else if (member_type == "bool") {
            field.type = SPECTRE_DYNAMIC_TYPE_BOOL;
            field_size = sizeof(bool);
            field_align = alignof(bool);
        } else {
            field.type = SPECTRE_DYNAMIC_TYPE_UNKNOWN;
        }

        if (field.type != SPECTRE_DYNAMIC_TYPE_UNKNOWN) {
            size_t padding = (field_align - (offset % field_align)) % field_align;
            offset += padding;
            field.offset = offset;
            schema.fields.push_back(field);
            offset += field_size;

            if (field_align > schema.alignment)
                schema.alignment = field_align;
        }
    }
    schema.total_size = offset;
    schema_entity.set<spectre_dynamic_schema_component_t>(std::move(schema));

    // 2. Register Dynamic Component in Flecs
    ecs_component_desc_t desc = {};
    ecs_entity_desc_t ent_desc = {};
    ent_desc.name = name.data();
    ent_desc.parent = m_components_root.id(); // Ensure it finds existing component instead of creating a new one in root
    desc.entity = ecs_entity_init(m_world.c_ptr(), &ent_desc);
    desc.type.size = sizeof(spectre_dynamic_component_header_t) + schema.total_size;
    desc.type.alignment = (alignof(spectre_dynamic_component_header_t) > schema.alignment)
                              ? alignof(spectre_dynamic_component_header_t)
                              : schema.alignment;

    ecs_entity_t comp_id = ecs_component_init(m_world.c_ptr(), &desc);
    flecs::entity comp(m_world, comp_id);
    comp.add<spectre_component_dynamic_flag_t>();
    if (!m_components_root.lookup(name.data()).is_valid()) {
        comp.child_of(m_components_root);
    }
    comp.add<spectre_serializer_relation_t>(schema_entity);

    // 3. Register Serializer
    spectre_serializer_component serializer = {generic_dynamic_deserialize, generic_dynamic_serialize};
    comp.set<spectre_serializer_component>(serializer);
    spectre::modules::serializer::register_serializer(m_world, name.data(), &serializer);

    flecs::entity serializer_entity = m_world.entity("::serializers").lookup(name.data());
    if (serializer_entity.is_valid()) {
        spectre_component_schema_t schema_comp;
        schema_comp.schema = sandbox::properties(properties.get_raw(), true);
        serializer_entity.set<spectre_component_schema_t>(std::move(schema_comp));
    }

    // TODO: Use the serializer module to get the serializer
    if (serializer_entity.is_valid()) {
        serializer_entity.add<spectre_serializer_relation_t>(comp);
        spectre_component_schema_t schema_comp;
        schema_comp.schema = sandbox::properties(properties.get_raw(), true);
        serializer_entity.set<spectre_component_schema_t>(std::move(schema_comp));
    }

    sandbox::modules::logs::info(m_world, "[Components Module] Dynamically registered component '{}' with size {}",
                                 name.data(), desc.type.size);
}

bool components_module_t::is_static(std::string_view name) const {
    flecs::entity comp = find_component(name);
    if (!comp.is_valid()) return false;
    return !comp.has<spectre_component_dynamic_flag_t>();
}

sandbox::properties components_module_t::find_schema(std::string_view name) const {
    flecs::entity serializer_entity = m_world.entity("::serializers").lookup(name.data());
    if (serializer_entity.is_valid() && serializer_entity.has<spectre_component_schema_t>()) {
        const auto* schema_comp = serializer_entity.try_get<spectre_component_schema_t>();
        return sandbox::properties(schema_comp->schema.get_raw(), false);
    }
    
    flecs::entity schema_entity = m_world.entity("::components_properties").lookup(name.data());
    if (schema_entity.is_valid() && schema_entity.has<spectre_dynamic_schema_component_t>()) {
        const auto* dyn_schema = schema_entity.try_get<spectre_dynamic_schema_component_t>();
        return sandbox::properties(dyn_schema->raw_properties.get_raw(), false);
    }
    
    sandbox_properties_handle_t null_handle = {0};
    return sandbox::properties(null_handle, false);
}

flecs::entity components_module_t::find_component(std::string_view name) const {
    if (name.empty())
        return flecs::entity::null();
    flecs::entity comp = m_components_root.lookup(name.data());
    if (!comp.is_valid()) {
        sandbox::modules::logs::error(const_cast<flecs::world&>(m_world),
                                      "[Components Module] Could not find component '{}'.", name.data());
    }
    return comp;
}

bool components_module_t::has_component(std::string_view name) const {
    if (name.empty())
        return false;
    return m_components_root.lookup(name.data()).is_valid();
}

bool components_module_t::is_component(flecs::entity entity) const {
    return entity.has<flecs::Component>() || entity.has<spectre_component_dynamic_flag_t>();
}

std::vector<flecs::entity> components_module_t::list_components() const {
    std::vector<flecs::entity> list;
    m_components_root.children([&](flecs::entity e) { list.push_back(e); });
    return list;
}

void components_module_t::import_configuration(std::string_view directory_path) {
    auto files = sandbox::modules::filesystem::list_files(m_world, directory_path.data(), true);
    for (const auto& file : files) {
        if (file.find(".json") != std::string::npos) {
            auto content = sandbox::modules::filesystem::read_all_text(m_world, file.c_str());
            if (!content.empty()) {
                sandbox::properties props;
                if (props.load(content, sandbox::properties::Format::JSON)) {
                    std::string name = file.substr(file.find_last_of('/') + 1);
                    name = name.substr(0, name.find_last_of('.'));
                    register_component(name, std::move(props));
                }
            }
        }
    }
}

void components_module_t::export_configuration(std::string_view directory_path) {
    // Left empty for now, or you can implement it based on m_components_properties_root
}

} // namespace spectre::modules
