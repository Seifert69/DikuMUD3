set(DEF_FILES abilities.def commands.def professions.def races.def sector.def spells.def weapons.def)
file(GLOB HEADER_DEPS ${CMAKE_SOURCE_DIR}/vme/include/*.h)

# Create empty variable for outputs
set(OUTPUT_FILES)

foreach (DEF_SRC_FILE ${DEF_FILES})
    get_filename_component(SHORT "${DEF_SRC_FILE}" NAME_WLE)

    set(OUTPUT_DATA_FILE "${SHORT}.dat")

    add_custom_command(
            OUTPUT ${OUTPUT_DATA_FILE}
            COMMAND perl ${CMAKE_SOURCE_DIR}/vme/bin/defconvert.perl ${SHORT}.def ${SHORT}.dat 
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/etc
            DEPENDS ${SHORT}.def ${HEADER_DEPS}
            VERBATIM
    )

    # Append new .data file to outputs list
    set(OUTPUT_FILES ${OUTPUT_FILES} ${OUTPUT_DATA_FILE})
    set(CLEAN_FILES ${CLEAN_FILES} "${SHORT}.dat;")
endforeach (DEF_SRC_FILE)

add_custom_target(dat_files ALL DEPENDS vmc ${HEADER_DEPS} ${OUTPUT_FILES})

set_target_properties(dat_files
        PROPERTIES
            ADDITIONAL_CLEAN_FILES "${CLEAN_FILES}"
        )


#
# Handle the color file. I'm sure this isn't a good way to handle a single file...
#

set(COLOR_FILES color.def)

# Create empty variable for outputs
set(OUTPUT_FILES)
set(CLEAN_FILES)

foreach (DEF_SRC_FILE ${COLOR_FILES})
    get_filename_component(SHORT "${DEF_SRC_FILE}" NAME_WLE)

    set(OUTPUT_DATA_FILE "${SHORT}.dat")

    add_custom_command(
            OUTPUT ${OUTPUT_DATA_FILE}
            COMMAND ${CMAKE_SOURCE_DIR}/vme/bin/defcomp -c color
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/etc
            DEPENDS color.def
            VERBATIM
    )

    # Append new .data file to outputs list
    set(OUTPUT_FILES ${OUTPUT_FILES} ${OUTPUT_DATA_FILE})
    set(CLEAN_FILES ${CLEAN_FILES} "${SHORT}.dat;")
endforeach (DEF_SRC_FILE)

add_custom_target(color_files ALL DEPENDS ${CMAKE_SOURCE_DIR}/vme/bin/defcomp ${OUTPUT_FILES})

set_target_properties(color_files
        PROPERTIES
            ADDITIONAL_CLEAN_FILES "${CLEAN_FILES}"
        )
