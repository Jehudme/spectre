#!/bin/bash
gdb -batch -ex "break _ZN7spectre6module13PrefabsModule15is_prefab_emptyEN5flecs6entityE" -ex "run" -ex "c" -ex "bt" build/bin/spectre_unit_tests
