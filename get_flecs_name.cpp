#include "flecs.h"
#include <iostream>

namespace spectre::modules {
    struct components_module_t {
        components_module_t(flecs::world& w) {}
    };
    struct serializer_module {
        serializer_module(flecs::world& w) {}
    };
}
int main() {
    flecs::world w;
    w.import<spectre::modules::components_module_t>();
    w.import<spectre::modules::serializer_module>();
    std::cout << w.id<spectre::modules::components_module_t>().path() << "\n";
    std::cout << w.id<spectre::modules::serializer_module>().path() << "\n";
}
