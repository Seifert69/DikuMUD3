project(defcomp)

set(DC_SRCS
        defcomp.cpp defcomp.h
        )

add_library(defcomp_objs STATIC ${DC_SRCS})
target_compile_definitions(defcomp_objs PUBLIC LINUX POSIX)

add_executable(defcomp main.cpp)
target_compile_definitions(defcomp PUBLIC LINUX POSIX)
target_link_libraries(defcomp defcomp_objs)
