project(vmc)

set(VMC_SRCS
        dilpar.h
        diltok.h
        vmc.cpp vmc.h
        vmc_process.cpp vmc_process.h
        vmctok.h
        # Source files from other parts to link with vmc
        ../bytestring.cpp ../bytestring.h
        ../char_data.cpp ../char_data.h
        ../color.cpp ../color.h
        ../common.cpp ../common.h
        ../db_file.cpp ../db_file.h
        ../destruct.cpp ../destruct.h
        ../dilshare.cpp ../dil.h
        ../extra.cpp ../extra.h
        ../files.cpp ../files.h
        ../intlist.cpp ../intlist.h
        ../membug.cpp ../membug.h
        ../money.cpp ../money.h
        ../namelist.cpp ../namelist.h
        ../npc_data.cpp ../npc_data.h
        ../obj_data.cpp ../obj_data.h
        ../OutputCapture.cpp ../OutputCapture.h
        ../pc_data.cpp ../pc_data.h
        ../room_data.cpp ../room_data.h
        ../spec_assign.cpp ../spec_assign.h
        ../textutil.cpp ../textutil.h
        ../unit_data.cpp ../unit_data.h
        ../utility.cpp ../utility.h
        )

BISON_TARGET(vmcpar vmcpar.y
        ${CMAKE_CURRENT_BINARY_DIR}/tmp_vmcpar.cpp
        COMPILE_FLAGS "--debug -y -d -v"
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp_vmcpar.h
        REPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp_vmcpar.output
        )
FLEX_TARGET(vmclex
        vmclex.l
        ${CMAKE_CURRENT_BINARY_DIR}/tmp_vmclex.cpp
        )
ADD_FLEX_BISON_DEPENDENCY(vmclex vmcpar)

BISON_TARGET(dilpar dilpar.y
        ${CMAKE_CURRENT_BINARY_DIR}/tmp_dilpar.cpp
        COMPILE_FLAGS "--debug -d -v -pdil"
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp_dilpar.h
        REPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp_dilpar.output
        )
FLEX_TARGET(dillex
        dillex.l
        ${CMAKE_CURRENT_BINARY_DIR}/tmp_dillex.cpp
        COMPILE_FLAGS "-Pdil"
        )
ADD_FLEX_BISON_DEPENDENCY(dillex dilpar)

add_library(vmc_objs STATIC
        ${VMC_SRCS}
        ${BISON_dilpar_OUTPUTS}
        ${FLEX_dillex_OUTPUTS}
        ${BISON_vmcpar_OUTPUTS}
        ${FLEX_vmclex_OUTPUTS}
        )
target_compile_definitions(vmc_objs PUBLIC VMC_SRC VMC MUD_DEBUG)
target_include_directories(vmc_objs
        PRIVATE . ${CMAKE_SOURCE_DIR}/vme/src ${CMAKE_SOURCE_DIR}/vme/src/pp
        PUBLIC ${CMAKE_SOURCE_DIR}/vme/include
        )

# Add report files to clean target
set_target_properties(vmc_objs
        PROPERTIES
            ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_BINARY_DIR}/tmp_vmcpar.output;${CMAKE_CURRENT_BINARY_DIR}/tmp_dilpar.output;"
        )


add_executable(vmc main.cpp)
target_compile_definitions(vmc PUBLIC VMC_SRC VMC MUD_DEBUG)
target_link_options(vmc PUBLIC -Wl,-zmuldefs) # TODO Come back and see if this can be removed
target_link_libraries(vmc
        vmc_objs
        pp_objs
        vme_objs
        crypt
        Boost::filesystem
        Boost::regex
        pthread
        )
target_include_directories(vmc
        PRIVATE . ${CMAKE_SOURCE_DIR}/vme/src ${CMAKE_SOURCE_DIR}/vme/src/pp
        PUBLIC ${CMAKE_SOURCE_DIR}/vme/include
        )
if (DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "Diku MUD III - vmc")
    set(DOXYGEN_HTML_OUTPUT ".")
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/vmc)
    doxygen_add_docs(
            vmc_docs
            ${VMC_SRCS} main.cpp
            ALL
            COMMENT "Building VMC docs"
    )
endif ()
