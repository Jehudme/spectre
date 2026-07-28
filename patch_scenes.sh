sed -i 's/void list_states() const;/std::vector<flecs::entity> list_states() const;/g' spectre/source/modules/scenes/scenes_module.h
