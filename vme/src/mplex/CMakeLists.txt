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
        ../account.cpp ../account.h
        ../act_change.cpp
        ../act_color.cpp
        ../act_info.cpp
        ../act_jstat.cpp
        ../act_movement.cpp
        ../act_offensive.cpp
        ../act_other.cpp ../act_other.h
        ../act_skills.cpp
        ../act_wizard.cpp
        ../act_wstat.cpp
        ../affect.cpp ../affect.h
        ../apf_affect.cpp ../apf_affect.h
        ../badnames.cpp ../badnames.h
        ../ban.cpp ../ban.h
        ../bank.cpp
        ../bytestring.cpp ../bytestring.h
        ../char_data.cpp ../char_data.h
        ../char_follow_type.cpp ../char_follow_type.h
        ../char_point_data.cpp ../char_point_data.h
        ../cmdload.cpp ../cmdload.h
        ../color.cpp ../color.h
        ../combat.cpp ../combat.h
        ../comm.cpp ../comm.h
        ../common.cpp ../combat.h
        ../config.cpp ../config.h
        ../constants.cpp ../constants.h
        ../convert.cpp
        ../db.cpp ../db.h
        ../db_file.cpp ../db_file.h
        ../dbfind.cpp ../dbfind.h
        ../descriptor_data.cpp ../descriptor_data.h
        ../destruct.cpp ../destruct.h
        ../dictionary.cpp
        ../dilexp.cpp ../dilexp.h
        ../dilfld.cpp
        ../dilinst.cpp ../dilinst.h
        ../dilrun.cpp ../dilrun.h
        ../dilshare.cpp ../dilshare.h
        ../dilsup.cpp
        ../eliza.cpp
        ../event.cpp ../event.h
        ../experience.cpp ../experience.h
        ../extra.cpp ../extra.h
        ../fight.cpp ../fight.h
        ../file_index_type.cpp ../file_index_type.h
        ../files.cpp ../files.h
        ../guild.cpp ../guild.h
        ../handler.cpp ../handler.h
        ../hook.cpp ../hook.h
        ../hookmud.cpp ../hookmud.h
        ../interpreter.cpp ../interpreter.h
        ../intlist.cpp ../intlist.h
        ../json_helper.cpp ../json_helper.h
        ../justice.cpp ../justice.h
        ../magic.cpp ../magic.h
        ../main_functions.cpp
        ../membug.cpp ../membug.h
        ../mobact.cpp ../mobact.h
        ../modify.cpp ../modify.h
        ../money.cpp ../modify.h
        ../namelist.cpp ../namelist.h
        ../nanny.cpp ../nanny.h
        ../nice.cpp
        ../npc_data.cpp ../npc_data.h
        ../obj_data.cpp ../obj_data.h
        ../OutputCapture.cpp ../OutputCapture.h
        ../path.cpp
        ../pc_account_data.cpp ../pc_account_data.h
        ../pc_data.cpp ../pc_data.h
        ../pc_time_data.cpp ../pc_time_data.h
        ../pcsave.cpp ../pcsave.h
        ../pipe.cpp ../pipe.h
        ../protocol.cpp ../protocol.h
        ../queue.cpp ../queue.h
        ../reception.cpp ../reception.h
        ../room_data.cpp ../room_data.h
        ../room_direction_data.cpp ../room_direction_data.h
        ../sector.cpp
        ../signals.cpp
        ../skills.cpp ../skills.h
        ../slime.cpp ../slime.h
        ../slog_raw.cpp ../slog_raw.h
        ../snoop_data.cpp ../snoop_data.h
        ../spec_assign.cpp ../spec_assign.h
        ../spec_procs.cpp ../spec_procs.h
        ../spell_parser.cpp ../spell_parser.h
        ../str_parse.cpp ../str_parse.h
        ../system.cpp ../system.h
        ../szonelog_raw.cpp ../szonelog_raw.h
        ../teach.cpp ../teach.h
        ../textutil.cpp ../textutil.h
        ../textutil.cpp ../textutil.h
        ../tif_affect.cpp ../tif_affect.h
        ../trie.cpp ../trie.h
        ../unit_affected_type.cpp ../unit_affected_type.h
        ../unit_data.cpp ../unit_data.h
        ../unit_fptr.cpp ../unit_fptr.h
        ../unitfind.cpp ../unitfind.h
        ../utility.cpp ../utility.h
        ../vmelimits.cpp ../vmelimits.h
        ../weather.cpp ../weather.h
        ../zon_basis.cpp ../zon_basis.h
        ../zon_wiz.cpp
        ../zone_reset.cpp ../zone_reset.h
        ../zone_reset_cmd.cpp ../zone_reset_cmd.h
        ../zone_type.cpp ../zone_type.h
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
        crypt
        Boost::system
        Boost::regex
        Boost::filesystem
        pthread
        )
target_include_directories(mplex
        PRIVATE . ${CMAKE_SOURCE_DIR}/vme/src
        PUBLIC ${CMAKE_SOURCE_DIR}/vme/include
        )

if (DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "Diku MUD III - mplex")
    set(DOXYGEN_HTML_OUTPUT ".")
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/mplex)
    doxygen_add_docs(
            mplex_docs
            ${MPLEX_SRCS} main.cpp
            ALL
            COMMENT "Building MPLEX docs"
    )
endif ()
