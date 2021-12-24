project(vmc)

set(VMC_SRCS
        dilpar.h
        dilshare.cpp
        diltok.h
        vmc.cpp vmc.h
        vmc_process.cpp
        vmctok.h
        )

# Source files from other parts to link with vme
set(VMC_DEP_SRCS
        ../bytestring.cpp ../bytestring.h
        ../color.cpp ../color.h
        ../common.cpp ../common.h
        ../db_file.cpp ../db_file.h
        ../destruct.cpp ../destruct.h
        ../extra.cpp ../extra.h
        ../files.cpp ../files.h
        ../intlist.cpp ../intlist.h
        ../membug.cpp ../membug.h
        ../money.cpp ../money.h
        ../namelist.cpp ../namelist.h
        ../spec_assign.cpp ../spec_assign.h
        ../structs.cpp ../structs.h
        ../textutil.cpp ../textutil.h
        ../utility.cpp ../utility.h
        )

include_directories(.)

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

add_compile_definitions(VMC_SRC VMC MUD_DEBUG)

add_executable(vmc vmc.cpp)

add_library(vmc_objs STATIC
        ${VMC_SRCS}
        ${BISON_dilpar_OUTPUTS}
        ${FLEX_dillex_OUTPUTS}
        ${BISON_vmcpar_OUTPUTS}
        ${FLEX_vmclex_OUTPUTS}
        )

#Add all other project sources to a library file so we can link with subject projects and test suites
add_library(vmc_dep_objs STATIC ${VMC_DEP_SRCS})

target_link_libraries(vmc
        vmc_objs
        pp_objs
        vmc_dep_objs
        ${FLEX_LIBRARIES}
        )
