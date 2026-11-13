file(GLOB VME_ZONES "*.zon")
# Every zone recompiles when a shared header or vmc itself changes, not only when its .zon does
file(GLOB HEADER_DEPS "${CMAKE_SOURCE_DIR}/vme/include/*.h")

# Create empty variable for outputs
set(OUTPUT_FILES)
#set(CLEAN_FILES)

foreach (VME_SRC_ZONE ${VME_ZONES})
    get_filename_component(SHORT "${VME_SRC_ZONE}" NAME_WLE)

    set(OUTPUT_DATA_FILE "${SHORT}.data;${SHORT}.reset")

    add_custom_command(
            OUTPUT ${OUTPUT_DATA_FILE}
            COMMAND vmc -q -d ${CMAKE_SOURCE_DIR}/vme/etc/ -I${CMAKE_SOURCE_DIR}/vme/include/ ${SHORT}.zon
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/zone
            DEPENDS ${SHORT}.zon ${HEADER_DEPS} vmc
            VERBATIM
    )

    # Append new .data file to outputs list
    set(OUTPUT_FILES ${OUTPUT_FILES} ${OUTPUT_DATA_FILE})
    set(CLEAN_FILES ${CLEAN_FILES} "${SHORT}.dh;${SHORT}.err;${SHORT}.dot;")
endforeach (VME_SRC_ZONE)

# The example zones of the .claude/skills must keep compiling, warnings included. vmc writes
# .err next to the source, so it compiles a copy in the build tree, and -s writes no .data.
file(GLOB SKILL_ZONES "${CMAKE_SOURCE_DIR}/.claude/skills/*/example.zon")
foreach (SKILL_ZONE ${SKILL_ZONES})
    get_filename_component(SKILL_DIR "${SKILL_ZONE}" DIRECTORY)
    get_filename_component(SKILL "${SKILL_DIR}" NAME)
    set(SKILL_OUT "${CMAKE_CURRENT_BINARY_DIR}/skill_examples/${SKILL}")
    file(MAKE_DIRECTORY ${SKILL_OUT})

    add_custom_command(
            OUTPUT ${SKILL_OUT}/example.ok
            COMMAND ${CMAKE_COMMAND} -E copy ${SKILL_ZONE} ${SKILL_OUT}/example.zon
            COMMAND vmc -q -s -l -d ${CMAKE_SOURCE_DIR}/vme/etc/ -I${CMAKE_SOURCE_DIR}/vme/include/ example.zon
            COMMAND ${CMAKE_COMMAND} -E touch ${SKILL_OUT}/example.ok
            WORKING_DIRECTORY ${SKILL_OUT}
            DEPENDS ${SKILL_ZONE} ${HEADER_DEPS} vmc
            VERBATIM
    )
    set(OUTPUT_FILES ${OUTPUT_FILES} ${SKILL_OUT}/example.ok)
endforeach (SKILL_ZONE)

add_custom_target(zone_files ALL DEPENDS ${CMAKE_SOURCE_DIR}/vme/bin/vmc ${OUTPUT_FILES})

set_target_properties(zone_files
        PROPERTIES
            ADDITIONAL_CLEAN_FILES "${CLEAN_FILES}"
        )
