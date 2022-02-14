cmake_minimum_required(VERSION 3.16.3)
project(DikuMUD3)

find_package(BISON)
find_package(FLEX)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug)
else ()
    message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}'.")
endif ()

if (DIKU_STATIC)
    set(Boost_USE_STATIC_LIBS ON) # This has to be before the boost find package below
    add_compile_definitions(DIKU_STATIC)
    message(STATUS "DIKU: Linking with static boost libraries")
else ()
    set(Boost_USE_STATIC_LIBS OFF)
    message(STATUS "DIKU: Linking with dynamic boost libraries")
endif ()
find_package(Boost COMPONENTS unit_test_framework filesystem regex system REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Common cmake flags for all builds
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fdiagnostics-color=always")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    # Stronger warnings for debug build
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wredundant-move -Wvirtual-move-assign -Wpessimizing-move")

    # Add warnings for newer compilers
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "11.2.1")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-tags")
    endif ()
else ()
    # Hide some warnings for a release build
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif ()

if (WERROR)
    # Treat all warnings as errors if cmake generator is run with -DWERROR=1
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif ()

message(STATUS "Compiler: ${CMAKE_C_COMPILER} version: ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER} version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin)

link_directories(${CMAKE_SOURCE_DIR}/vme/bin)

add_subdirectory(vme/src)
add_subdirectory(vme/src/defcomp)
add_subdirectory(vme/src/mplex)
add_subdirectory(vme/src/pp)
add_subdirectory(vme/src/vmc)
add_subdirectory(vme/zone)
add_subdirectory(vme/etc)

enable_testing()
add_subdirectory(unit_tests)

# Run the unit tests, but dont fail the build because of them
set(TEST_RUN_ARGS --log_level=message --no_result_code)
# Add dependencies so that the tests run after everything and are last result on console
add_custom_target(all_binaries ALL)
add_dependencies(all_binaries
        vme vmc mplex pp defcomp vmc_unit_tests pp_unit_tests mplex_unit_tests
        defcomp_unit_tests vme_unit_tests zone_files dat_files color_files)

add_custom_command(TARGET all_binaries POST_BUILD
        COMMENT "Running all unit tests"
        COMMAND defcomp_unit_tests ${TEST_RUN_ARGS}
        COMMAND pp_unit_tests ${TEST_RUN_ARGS}
        COMMAND vmc_unit_tests ${TEST_RUN_ARGS}
        COMMAND mplex_unit_tests ${TEST_RUN_ARGS}
        COMMAND vme_unit_tests ${TEST_RUN_ARGS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        WORKING_DIRECTORY
        DEPENDS all_binaries
        )
