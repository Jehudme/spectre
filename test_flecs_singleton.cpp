#include <flecs.h>
#include <iostream>
struct MySingleton { int x = 42; };
int main() {
    flecs::world w;
    w.import<MySingleton>();
    w.get_mut<MySingleton>()->x = 100;
    
    ecs_entity_t id = w.id<MySingleton>();
    void* ptr = ecs_get_mut_id(w.c_ptr(), id, id);
    if (ptr) {
        std::cout << "Singleton found on itself! x=" << static_cast<MySingleton*>(ptr)->x << "\n";
    } else {
        std::cout << "Singleton NOT found on itself!\n";
    }
}
