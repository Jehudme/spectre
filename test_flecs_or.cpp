#include <flecs.h>
#include <iostream>

struct RectangleRenderable {};
struct CircleRenderable {};

int main() {
    flecs::world w;
    w.component<RectangleRenderable>("RectangleRenderable");
    w.component<CircleRenderable>("CircleRenderable");
    
    auto q = w.query_builder<>()
        .expr("RectangleRenderable || CircleRenderable")
        .build();
        
    auto e1 = w.entity().add<RectangleRenderable>();
    auto e2 = w.entity().add<CircleRenderable>();
    auto e3 = w.entity(); // Should not match
    
    int count = 0;
    q.each([&](flecs::entity e) {
        count++;
        std::cout << "Matched " << e.id() << std::endl;
    });
    
    std::cout << "Total matched: " << count << std::endl;
    return 0;
}
