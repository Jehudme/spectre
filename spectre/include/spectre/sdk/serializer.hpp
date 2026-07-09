#pragma once
#include <spectre/services/serializer_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class serializer {
    public:
    static void register_serializer(const flecs::world& entity_world, const char* type, const spectre_serializer_component* serializer);

    static bool has_serializer(const flecs::world& entity_world, const char* type);
    static bool is_serializer(const flecs::world& entity_world, flecs::entity entity);

    static ecs_entity_t find_serializer(const flecs::world& entity_world, const char* type);

    static sandbox_properties_handle_t serialize_entity(const flecs::world& entity_world, ecs_entity_t serializer, ecs_entity_t entity);

    static ecs_entity_t deserialize_entity(const flecs::world& entity_world, ecs_entity_t serializer, sandbox_properties_handle_t props);
    };
}
#endif
