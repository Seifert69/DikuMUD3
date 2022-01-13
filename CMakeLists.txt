cmake_minimum_required(VERSION 3.16.3)
project(DikuMUD3)

find_package(BISON)
find_package(FLEX)

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
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    # Stronger warnings for debug build
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fpermissive -fdiagnostics-color=always -Wformat=2 -Wformat-overflow=2 -Wall")
else ()
    # Hide some warnings for a release build
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fpermissive -fdiagnostics-color=always -Wno-register -Wno-deprecated-declarations")
endif ()
message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin)

link_directories(${CMAKE_SOURCE_DIR}/vme/bin)

add_subdirectory(vme/src)
add_subdirectory(vme/src/defcomp)
add_subdirectory(vme/src/mplex2)
add_subdirectory(vme/src/pp)
add_subdirectory(vme/src/vmc)
add_subdirectory(vme/zone)
add_subdirectory(vme/etc)

enable_testing()
add_subdirectory(unit_tests)
