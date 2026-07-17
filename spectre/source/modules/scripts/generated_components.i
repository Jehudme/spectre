// Auto-generated SWIG templates for components

%extend spectre_world {
    ecs_entity_t lookup(const char* name) {
        return ecs_lookup(self->world, name);
    }
    spectre_input_state_t* get_mut_input(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_input_state_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_input_state_t>();
        return (spectre_input_state_t*)ecs_get_mut_id(self->world, e, comp_id);
    }
    spectre_input_state_t* get_mut_singleton_input() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_input_state_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_input_state_t>();
        return (spectre_input_state_t*)ecs_get_mut_id(self->world, comp_id, comp_id);
    }
    const spectre_input_state_t* get_input(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_input_state_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_input_state_t>();
        return (const spectre_input_state_t*)ecs_get_id(self->world, e, comp_id);
    }
    const spectre_input_state_t* get_singleton_input() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_input_state_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_input_state_t>();
        return (const spectre_input_state_t*)ecs_get_id(self->world, comp_id, comp_id);
    }
    void set_input(ecs_entity_t e, const spectre_input_state_t& value) {
        if (!e) return;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_input_state_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_input_state_t>();
        ecs_set_id(self->world, e, comp_id, sizeof(spectre_input_state_t), &value);
    }
    bool has_input(ecs_entity_t e) {
        if (!e) return false;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_input_state_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_input_state_t>();
        return ecs_has_id(self->world, e, comp_id);
    }

    spectre_window_component_t* get_mut_window(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_window_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_window_component_t>();
        return (spectre_window_component_t*)ecs_get_mut_id(self->world, e, comp_id);
    }
    spectre_window_component_t* get_mut_singleton_window() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_window_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_window_component_t>();
        return (spectre_window_component_t*)ecs_get_mut_id(self->world, comp_id, comp_id);
    }
    const spectre_window_component_t* get_window(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_window_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_window_component_t>();
        return (const spectre_window_component_t*)ecs_get_id(self->world, e, comp_id);
    }
    const spectre_window_component_t* get_singleton_window() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_window_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_window_component_t>();
        return (const spectre_window_component_t*)ecs_get_id(self->world, comp_id, comp_id);
    }
    void set_window(ecs_entity_t e, const spectre_window_component_t& value) {
        if (!e) return;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_window_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_window_component_t>();
        ecs_set_id(self->world, e, comp_id, sizeof(spectre_window_component_t), &value);
    }
    bool has_window(ecs_entity_t e) {
        if (!e) return false;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_window_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_window_component_t>();
        return ecs_has_id(self->world, e, comp_id);
    }

    spectre_resource_component_t* get_mut_resource(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_component_t>();
        return (spectre_resource_component_t*)ecs_get_mut_id(self->world, e, comp_id);
    }
    spectre_resource_component_t* get_mut_singleton_resource() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_component_t>();
        return (spectre_resource_component_t*)ecs_get_mut_id(self->world, comp_id, comp_id);
    }
    const spectre_resource_component_t* get_resource(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_component_t>();
        return (const spectre_resource_component_t*)ecs_get_id(self->world, e, comp_id);
    }
    const spectre_resource_component_t* get_singleton_resource() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_component_t>();
        return (const spectre_resource_component_t*)ecs_get_id(self->world, comp_id, comp_id);
    }
    void set_resource(ecs_entity_t e, const spectre_resource_component_t& value) {
        if (!e) return;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_component_t>();
        ecs_set_id(self->world, e, comp_id, sizeof(spectre_resource_component_t), &value);
    }
    bool has_resource(ecs_entity_t e) {
        if (!e) return false;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_component_t>();
        return ecs_has_id(self->world, e, comp_id);
    }

    spectre_resource_loader_component_t* get_mut_resource_loader(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_loader_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_loader_component_t>();
        return (spectre_resource_loader_component_t*)ecs_get_mut_id(self->world, e, comp_id);
    }
    spectre_resource_loader_component_t* get_mut_singleton_resource_loader() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_loader_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_loader_component_t>();
        return (spectre_resource_loader_component_t*)ecs_get_mut_id(self->world, comp_id, comp_id);
    }
    const spectre_resource_loader_component_t* get_resource_loader(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_loader_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_loader_component_t>();
        return (const spectre_resource_loader_component_t*)ecs_get_id(self->world, e, comp_id);
    }
    const spectre_resource_loader_component_t* get_singleton_resource_loader() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_loader_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_loader_component_t>();
        return (const spectre_resource_loader_component_t*)ecs_get_id(self->world, comp_id, comp_id);
    }
    void set_resource_loader(ecs_entity_t e, const spectre_resource_loader_component_t& value) {
        if (!e) return;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_loader_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_loader_component_t>();
        ecs_set_id(self->world, e, comp_id, sizeof(spectre_resource_loader_component_t), &value);
    }
    bool has_resource_loader(ecs_entity_t e) {
        if (!e) return false;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_resource_loader_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_resource_loader_component_t>();
        return ecs_has_id(self->world, e, comp_id);
    }

    spectre_2D_transform_component_t* get_mut_2D_transform(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_2D_transform_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_2D_transform_component_t>();
        return (spectre_2D_transform_component_t*)ecs_get_mut_id(self->world, e, comp_id);
    }
    spectre_2D_transform_component_t* get_mut_singleton_2D_transform() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_2D_transform_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_2D_transform_component_t>();
        return (spectre_2D_transform_component_t*)ecs_get_mut_id(self->world, comp_id, comp_id);
    }
    const spectre_2D_transform_component_t* get_2D_transform(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_2D_transform_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_2D_transform_component_t>();
        return (const spectre_2D_transform_component_t*)ecs_get_id(self->world, e, comp_id);
    }
    const spectre_2D_transform_component_t* get_singleton_2D_transform() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_2D_transform_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_2D_transform_component_t>();
        return (const spectre_2D_transform_component_t*)ecs_get_id(self->world, comp_id, comp_id);
    }
    void set_2D_transform(ecs_entity_t e, const spectre_2D_transform_component_t& value) {
        if (!e) return;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_2D_transform_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_2D_transform_component_t>();
        ecs_set_id(self->world, e, comp_id, sizeof(spectre_2D_transform_component_t), &value);
    }
    bool has_2D_transform(ecs_entity_t e) {
        if (!e) return false;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_2D_transform_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_2D_transform_component_t>();
        return ecs_has_id(self->world, e, comp_id);
    }

    spectre_component_t* get_mut_component_t(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_component_t>();
        return (spectre_component_t*)ecs_get_mut_id(self->world, e, comp_id);
    }
    spectre_component_t* get_mut_singleton_component_t() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_component_t>();
        return (spectre_component_t*)ecs_get_mut_id(self->world, comp_id, comp_id);
    }
    const spectre_component_t* get_component_t(ecs_entity_t e) {
        if (!e) return nullptr;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_component_t>();
        return (const spectre_component_t*)ecs_get_id(self->world, e, comp_id);
    }
    const spectre_component_t* get_singleton_component_t() {
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_component_t>();
        return (const spectre_component_t*)ecs_get_id(self->world, comp_id, comp_id);
    }
    void set_component_t(ecs_entity_t e, const spectre_component_t& value) {
        if (!e) return;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_component_t>();
        ecs_set_id(self->world, e, comp_id, sizeof(spectre_component_t), &value);
    }
    bool has_component_t(ecs_entity_t e) {
        if (!e) return false;
        ecs_entity_t comp_id = ecs_lookup(self->world, "spectre_component_t");
        if (!comp_id) comp_id = flecs::world(self->world).id<spectre_component_t>();
        return ecs_has_id(self->world, e, comp_id);
    }

}
