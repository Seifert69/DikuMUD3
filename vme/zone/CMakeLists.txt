file(GLOB VME_ZONES "*.zon")

# Create empty variable for outputs
set(OUTPUT_FILES)

foreach (VME_SRC_ZONE ${VME_ZONES})
    get_filename_component(SHORT "${VME_SRC_ZONE}" NAME_WLE)

    set(OUTPUT_DATA_FILE ${SHORT}.data)

    add_custom_command(
            OUTPUT ${OUTPUT_DATA_FILE}
            COMMAND vmc -I../include ${SHORT}.zon
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/zone
            DEPENDS ${SHORT}.zon ../include/*.h
            VERBATIM
    )

    # Append new .data file to outputs list
    set(OUTPUT_FILES ${OUTPUT_FILES} ${OUTPUT_DATA_FILE})
endforeach (VME_SRC_ZONE)

add_custom_target(zone_files ALL DEPENDS vmc ${OUTPUT_FILES})