#!/bin/bash

# https://clang.llvm.org/extra/clang-tidy/checks/readability-braces-around-statements.html

which clang-tidy-12 > /dev/null 2>&1
if [ $? -ne 0 ]; then
   echo -e "\n\nERROR: clang-tidy needs to be installed"
   echo -e "\n    sudo apt-get install clang-tidy-12\n\n"
   exit 1
fi

sources="vme/src/account.cpp vme/src/act_change.cpp vme/src/act_color.cpp vme/src/act_info.cpp vme/src/act_movement.cpp vme/src/act_offensive.cpp vme/src/act_other.cpp vme/src/act_skills.cpp vme/src/act_wizard.cpp vme/src/act_wstat.cpp vme/src/affect.cpp vme/src/apf_affect.cpp vme/src/badnames.cpp vme/src/ban.cpp vme/src/bank.cpp vme/src/bytestring.cpp vme/src/cmdload.cpp vme/src/color.cpp vme/src/combat.cpp vme/src/comm.cpp vme/src/common.cpp vme/src/config.cpp vme/src/constants.cpp vme/src/convert.cpp vme/src/db.cpp vme/src/db_file.cpp vme/src/dbfind.cpp vme/src/defcomp/defcomp.cpp vme/src/defcomp/main.cpp vme/src/destruct.cpp vme/src/dictionary.cpp vme/src/dilexp.cpp vme/src/dilfld.cpp vme/src/dilinst.cpp vme/src/dilrun.cpp vme/src/dilshare.cpp vme/src/dilsup.cpp vme/src/eliza.cpp vme/src/event.cpp vme/src/experience.cpp vme/src/extra.cpp vme/src/fight.cpp vme/src/files.cpp vme/src/guild.cpp vme/src/handler.cpp vme/src/hook.cpp vme/src/hookmud.cpp vme/src/interpreter.cpp vme/src/intlist.cpp vme/src/justice.cpp vme/src/magic.cpp vme/src/main.cpp vme/src/main_functions.cpp vme/src/membug.cpp vme/src/mobact.cpp vme/src/modify.cpp vme/src/money.cpp vme/src/mplex2/ClientConnector.cpp vme/src/mplex2/MUDConnector.cpp vme/src/mplex2/echo_server.cpp vme/src/mplex2/main.cpp vme/src/mplex2/mplex.cpp vme/src/mplex2/network.cpp vme/src/mplex2/translate.cpp vme/src/namelist.cpp vme/src/nanny.cpp vme/src/nice.cpp vme/src/path.cpp vme/src/pcsave.cpp vme/src/pipe.cpp vme/src/protocol.cpp vme/src/queue.cpp vme/src/reception.cpp vme/src/sector.cpp vme/src/signals.cpp vme/src/skills.cpp vme/src/slime.cpp vme/src/spec_assign.cpp vme/src/spec_procs.cpp vme/src/spell_parser.cpp vme/src/str_parse.cpp vme/src/structs.cpp vme/src/system.cpp vme/src/teach.cpp vme/src/textutil.cpp vme/src/tif_affect.cpp vme/src/trie.cpp vme/src/unitfind.cpp vme/src/utility.cpp vme/src/vmc/main.cpp vme/src/vmc/vmc.cpp vme/src/vmc/vmc_process.cpp vme/src/vmelimits.cpp vme/src/weather.cpp vme/src/zon_basis.cpp vme/src/zon_wiz.cpp vme/src/zone_reset.cpp"

# Create clean fresh compile_commands.json file for clang-tidy
mkdir tidy_comp_db
pushd tidy_comp_db
cmake ..
popd

for source in $sources
do
   echo "Selecting [$source] as next victim for braces!"
   clang-tidy-12 -p tidy_comp_db --checks="-*,readability-braces-around-statements" --fix $source || exit 1
   clang-format -i $source
done

echo -e "\n\nFINISHED!\nYou can delete $0 and 'tidy_comp_db' now"
echo -e "\n     rm -rf tidy_comp_db"
echo -e "     rm $0\n\n"
