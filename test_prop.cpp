#include <sandbox/sdk/properties.hpp>
#include <iostream>

int main() {
    std::string json_obj = R"({
        "members": {
            "prop1": { "name": "health", "type": "int" },
            "prop2": { "name": "speed", "type": "float" }
        }
    })";
    sandbox::properties props;
    props.load(json_obj, sandbox::properties::Format::JSON);
    auto keys = props.keys("members");
    std::cout << "Keys size: " << keys.size() << std::endl;
    for (auto k : keys) std::cout << "Key: " << k << std::endl;
    
    auto name = props.get<std::string>("members.prop1.name").value_or("null");
    std::cout << "Name: " << name << std::endl;
    return 0;
}
