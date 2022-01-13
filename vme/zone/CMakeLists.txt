file(GLOB VME_ZONES "*.zon")
file(GLOB HEADER_DEPS "{CMAKE_SOURCE_DIR}/vme/include/*.h")

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
            DEPENDS ${SHORT}.zon ${HEADER_DEPS}
            VERBATIM
    )

    # Append new .data file to outputs list
    set(OUTPUT_FILES ${OUTPUT_FILES} ${OUTPUT_DATA_FILE})
    set(CLEAN_FILES ${CLEAN_FILES} "${SHORT}.dh;${SHORT}.err;${SHORT}.dot;")
endforeach (VME_SRC_ZONE)

add_custom_target(zone_files ALL DEPENDS ${CMAKE_SOURCE_DIR}/vme/bin/vmc ${OUTPUT_FILES})

set_target_properties(zone_files
        PROPERTIES
            ADDITIONAL_CLEAN_FILES "${CLEAN_FILES}"
        )
