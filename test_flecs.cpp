#include <flecs.h>
#include <iostream>

struct MyRelation {};

int main() {
    flecs::world w;
    flecs::entity script1 = w.entity("script1");
    flecs::entity script2 = w.entity("script2");
    
    flecs::entity prefab = w.prefab("prefab")
        .add<MyRelation>(script1)
        .add<MyRelation>(script2);
        
    flecs::entity inst = w.entity("inst").is_a(prefab);
    
    int index = 0;
    while (flecs::entity tgt = inst.target<MyRelation>(index++)) {
        std::cout << "Target: " << tgt.name() << "\n";
    }
    
    return 0;
}
