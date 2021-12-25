project(mplex2)

set(MPLEX_SRCS
        ClientConnector.cpp ClientConnector.h
        MUDConnector.cpp MUDConnector.h
        echo_server.cpp
        mplex.cpp mplex.h
        network.cpp network.h
        telnet.h
        translate.cpp translate.h
        ttydef.h
        )

# Source files from other parts to link with mplex
set(MPLEX_DEP_SRCS
        ../bytestring.cpp ../bytestring.h
        ../color.cpp ../color.h
        ../hook.cpp ../hook.h
        ../membug.cpp ../membug.h
        ../protocol.cpp ../protocol.h
        ../queue.cpp ../queue.h
        ../textutil.cpp ../textutil.h
        ../utility.cpp ../utility.h
        )
include_directories(.)

add_executable(mplex mplex.cpp)

# Add all sources to a library file so we can link with subject projects and test suites
add_library(mplex_objs STATIC ${MPLEX_SRCS})

#Add all other project sources to a library file so we can link with subject projects and test suites
add_library(mplex_dep_objs STATIC ${MPLEX_DEP_SRCS})

target_link_libraries(mplex
        mplex_objs
        mplex_dep_objs
        Boost::system
        pthread
        )
