project(pp)

set(PP_SRCS
        pp.h
        pp1.cpp
        pp2.cpp
        pp3.cpp
        pp4.cpp
        pp5.cpp
        pp6.cpp
        pp7.cpp
        pp8.cpp
        )

# Add all sources to a library file so we can link with other projects and test suites
add_library(pp_objs STATIC ${PP_SRCS})
target_compile_definitions(pp_objs PUBLIC LINUX POSIX)

add_executable(pp main.cpp)
target_compile_definitions(pp PUBLIC LINUX POSIX)
target_link_libraries(pp pp_objs)
