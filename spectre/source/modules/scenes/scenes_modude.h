#pragma once
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "flecs.h"

namespace spectre::module {
    struct Scene {};
    struct State {};

    class ScenesModule {
    public:
        ScenesModule(flecs::world& ecs);
        ~ScenesModule();

        void push_state(const char* state_name);
        void pop_state();

        flecs::entity current_state();
        std::vector<flecs::entity> currents_scenes();

    private:
        std::stack<flecs::world> m_state_stack;

    };
}