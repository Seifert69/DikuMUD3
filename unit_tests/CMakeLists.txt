include_directories(${Boost_INCLUDE_DIRS})

find_package(OpenSSL)

############################### VME ######################################
add_executable(vme_unit_tests
        vme_main.cpp
        CServerConfiguration_tests.cpp
        FixtureBase.cpp FixtureBase.h
        account_cpp_tests.cpp
        cNamelist_tests.cpp
        color_type_tests.cpp
        weather_cpp_tests.cpp
        )
target_link_options(vme_unit_tests PUBLIC -Wl,-zmuldefs)
target_compile_definitions(vme_unit_tests PUBLIC
        DMSERVER
        RAPIDJSON_HAS_STDSTRING
        )
target_link_libraries(vme_unit_tests
        vme_objs
        crypt
        ${Boost_LIBRARIES}
        pthread
        )
target_include_directories(vme_unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/vme/src)
# Add the test for cmake
add_test(NAME vme_unit_tests
        COMMAND vme_unit_tests --log_level=all
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        )

############################### VMC ######################################
add_executable(vmc_unit_tests
        vmc_main.cpp
        ChecksumDataset.cpp ChecksumDataset.h
        sha512.cpp sha512.h
        )
target_compile_definitions(vmc_unit_tests PUBLIC
        VMC_SRC
        VMC
        MUD_DEBUG
        RAPIDJSON_HAS_STDSTRING
        PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
        )
target_link_libraries(vmc_unit_tests
        vmc_objs
        pp_objs
        ${Boost_LIBRARIES}
        OpenSSL::Crypto
        )
# Add the test for cmake
add_test(NAME vmc_unit_tests
        COMMAND vmc_unit_tests --log_level=all
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        )

############################### PP #######################################
add_executable(pp_unit_tests
        pp_main.cpp
        )
target_compile_definitions(pp_unit_tests PUBLIC
        DMSERVER
        RAPIDJSON_HAS_STDSTRING
        )
target_link_libraries(pp_unit_tests
        pp_objs
        ${Boost_LIBRARIES}
        )
# Add the test for cmake
add_test(NAME pp_unit_tests
        COMMAND pp_unit_tests --log_level=all
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        )

############################### MPLEX ####################################
add_executable(mplex_unit_tests
        mplex_main.cpp
        )
target_compile_definitions(mplex_unit_tests PUBLIC
        DMSERVER
        RAPIDJSON_HAS_STDSTRING
        )
target_link_libraries(mplex_unit_tests
        mplex_objs
        ${Boost_LIBRARIES}
        )
# Add the test for cmake
add_test(NAME mplex_unit_tests
        COMMAND mplex_unit_tests --log_level=all
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        )

############################### DEFCOMP ##################################
add_executable(defcomp_unit_tests
        defcomp_main.cpp
        ChecksumDataset.cpp ChecksumDataset.h
        sha512.cpp sha512.h
        )
target_compile_definitions(defcomp_unit_tests PUBLIC
        DMSERVER
        RAPIDJSON_HAS_STDSTRING
        PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
        )
target_link_libraries(defcomp_unit_tests
        defcomp_objs
        ${Boost_LIBRARIES}
        OpenSSL::Crypto
        )
# Add the test for cmake
add_test(NAME defcomp_unit_tests
        COMMAND defcomp_unit_tests --log_level=all
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/bin
        )
