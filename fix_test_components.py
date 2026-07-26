import re

path = 'tests/unit/source/modules/test_components.cpp'
with open(path, 'r') as f:
    content = f.read()

new_section = """
    SECTION("Dynamic Component Serialization") {
        std::string json_obj = R"({
            "members": {
                "prop1": { "name": "health", "type": "int" },
                "prop2": { "name": "speed", "type": "float" }
            }
        })";
        sandbox::properties props;
        props.load(json_obj, sandbox::properties::Format::JSON);

        spectre::modules::components::register_component(world, "DynamicCompTest", std::move(props));
        
        flecs::entity comp(world, spectre::modules::components::find_component(world, "DynamicCompTest"));
        REQUIRE(comp.is_valid() == true);
        
        // Find serializer
        flecs::entity serializer_entity(world, spectre::modules::serializer::find_serializer(world, "DynamicCompTest"));
        REQUIRE(serializer_entity.is_valid() == true);
        
        // Deserialize into entity
        flecs::entity target = world.entity();
        
        sandbox::properties input_props;
        input_props.set<int32_t>("health", 42);
        input_props.set<float>("speed", 3.14f);
        
        spectre::modules::serializer::deserialize_entity(world, serializer_entity.id(), target.id(), std::move(input_props));
        
        // Verify it was added
        REQUIRE(target.has(comp) == true);
        
        // Now serialize it back out
        sandbox::properties out_props = spectre::modules::serializer::serialize_entity(world, serializer_entity.id(), target.id());
        REQUIRE(out_props.is_valid() == true);
        
        // Verify values
        REQUIRE(out_props.get<int32_t>("health").value_or(0) == 42);
        REQUIRE(out_props.get<float>("speed").value_or(0.0f) == 3.14f);
    }
}
"""

content = content.replace("}\n}", "}\n" + new_section)

with open(path, 'w') as f:
    f.write(content)
