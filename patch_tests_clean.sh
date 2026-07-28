sed -i 's/spectre::modules::scenes::register_state(world, props);//g' tests/unit/source/modules/test_scenes.cpp
sed -i '1i #include <spectre/sdk/scenes.hpp>' tests/unit/source/modules/test_scenes.cpp

sed -i 's/spectre::modules::prefabs::register_prefab(world, "TestPrefabList", props);//g' tests/unit/source/modules/test_prefabs.cpp
sed -i '1i #include <spectre/sdk/prefabs.hpp>' tests/unit/source/modules/test_prefabs.cpp

sed -i '1i #include <spectre/sdk/resources.hpp>' tests/unit/source/modules/test_resources.cpp

sed -i '1i #include <spectre/sdk/scripts.hpp>' tests/unit/source/modules/test_scripts.cpp

