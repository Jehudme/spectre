import re
import os

filepath = "/home/jehud/CLionProjects/spectre/tests/unit/source/modules/test_components.cpp"
with open(filepath, 'r') as f:
    content = f.read()

content = re.sub(
    r'components_mod->register_component\("DummyComponent", register_dummy, serializer\);',
    r'sandbox::properties dummy_props;\n        dummy_props.load(R"({"members":[{"name":"x","type":"float"},{"name":"y","type":"float"}]})", sandbox::properties::Format::JSON);\n        components_mod->register_component("DummyComponent", register_dummy, serializer, dummy_props);',
    content
)

content = re.sub(
    r'components_mod->register_component\("", register_dummy, serializer\);',
    r'components_mod->register_component("", register_dummy, serializer, sandbox::properties());',
    content
)

# Add tests for is_static and find_schema
extra_tests = r'''
    SECTION("is_static and find_schema") {
        REQUIRE(spectre::modules::components::is_static(world, "DummyComponent") == true);
        REQUIRE(spectre::modules::components::is_static(world, "DynamicComp") == false);
        
        sandbox::properties dummy_schema = spectre::modules::components::find_schema(world, "DummyComponent");
        REQUIRE(dummy_schema.is_valid() == true);
        
        sandbox::properties dynamic_schema = spectre::modules::components::find_schema(world, "DynamicComp");
        REQUIRE(dynamic_schema.is_valid() == true);
    }
'''

if 'SECTION("is_static and find_schema")' not in content:
    content = content.replace('SECTION("Can list components") {', extra_tests + '\n    SECTION("Can list components") {')

with open(filepath, 'w') as f:
    f.write(content)
print("Patched test_components.cpp")
