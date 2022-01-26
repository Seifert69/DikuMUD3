project(mplex2)

set(MPLEX_SRCS
        ClientConnector.cpp ClientConnector.h
        MUDConnector.cpp MUDConnector.h
        echo_server.cpp echo_server.h
        mplex.cpp mplex.h
        network.cpp network.h
        telnet.h
        translate.cpp translate.h
        ttydef.h
        # Source files from other parts to link with mplex
        ../bytestring.cpp ../bytestring.h
        ../color.cpp ../color.h
        ../hook.cpp ../hook.h
        ../membug.cpp ../membug.h
        ../protocol.cpp ../protocol.h
        ../queue.cpp ../queue.h
        ../slog_raw.cpp ../slog_raw.h
        ../textutil.cpp ../textutil.h
        ../utility.cpp ../utility.h
        ../OutputCapture.cpp ../OutputCapture.h
        )


# Add all sources to a library file so we can link with other projects and test suites
add_library(mplex_objs STATIC ${MPLEX_SRCS})
target_compile_definitions(mplex_objs PUBLIC LINUX POSIX MPLEX_COMPILE)
target_include_directories(mplex_objs
        PRIVATE . ${CMAKE_SOURCE_DIR}/vme/src
        PUBLIC ${CMAKE_SOURCE_DIR}/vme/include
        )

add_executable(mplex main.cpp)
target_compile_definitions(mplex PUBLIC LINUX POSIX MPLEX_COMPILE)
target_link_libraries(mplex
        mplex_objs
        Boost::system
        pthread
        )
target_include_directories(mplex
        PRIVATE . ${CMAKE_SOURCE_DIR}/vme/src
        PUBLIC ${CMAKE_SOURCE_DIR}/vme/include
        )
