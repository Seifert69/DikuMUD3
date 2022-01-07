project(pp)

set(PP_SRCS
        main.cpp
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

add_executable(pp main.cpp)

# Add all sources to a library file so we can link with subject projects and test suites
add_library(pp_objs STATIC ${PP_SRCS})

target_link_libraries(pp pp_objs)
