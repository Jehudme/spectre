#!/bin/bash
gdb -batch -ex "break abi_register_component_factory" -ex "run" -ex "n" -ex "n" -ex "p module" build/bin/spectre_unit_tests
