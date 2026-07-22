#include <flecs.h>
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
    
    auto e1 = w.lookup("spectre::modules::components_module_t");
    auto e2 = w.lookup("spectre::modules::serializer_module");
    std::cout << "e1 valid: " << e1.is_valid() << "\n";
    std::cout << "e2 valid: " << e2.is_valid() << "\n";
}
