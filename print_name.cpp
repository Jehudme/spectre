#include <flecs.h>
#include <iostream>

namespace spectre::modules {
    struct spectre_2D_transform_component_t {
        float position_x, position_y, rotation, scale_x, scale_y;
    };
}

int main() {
    flecs::world w;
    auto comp = w.component<spectre::modules::spectre_2D_transform_component_t>();
    std::cout << "Name: " << comp.name().c_str() << std::endl;
    std::cout << "Path: " << comp.path().c_str() << std::endl;
    return 0;
}
