Run this command:

for file in *_prompt.txt; do name="${file%_prompt.txt}"; if [[ ! -f "${name}.wiki" ]]; then echo "$file"; break; fi; done

Then read the .txt file, which is based on this automated extract from DIKUMUD DIL. Then lookup any DIL manual files
that are relevant to understand the function, procedure, or field being described - the DIL manual files might be incorrect as
they are very dated.

Do NOT create any output about YACC: only use YACC understand the DIL function, procedure, or field.
Do NOT create any output about C implementation: only use the C code to understand the DIL function, procedure, or field, and to make relevant notes to the user.

Finally with all this knowledge create a high quality WikiMedia markdown article for the function, procedure, or field described in the .txt file. Output and save the article as a .wiki file with the same name as the function in question.

Wiki Markdown

== Heading Level 2 ==
=== Heading Level 3 ===
==== Heading Level 4 ====

'''Bold text'''
''Italic text''
'''''Bold italic text'''''

 Preformatted text / code block
 (leading space on each line)
  Example line 1
  Example line 2

:Indented line (1 level)
::Indented line (2 levels)
:::Indented line (3 levels)

{| class="wikitable"
! Header 1 !! Header 2 !! Header 3
|-
| Cell 1-1 || Cell 1-2 || Cell 1-3
|-
| Cell 2-1 || Cell 2-2 || Cell 2-3
|-
| Cell 3-1 || Cell 3-2 || Cell 3-3
|}

The function heading must be 
= equipment =

Do NOT use these for markdown:
# something  -> instead use =
{{something}}
<pre>something</pre> -> instead use space for preformatted text. Do NOT add blank lines in code blocks / preformatted text.

Here are help files form the DIL manual in the folder:
 acc_modify.hlp       findsymbolic.hlp         on_activation.hlp     sfb_msg.hlp
 act.hlp              findunit.hlp             openroll.hlp          sfb_pre.hlp
 activator.hlp        fits.hlp                 opponent.hlp          sfb_tick.hlp
 addaff.hlp           flog.hlp                 pagestring.hlp        shell.hlp
 addcolor.hlp         follow.hlp               pathto.hlp            skill_name.hlp
 addequip.hlp         foreach.hlp              paycheck.hlp          spellindex.hlp
 addextra.hlp         gamestate.hlp            position_update.hlp   spellinfo.hlp
 addstring.hlp        getaffects.hlp           power.hlp             split.hlp
 argument.hlp         getcolor.hlp             priority.hlp          stop_fighting.hlp
 asctime.hlp          getword.hlp              purse.hlp             store.hlp
 atoi.hlp             getwords.hlp             quit.hlp              strcmp.hlp
 attack_spell.hlp     ghead.hlp                realtime.hlp          strdir.hlp
 beginedit.hlp        goto.hlp                 reboot.hlp            string.hlp
 block.hlp            heartbeat.hlp            remove.hlp            stringlist.hlp
 break.hlp            if.hlp                   replace.hlp           strncmp.hlp
 cancarry.hlp         in.hlp                   reset_level.hlp       subaff.hlp
 cast_spell.hlp       insert.hlp               reset_race.hlp        subextra.hlp
 change_speed.hlp     integer.hlp              reset_vlevel.hlp      substring.hlp
 changecolor.hlp      integerlist.hlp          restore.hlp           target.hlp
 check_password.hlp   interrupt.hlp            return.hlp            textformat.hlp
 clear.hlp            isaff.hlp                right.hlp             tolower.hlp
 cmdptr.hlp           islight.hlp              rnd.hlp               toupper.hlp
 cmdstr.hlp           isplayer.hlp             savestr.hlp           transfermoney.hlp
 command.hlp          isset.hlp                secure.hlp            unequip.hlp
 continue.hlp         itoa.hlp                 self.hlp              unit_st_npc.hlp
 delcolor.hlp         killedit.hlp             send.hlp              unit_st_obj.hlp
 delete_player.hlp    left.hlp                 send_done.hlp        'unit_st_pc and unit_st_npc.hlp'
 delstr.hlp           length.hlp               send_pre.hlp          unit_st_pc.hlp
 delunit.hlp          link.hlp                 sendtext.hlp          unit_st_room.hlp
 destroy.hlp          load.hlp                 sendto.hlp            unitdir.hlp
 dilcopy.hlp          loadstr.hlp              sendtoall.hlp         unitptr.hlp
 dildestroy.hlp       log.hlp                  sendtoalldil.hlp      unsecure.hlp
 dilfind.hlp          logcrime.hlp             set.hlp               unset.hlp
 equipment.hlp        medium.hlp               set_fighting.hlp      visible.hlp
 excmdstr.hlp         meleeattack.hlp          set_password.hlp      wait.hlp
 excmdstr_case.hlp    meleedamage.hlp          set_weight.hlp        walkto.hlp
 exec.hlp             mid.hlp                  set_weight_base.hlp   weapon_info.hlp
 experience.hlp       moneystring.hlp          setbright.hlp         weapon_name.hlp
 extraptr.hlp        'mud(day,month,..).hlp'   sfb_cmd.hlp           weather.hlp
 filesize.hlp         nopriority.hlp           sfb_com.hlp           while.hlp
 findrndunit.hlp      null.hlp                 sfb_dead.hlp          zoneptr.hlp
 findroom.hlp         on_..._goto_....hlp      sfb_done.hlp

