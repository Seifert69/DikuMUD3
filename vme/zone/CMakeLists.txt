file(GLOB VME_ZONES "*.zon")

# Create empty variable for outputs
set(OUTPUT_FILES)

foreach (VME_SRC_ZONE ${VME_ZONES})
    set(OUTPUT_DATA_FILE "${VME_SRC_ZONE}.data")

    add_custom_command(
            OUTPUT ${OUTPUT_DATA_FILE}
            COMMAND vmc -m -I../include ${VME_SRC_ZONE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vme/zone
            DEPENDS ${VME_SRC_ZONE}
            VERBATIM
    )

    # Append new .data file to outputs list
    set(OUTPUT_FILES ${OUTPUT_FILES} ${OUTPUT_DATA_FILE})
endforeach (VME_SRC_ZONE)

add_custom_target(zone_files ALL DEPENDS vmc ${OUTPUT_FILES})
