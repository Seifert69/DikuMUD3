#!/bin/bash

# This is a bit hardcoded - from top level if you do
#
# mkdir build; cd build; cmake -DCMAKE_CXX_FLAGS=--coverage -DCMAKE_C_FLAGS=--coverage -DCMAKE_BUILD_TYPE=Debug ..
#
# Then you can run this script to run unit tests and generate report

pushd ..
toplevel=$(pwd)
popd

# VME Unit Tests
lcov -q --directory $toplevel/build --zerocounters
pushd ../vme/bin
vme_unit_tests
popd
lcov -q --directory $toplevel/build --base-directory $toplevel --capture --no-external --rc lcov_branch_coverage=1 --output-file vme_unit_tests.info
genhtml vme_unit_tests.info -q --prefix $toplevel --output-directory vme_report --show-details --legend --demangle-cpp --sort --rc lcov_branch_coverage=1

# PP Unit Tests
lcov -q --directory $toplevel/build --zerocounters
pushd ../vme/bin
pp_unit_tests
popd
lcov -q --directory $toplevel/build --base-directory $toplevel --capture --no-external --rc lcov_branch_coverage=1 --output-file pp_unit_tests.info
genhtml pp_unit_tests.info -q --prefix $toplevel --output-directory pp_report --show-details --legend --demangle-cpp --sort --rc lcov_branch_coverage=1

# DEFCOMP Unit Tests
lcov -q --directory $toplevel/build --zerocounters
pushd ../vme/bin
defcomp_unit_tests
popd
lcov -q --directory $toplevel/build --base-directory $toplevel --capture --no-external --rc lcov_branch_coverage=1 --output-file defcomp_unit_tests.info
genhtml defcomp_unit_tests.info -q --prefix $toplevel --output-directory defcomp_report --show-details --legend --demangle-cpp --sort --rc lcov_branch_coverage=1

# VMC Unit Tests
lcov -q --directory $toplevel/build --zerocounters
pushd ../vme/bin
vmc_unit_tests
popd
lcov -q --directory $toplevel/build --base-directory $toplevel --capture --no-external --rc lcov_branch_coverage=1 --output-file vmc_unit_tests.info
genhtml vmc_unit_tests.info -q --prefix $toplevel --output-directory vmc_report --show-details --legend --demangle-cpp --sort --rc lcov_branch_coverage=1

# MPLEX Unit Tests
lcov -q --directory $toplevel/build --zerocounters
pushd ../vme/bin
mplex_unit_tests
popd
lcov -q --directory $toplevel/build --base-directory $toplevel --capture --no-external --rc lcov_branch_coverage=1 --output-file mplex_unit_tests.info
genhtml mplex_unit_tests.info -q --prefix $toplevel --output-directory mplex_report --show-details --legend --demangle-cpp --sort --rc lcov_branch_coverage=1
