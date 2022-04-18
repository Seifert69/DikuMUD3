project(defcomp)

set(DC_SRCS
        defcomp.cpp defcomp.h
        )

add_library(defcomp_objs STATIC ${DC_SRCS})
target_compile_definitions(defcomp_objs PUBLIC LINUX POSIX)

add_executable(defcomp main.cpp)
target_compile_definitions(defcomp PUBLIC LINUX POSIX)
target_link_libraries(defcomp defcomp_objs)

if (DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "Diku MUD III - defcomp")
    set(DOXYGEN_HTML_OUTPUT ".")
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/defcomp)
    doxygen_add_docs(
            defcomp_docs
            ${DC_SRCS} main.cpp
            ALL
            COMMENT "Building DEFCOMP docs"
    )
endif ()
