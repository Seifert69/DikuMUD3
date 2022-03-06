set(VME_SRCS
        account.cpp account.h
        act_change.cpp act_change.h
        act_color.cpp act_color.h
        act_info.cpp act_info.h
        act_movement.cpp act_movement.h
        act_offensive.cpp act_offensive.h
        act_other.cpp act_other.h
        act_skills.cpp act_skills.h
        act_wizard.cpp act_wizard.h
        act_wstat.cpp act_wstat.h
        affect.cpp affect.h
        affectdil.h
        apf_affect.cpp apf_affect.h
        badnames.cpp badnames.h
        ban.cpp ban.h
        bank.cpp bank.h
        bytestring.cpp bytestring.h
        cmdload.cpp cmdload.h
        color.cpp color.h
        combat.cpp combat.h
        comm.cpp comm.h
        common.cpp common.h
        compile_defines.h
        config.cpp config.h
        constants.cpp constants.h
        convert.cpp convert.h
        db.cpp db.h
        db_file.cpp db_file.h
        dbfind.cpp dbfind.h
        descriptor_data.cpp descriptor_data.h
        destruct.cpp destruct.h
        dictionary.cpp dictionary.h
        diku_exception.h
        dil.h
        dilexp.cpp dilexp.h
        dilfld.cpp
        dilinst.cpp dilinst.h
        dilrun.cpp dilrun.h
        dilshare.cpp dilshare.h
        dilsup.cpp dilsup.h
        eliza.cpp eliza.h
        error.h
        essential.h
        event.cpp event.h
        experience.cpp experience.h
        extra.cpp extra.h
        fight.cpp fight.h
        file_index_type.cpp file_index_type.h
        files.cpp files.h
        formatter.h
        guild.cpp guild.h
        handler.cpp handler.h
        hook.cpp hook.h
        hookmud.cpp hookmud.h
        interpreter.cpp interpreter.h
        intlist.cpp intlist.h
        justice.cpp justice.h
        magic.cpp magic.h
        main.h
        main_functions.cpp main_functions.h
        membug.cpp membug.h
        mobact.cpp mobact.h
        modify.cpp modify.h
        money.cpp money.h
        movement.h
        namelist.cpp namelist.h
        nanny.cpp nanny.h
        OutputCapture.cpp OutputCapture.h
        nice.cpp nice.h
        path.cpp path.h
        pcsave.cpp pcsave.h
        pipe.cpp pipe.h
        protocol.cpp protocol.h
        queue.cpp queue.h
        reception.cpp reception.h
        sector.cpp sector.h
        signals.cpp signals.h
        skills.cpp skills.h
        slime.cpp slime.h
        slog.h
        slog_raw.cpp slog_raw.h
        snoop_data.cpp snoop_data.h
        spec_assign.cpp spec_assign.h
        spec_procs.cpp spec_procs.h
        spell_parser.cpp spell_parser.h
        spells.h
        str_parse.cpp str_parse.h
        structs.cpp structs.h
        system.cpp system.h
        szonelog.h
        szonelog_raw.cpp szonelog_raw.h
        teach.cpp teach.h
        textutil.cpp textutil.h
        tif_affect.cpp tif_affect.h
        time_info_data.cpp time_info_data.h
        trie.cpp trie.h
        unit_affected_type.cpp unit_affected_type.h
        unit_dil_affected_type.cpp unit_dil_affected_type.h
        unit_fptr.cpp unit_fptr.h
        unitfind.cpp unitfind.h
        utility.cpp utility.h
        utils.h
        vmelimits.cpp vmelimits.h
        weather.cpp weather.h
        zon_basis.cpp zon_basis.h
        zon_wiz.cpp zon_wiz.h
        zone_reset.cpp zone_reset.h
        zone_reset_cmd.cpp zone_reset_cmd.h
        zone_type.cpp zone_type.h
        )

# Add all sources to a library file so we can link with other projects and test suites
add_library(vme_objs STATIC ${VME_SRCS})
target_compile_definitions(vme_objs PUBLIC DMSERVER LINUX POSIX)
target_include_directories(vme_objs
        PRIVATE .
        PUBLIC ${CMAKE_SOURCE_DIR}/vme/include)

add_executable(vme main.cpp)
target_compile_definitions(vme PUBLIC DMSERVER LINUX POSIX)
target_include_directories(vme_objs PUBLIC ${CMAKE_SOURCE_DIR}/vme/include)
target_link_libraries(vme
        vme_objs
        crypt
        Boost::filesystem
        Boost::regex
        pthread
        )
