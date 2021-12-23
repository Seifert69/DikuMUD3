set (Boost_USE_STATIC_LIBS OFF)

include_directories (${Boost_INCLUDE_DIRS})

set(VME vme)
set(VMC vmc)
set(PP pp)
set(DEFCOMP defcomp)
set(MPLEX mplex)

add_custom_target(all_unit_tests)
add_dependencies(vme ${VME}_unit_tests ${VMC}_unit_tests ${PP}_unit_tests ${DEFCOMP}_unit_tests ${MPLEX}_unit_tests)


############################### VME ######################################
add_executable(${VME}_unit_tests ${VME}_main.cpp)
target_compile_definitions(${VME}_unit_tests PUBLIC ${COMPILE_DEFINITIONS} DMSERVER)
target_link_libraries(${VME}_unit_tests ${Boost_LIBRARIES} ${VME}_objs)

############################### VMC ######################################
add_executable(${VMC}_unit_tests vmc_main.cpp)
target_compile_definitions(${VMC}_unit_tests PUBLIC ${COMPILE_DEFINITIONS} VMC_SRC VMC MUD_DEBUG)
target_link_libraries(${VMC}_unit_tests
        ${Boost_LIBRARIES}
        ${VMC}_objs
        ${VMC}_dep_objs
        ${PP}_objs
        )

############################### PP #######################################
add_executable(${PP}_unit_tests ${PP}_main.cpp)
target_link_libraries (${PP}_unit_tests ${Boost_LIBRARIES} ${PP}_objs)

############################### MPLEX ####################################
add_executable(${MPLEX}_unit_tests ${MPLEX}_main.cpp)
target_link_libraries(${MPLEX}_unit_tests
        ${Boost_LIBRARIES}
        ${MPLEX}_objs
        ${MPLEX}_dep_objs)

############################### DEFCOMP ##@###############################
add_executable(${DEFCOMP}_unit_tests ${DEFCOMP}_main.cpp)
target_link_libraries(${DEFCOMP}_unit_tests ${Boost_LIBRARIES})