Here are code extracts on disk from YACC & C++  :

abilities_prompt.txt       fighting_prompt.txt       medium_prompt.txt           sact_prompt.txt
ability_costs_prompt.txt   findrndunit_prompt.txt    meleeattack_prompt.txt      savestr_prompt.txt
ability_levels_prompt.txt  findroom_prompt.txt       meleedamage_prompt.txt      secure_prompt.txt
ability_points_prompt.txt  findsymbolic_prompt.txt   mid_prompt.txt              self_prompt.txt
acc_balance_prompt.txt     findunit_prompt.txt       minv_prompt.txt             send_done_prompt.txt
acc_modify_prompt.txt      findzone_prompt.txt       moneystring_prompt.txt      send_pre_prompt.txt
acc_total_prompt.txt       fits_prompt.txt           movement_prompt.txt         send_prompt.txt
access_prompt.txt          flags_prompt.txt          mudday_prompt.txt           sendtext_prompt.txt
act_prompt.txt             flog_prompt.txt           mudhour_prompt.txt          sendto_prompt.txt
activator_prompt.txt       fname_prompt.txt          mudmonth_prompt.txt         sendtoall_prompt.txt
addaff_prompt.txt          follow_prompt.txt         mudyear_prompt.txt          sendtoalldil_prompt.txt
addcolor_prompt.txt        follower_prompt.txt       name_prompt.txt             set_fighting_prompt.txt
addequip_prompt.txt        followercount_prompt.txt  nameidx_prompt.txt          set_password_prompt.txt
addextra_prompt.txt        foreach_prompt.txt        names_prompt.txt            set_prompt.txt
addstring_prompt.txt       full_prompt.txt           natural_armour_prompt.txt   set_weight_base_prompt.txt
alignment_prompt.txt       gamestate_prompt.txt      next_prompt.txt             set_weight_prompt.txt
argument_prompt.txt        getaffects_prompt.txt     nopriority_prompt.txt       setbright_prompt.txt
asctime_prompt.txt         getcmd_prompt.txt         notes_prompt.txt            sex_prompt.txt
atoi_prompt.txt            getcolor_prompt.txt       npc_head_prompt.txt         shell_prompt.txt
attack_spell_prompt.txt    getfollower_prompt.txt    npccount_prompt.txt         skill_costs_prompt.txt
attack_type_prompt.txt     getinteger_prompt.txt     npcflags_prompt.txt         skill_levels_prompt.txt
baseweight_prompt.txt      getopponent_prompt.txt    npcs_prompt.txt             skill_name_prompt.txt
beginedit_prompt.txt       getword_prompt.txt        null_prompt.txt             skill_points_prompt.txt
birth_prompt.txt           getwords_prompt.txt       obj_head_prompt.txt         skills_prompt.txt
block_prompt.txt           ghead_prompt.txt          objcount_prompt.txt         speed_prompt.txt
bright_prompt.txt          global_head_prompt.txt    objectflags_prompt.txt      spell_costs_prompt.txt
can_carry_prompt.txt       gnext_prompt.txt          objecttype_prompt.txt       spell_levels_prompt.txt
capacity_prompt.txt        gprevious_prompt.txt      objs_prompt.txt             spellindex_prompt.txt
cast_spell_prompt.txt      guild_prompt.txt          offensive_prompt.txt        spellinfo_prompt.txt
change_speed_prompt.txt    hasfunc_prompt.txt        on_prompt.txt               spells_prompt.txt
changecolor_prompt.txt     heartbeat_prompt.txt      opendiff_prompt.txt         split_prompt.txt
charflags_prompt.txt       height_prompt.txt         openflags_prompt.txt        stop_fighting_prompt.txt
chead_prompt.txt           help_prompt.txt           openroll_prompt.txt         store_prompt.txt
check_password_prompt.txt  hometown_prompt.txt       opponent_prompt.txt         strcmp_prompt.txt
clear_prompt.txt           hp_prompt.txt             opponentcount_prompt.txt    strdir_prompt.txt
clone_prompt.txt           idx_prompt.txt            outside_descr_prompt.txt    strncmp_prompt.txt
cmdstr_prompt.txt          if_prompt.txt             outside_prompt.txt          subaff_prompt.txt
command_head_prompt.txt    illum_prompt.txt          pagestring_prompt.txt       subextra_prompt.txt
command_prompt.txt         info_prompt.txt           pathto_prompt.txt           substring_prompt.txt
cost_prompt.txt            insert_prompt.txt         paycheck_prompt.txt         switched_prompt.txt
creators_prompt.txt        inside_descr_prompt.txt   payonly_prompt.txt          symname_prompt.txt
crimes_prompt.txt          inside_prompt.txt         pc_head_prompt.txt          target_prompt.txt
defaultpos_prompt.txt      interrupt_prompt.txt      pcflags_prompt.txt          textformat_prompt.txt
defensive_prompt.txt       isaff_prompt.txt          playtime_prompt.txt         thirst_prompt.txt
delcolor_prompt.txt        islight_prompt.txt        position_prompt.txt         title_prompt.txt
delete_player_prompt.txt   isplayer_prompt.txt       position_update_prompt.txt  tolower_prompt.txt
delstr_prompt.txt          isset_prompt.txt          power_prompt.txt            toupper_prompt.txt
delunit_prompt.txt         itoa_prompt.txt           priority_prompt.txt         transfermoney_prompt.txt
descr_prompt.txt           key_prompt.txt            profession_prompt.txt       type_prompt.txt
destroy_prompt.txt         killedit_prompt.txt       prompt_prompt.txt           unequip_prompt.txt
dilcall_prompt.txt         lastroom_prompt.txt       purse_prompt.txt            unitdir_prompt.txt
dilcopy_prompt.txt         left_prompt.txt           quests_prompt.txt           unsecure_prompt.txt
dildestroy_prompt.txt      length_prompt.txt         quit_prompt.txt             unset_prompt.txt
dilfind_prompt.txt         level_prompt.txt          race_prompt.txt             vals_prompt.txt
dispatch_prompt.txt        lifespan_prompt.txt       realtime_prompt.txt         value_prompt.txt
drunk_prompt.txt           light_prompt.txt          reboot_prompt.txt           visible_prompt.txt
editing_prompt.txt         link_prompt.txt           remove_prompt.txt           vlevel_prompt.txt
endurance_prompt.txt       load_prompt.txt           rent_prompt.txt             wait_prompt.txt
equip_prompt.txt           loadcount_prompt.txt      replace_prompt.txt          waitnoop_prompt.txt
equipment_prompt.txt       loadlevel_prompt.txt      reset_level_prompt.txt      weapon_costs_prompt.txt
excmdstr_case_prompt.txt   loadstr_prompt.txt        reset_race_prompt.txt       weapon_info_prompt.txt
excmdstr_prompt.txt        log_prompt.txt            reset_vlevel_prompt.txt     weapon_levels_prompt.txt
exec_prompt.txt            logcrime_prompt.txt       resetmode_prompt.txt        weapon_name_prompt.txt
exit_diff_prompt.txt       loglevel_prompt.txt       resettime_prompt.txt        weapons_prompt.txt
exit_info_prompt.txt       mana_prompt.txt           restore_prompt.txt          weather_prompt.txt
exit_key_prompt.txt        manipulate_prompt.txt     return_prompt.txt           weight_prompt.txt
exit_names_prompt.txt      mapx_prompt.txt           right_prompt.txt            zhead_prompt.txt
exit_to_prompt.txt         mapy_prompt.txt           rnd_prompt.txt              zone_head_prompt.txt
exp_prompt.txt             master_prompt.txt         room_head_prompt.txt        zone_prompt.txt
experience_prompt.txt      max_endurance_prompt.txt  roomcount_prompt.txt        zoneidx_prompt.txt
exptol_prompt.txt          max_hp_prompt.txt         roomflags_prompt.txt        zonereset_prompt.txt
extra_prompt.txt           max_mana_prompt.txt       rooms_prompt.txt