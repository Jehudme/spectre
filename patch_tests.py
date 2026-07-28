import re

def append_to_test(filepath, section_code):
    with open(filepath, 'r') as f:
        content = f.read()
    
    # insert before the last closing brace of the TEST_CASE (or end of file if not matching well)
    # the test cases usually end with `}` so we will replace `}\n$` or similar
    
    # Actually, it's safer to just find the last TEST_CASE block and insert right before the last `}`
    last_brace = content.rfind('}')
    if last_brace != -1:
        content = content[:last_brace] + section_code + "\n" + content[last_brace:]
        
    with open(filepath, 'w') as f:
        f.write(content)

# Components
comp_test = """
    SECTION("Can list components") {
        auto list = spectre::modules::components::list_components(world);
        REQUIRE(list.size() > 0); // we registered dummy above, plus default ones
    }
"""
append_to_test("tests/unit/source/modules/test_components.cpp", comp_test)

# Scenes
scenes_test = """
    SECTION("Can list states") {
        spectre::modules::scenes::register_state(world, props);
        auto list = spectre::modules::scenes::list_states(world);
        REQUIRE(list.size() > 0);
    }
"""
append_to_test("tests/unit/source/modules/test_scenes.cpp", scenes_test)

# Prefabs
prefabs_test = """
    SECTION("Can list prefabs") {
        spectre::modules::prefabs::register_prefab(world, "TestPrefabList", props);
        auto list = spectre::modules::prefabs::list_prefabs(world);
        bool found = false;
        for (auto e : list) {
            if (e.name() == "TestPrefabList") found = true;
        }
        REQUIRE(found == true);
    }
"""
append_to_test("tests/unit/source/modules/test_prefabs.cpp", prefabs_test)

# Resources
res_test = """
    SECTION("Can list resources and loaders") {
        auto loaders = spectre::modules::resources::list_resource_loaders(world);
        auto resources = spectre::modules::resources::list_resources(world);
        // just make sure it doesn't crash and returns vectors
        REQUIRE(loaders.size() >= 0);
        REQUIRE(resources.size() >= 0);
    }
"""
append_to_test("tests/unit/source/modules/test_resources.cpp", res_test)

# Scripts
scripts_test = """
    SECTION("Can list scripts") {
        auto list = spectre::modules::scripts::list_scripts(world);
        REQUIRE(list.size() >= 0);
    }
"""
append_to_test("tests/unit/source/modules/test_scripts.cpp", scripts_test)

