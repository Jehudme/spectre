#!/bin/bash
gdb -batch -ex "break abi_create_entity" -ex "run" -ex "s" -ex "s" -ex "s" -ex "s" -ex "s" -ex "s" -ex "bt" build/bin/spectre_unit_tests
