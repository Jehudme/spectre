#pragma once
#include <spectre/services/serializer_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class serializer {
    public:
        static void register_serializer(const flecs::world& entity_world, const char* type, const spectre_serializer_t* serializer) {
            spectre_serializer_register_serializer(entity_world.c_ptr(), type, serializer);}

        static bool has_serializer(const flecs::world& entity_world, const char* type) {
            return spectre_serializer_has_serializer(entity_world.c_ptr(), type);
        }

        static ecs_entity_t find_serializer(const flecs::world& entity_world, const char* type) {
            return spectre_serializer_find_serializer(entity_world.c_ptr(), type);
        }

        static sandbox_properties_handle_t serialize_entity(const flecs::world& entity_world, ecs_entity_t serializer, ecs_entity_t entity) {
            return spectre_serializer_serialize_entity(entity_world.c_ptr(), serializer, entity);
        }

        static ecs_entity_t deserialize_entity(const flecs::world& entity_world, ecs_entity_t serializer, sandbox_properties_handle_t props) {
            return spectre_serializer_deserialize_entity(entity_world.c_ptr(), serializer, props);
        }
    };
}
#endif
