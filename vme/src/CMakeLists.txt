set(VME_SRCS
        account.cpp account.h
        act.h
        act_change.cpp
        act_color.cpp
        act_info.cpp
        act_movement.cpp
        act_offensive.cpp
        act_other.cpp
        act_skills.cpp
        act_wizard.cpp
        act_wstat.cpp
        affect.cpp affect.h
        affectdil.h
        apf_affect.cpp
        badnames.cpp badnames.h
        ban.cpp ban.h
        bank.cpp bank.h
        base.h
        bytestring.cpp bytestring.h
        cmdload.cpp cmdload.h
        color.cpp color.h
        combat.cpp combat.h
        comm.cpp comm.h
        common.cpp common.h
        config.cpp config.h
        constants.cpp constants.h
        convert.cpp
        db.cpp db.h
        db_file.cpp db_file.h
        db_utils.cpp
        dbfind.cpp dbfind.h
        destruct.cpp destruct.h
        dictionary.cpp
        diku_exception.h
        dil.h
        dilexp.cpp dilexp.h
        dilfld.cpp
        dilinst.cpp dilinst.h
        dilrun.cpp dilrun.h
        dilshare.cpp
        dilsup.cpp dilsup.h
        eliza.cpp
        essential.h
        event.cpp event.h
        experience.cpp
        external_vars.h
        extra.cpp extra.h
        fight.cpp fight.h
        files.cpp files.h
        guild.cpp guild.h
        guilddef.h
        handler.cpp handler.h
        hook.cpp hook.h
        hookmud.cpp hookmud.h
        interpreter.cpp interpreter.h
        intlist.cpp intlist.h
        justice.cpp justice.h
        limits.cpp
        liquid.h
        magic.cpp magic.h
        main.h
        membug.cpp membug.h
        mobact.cpp mobact.h
        modify.cpp modify.h
        money.cpp money.h
        movement.h
        namelist.cpp namelist.h
        nanny.cpp
        nice.cpp
        path.cpp path.h
        pcsave.cpp
        pipe.cpp
        protocol.cpp protocol.h
        queue.cpp queue.h
        reception.cpp
        sector.cpp sector.h
        signals.cpp
        skills.cpp skills.h
        slime.cpp
        spec_assign.cpp spec_assign.h
        spec_procs.cpp
        spell_parser.cpp
        spelldef.h
        spells.h
        str_parse.cpp str_parse.h
        structs.cpp structs.h
        system.cpp system.h
        teach.cpp
        textutil.cpp textutil.h
        tif_affect.cpp
        trie.cpp trie.h
        unitfind.cpp unitfind.h
        utility.cpp utility.h
        utils.h
        values.h
        vme.h
        vmelimits.h
        weather.cpp weather.h
        wmacros.h
        zon_basis.cpp
        zon_wiz.cpp
        zone_reset.cpp
        )

add_compile_definitions(DMSERVER)

# Add all sources to a library file so we can link with subject projects and test suites
add_library(vme_objs STATIC ${VME_SRCS})

add_executable(vme main.cpp)

target_link_libraries(vme
        vme_objs
        crypt
        Boost::filesystem
        Boost::regex
        pthread
        )
