#!/bin/bash
gdb -batch -ex "break CATCH2_INTERNAL_TEST_0" -ex "run" -ex "n" -ex "n" -ex "n" -ex "n" -ex "s" -ex "bt full" build/bin/spectre_unit_tests
