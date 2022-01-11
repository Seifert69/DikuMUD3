set(Boost_USE_STATIC_LIBS OFF)

include_directories(${Boost_INCLUDE_DIRS})

add_custom_target(all_unit_tests)
add_dependencies(all_unit_tests
        vme
        vme_unit_tests
        vmc_unit_tests
        pp_unit_tests
        defcomp_unit_tests
        mplex_unit_tests
        run_vme_unit_test
        run_vmc_unit_test
        run_pp_unit_test
        run_mplex_unit_test
        run_defcomp_unit_test
        )

############################### VME ######################################
add_executable(vme_unit_tests
        vme_main.cpp
        CServerConfiguration_tests.cpp
        color_type_tests.cpp
        cNamelist_tests.cpp
        #Adding vme main for globals - multidef takes care of 2 main() funcs
        ../vme/src/main.cpp
        )
# Add the test for cmake
add_test(vme_unit_tests
        COMMAND vme_unit_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        )
# Run the test as part of build
add_custom_target(run_vme_unit_test ALL
        COMMAND vme_unit_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        DEPENDS vme_unit_tests
        )
target_compile_definitions(vme_unit_tests PUBLIC
        ${COMPILE_DEFINITIONS}
        DMSERVER
        )
target_link_options(vme_unit_tests PUBLIC -Wl,-zmuldefs)
target_link_libraries(vme_unit_tests
        vme_objs
        crypt
        ${Boost_LIBRARIES}
        pthread
        )

############################### VMC ######################################
add_executable(vmc_unit_tests
        vmc_main.cpp
        )
# Add the test for cmake
add_test(vmc_unit_tests
        COMMAND vmc_unit_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        )
# Run the test as part of build
add_custom_target(run_vmc_unit_test ALL
        COMMAND vmc_unit_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        DEPENDS vmc_unit_tests
        )
target_compile_definitions(vmc_unit_tests PUBLIC
        ${COMPILE_DEFINITIONS}
        VMC_SRC
        VMC MUD_DEBUG
        )
target_link_libraries(vmc_unit_tests
        vmc_objs
        vmc_dep_objs
        pp_objs
        ${Boost_LIBRARIES}
        )

############################### PP #######################################
add_executable(pp_unit_tests
        pp_main.cpp
        )
# Add the test for cmake
add_test(pp_unit_tests
        COMMAND pp_unit_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        )
# Run the test as part of build
add_custom_target(run_pp_unit_test ALL
        COMMAND pp_unit_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        DEPENDS pp_unit_tests
        )
target_link_libraries(pp_unit_tests
        pp_objs
        ${Boost_LIBRARIES}
        )

############################### MPLEX ####################################
add_executable(mplex_unit_tests
        mplex_main.cpp
        )
# Add the test for cmake
add_test(mplex_unit_tests
        COMMAND mplex_unit_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        )
# Run the test as part of build
add_custom_target(run_mplex_unit_test ALL
        COMMAND mplex_unit_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        DEPENDS mplex_unit_tests
        )
target_link_libraries(mplex_unit_tests
        mplex_objs
        mplex_dep_objs
        ${Boost_LIBRARIES}
        )

############################### DEFCOMP ##################################
add_executable(defcomp_unit_tests
        defcomp_main.cpp
        )
# Add the test for cmake
add_test(defcomp_unit_tests
        COMMAND defcomp_unit_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        )
# Run the test as part of build
add_custom_target(run_defcomp_unit_test ALL
        COMMAND defcomp_unit_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        DEPENDS defcomp_unit_tests
        )
target_link_libraries(defcomp_unit_tests
        ${Boost_LIBRARIES}
        )
