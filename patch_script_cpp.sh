sed -i '/} \/\/ namespace spectre::modules/i \
std::vector<flecs::entity> script_module_t::list_scripts() const {\n    std::vector<flecs::entity> list;\n    m_scripts_root.children([&](flecs::entity e) {\n        list.push_back(e);\n    });\n    return list;\n}\n' spectre/source/modules/scripts/scripts_module.cpp
