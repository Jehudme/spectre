#include <flecs.h>
#include <iostream>

int main() {
    flecs::world ecs;
    flecs::entity p = ecs.prefab("my_prefab");
    std::cout << "Empty prefab count: " << p.type().count() << "\n";
    p.each([](flecs::id id) {
        std::cout << "  Id: " << id.str() << "\n";
    });
    p.add<int>();
    std::cout << "After int count: " << p.type().count() << "\n";
    return 0;
}
