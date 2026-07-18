#include <flecs.h>
#include <iostream>

typedef struct spectre_2D_transform_component_t {
    float x, y;
} spectre_2D_transform_component_t;

int main() {
    ecs_world_t *world = ecs_init();
    
    // simulate how it's registered
    flecs::world w(world);
    w.component<spectre_2D_transform_component_t>();
    
    ecs_entity_t e = ecs_lookup(world, "spectre_2D_transform_component_t");
    std::cout << "Lookup spectre_2D_transform_component_t: " << e << std::endl;
    
    ecs_fini(world);
    return 0;
}
