/* *********************************************************************** *
 * File   : guilddef.h                                Part of Valhalla MUD *
 * Version: 1.01                                                           *
 * Author : seifert, drevar & sk(r)ay                                      *
 *                                                                         *
 * Status : Published.                                                     *
 *                                                                         *
 * Copyright (C) 1994 - 1996 by Valhalla (This work is published).         *
 *                                                                         *
 * This work is copyrighted. No part of this work may be copied,           *
 * reproduced, translated or reduced to any medium without the prior       *
 * written consent of Valhalla.                                            *
 * *********************************************************************** */

#if defined(_GUILD_UDGAARD_THIEF)

/* Each guild can have at most 2 stats at 100, it can have 2 cheap stats,
   2 average stats and the rest above average. */

#if defined(_ABILITY)
special SFUN_TEACH_INIT
"&labilities;0;
$1n tells you, 'I have never heard of such an ability.';
$1n tells you, 'I do not know how to teach this ability.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d ability points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you.';
$1n tells you, 'Remove all equipment, please.';

  0;   70; Strength                      ;   9;  9000;  10;       0;
  0;  100; Dexterity                     ;   4;  4000;   8;       0;
  0;   80; Constitution                  ;  14; 14000;  12;       0;
  0;   60; Hitpoints                     ;  14; 14000;  11;       0;
  0;  100; Charisma                      ;  14; 14000;  10;       0;
  0;   70; Brain                         ;  14; 14000;   9;       0;
  5;   40; Magic                         ;  23; 23000;  14;       0;"

/* Sum (max) 520, Average = 10.57 */

#elif defined(_SKILL)

special SFUN_TEACH_INIT
"&lskills;0;
$1n tells you, 'I have never heard of such a skill.';
$1n tells you, 'I do not know how to teach this skill.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d skill points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;   50; backstab                      ;   9;  9000;   8;          0;
  0;   80; consider                      ;   4;  4000;   5; 10;      0;
  0;  100; appraisal                     ;   4;  4000;   5;          0;
  0;  100; ventriloquate                 ;   4;  4000;   5;          0;
  0;  100; fleeing                       ;   4;  4000;   5; 10;      0;
  0;  100; hide                          ;   9;  9000;   8;          0;
  0;  100; search                        ;   4;  4000;   5;          0;
  0;  100; climb                         ;   9;  9000;   8;          0;
  0;  100; pick pockets                  ;   4;  4000;   8; 16;      0;
  1;  100; sneak                         ;   9;  9000;   8;          0;
  2;  100; pick lock                     ;   4;  4000;   8; 16;      0;
  3;   70; shield                        ;  14; 14000;  10;          0;
  4;   70; diagnostics                   ;   4;  4000;   5; 10;      0;
  5;  100; filching                      ;   4;  4000;   5; 10;      0;
  6;   50; swimming                      ;  14; 14000;  10;          0;
  7;  100; tripping                      ;   9;  9000;  10;          0;
  8;  100; peeking                       ;   4;  4000;  10; 20;      0;
  9;   30; first aid                     ;  14; 14000;  10;          0;
 10;  100; stealing                      ;   4;  4000;  10; 20;      0;
 12;   70; disarming                     ;  14; 14000;  12;          0;
 15;   50; scroll use                    ;  23; 23000;  20;          0;
 19;   70; two weapon style              ;   9;  9000;  10;          0;
"
#elif defined(_WEAPON)

special SFUN_TEACH_INIT
"&lweapons;0;
$1n tells you, 'I have never heard of such a weapon.';
$1n tells you, 'I do not know how to teach that weapon.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d skill points.';
$1n tells you, 'I can not teach you any more.';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;   80; dagger                        ;   4;  4000;   5;          0;
  2;   80; short sword                   ;   4;  4000;   8;          0;
  4;   80; club                          ;   9;  9000;  10;          0;
  4;   80; fist                          ;  15; 15000;  15;          0;
  8;   80; rapier                        ;  12; 12000;   8;          0;
  8;   80; whip                          ;  18; 18000;  15;          0;
 12;   80; scimitar                      ;  14; 14000;  10;          0;
 20;   70; long sword                    ;  18; 18000;  15;          0;
"
#elif defined(_SPELL)

special SFUN_TEACH_INIT
"&lspells;0;
$1n tells you, 'I have never heard of such a way to use magic.';
$1n tells you, 'I do not know how to teach this sort of magic.';
$1n tells you, 'You have not got %s for me.';
$1n tells you, 'You have not got %d skill points.';
$1n tells you, 'I can not teach you any more, you are my equal in this.';
$1n tells you, 'You must be without magical aura in order to learn.';
$1n tells you, 'You must remove all that you wear.';

 35;   50; quicken                       ;  23; 23000;  20;          0;
 40;   50; lock                          ;  23; 23000;  20;          0;
 35;   50; unlock                        ;  18; 18000;  15;          0;
"
#else
NO TYPE FOR THE THIEF GUILD ERROR
#endif





#elif defined(_GUILD_UDGAARD_FIGHTER)

#if defined(_ABILITY)

special SFUN_TEACH_INIT
"&labilities;0;
$1n tells you, 'I have never heard of such an ability.';
$1n tells you, 'I do not know how to teach this ability.';
$1n tells you, 'You havn't got %s for me.';
$1n tells you, 'You havn't got %d ability points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach
you.';
$1n tells you, 'Remove all equipment, please.';

  0;  100; Strength                      ;   4;  4000;   8;       0;
  0;   90; Dexterity                     ;  14; 14000;  12;       0;
  0;   90; Constitution                  ;  14; 14000;   9;       0;
  0;  100; Hitpoints                     ;   4;  4000;  11;       0;
  2;   60; Brain                         ;  23; 23000;  14;       0;
  4;   80; Charisma                      ;  18; 18000;  14;       0;
"

/* Sum (max) 520, Average = 9.3 */

#elif defined(_SKILL)

special SFUN_TEACH_INIT
"&lskills;0;
$1n tells you, 'I have never heard of such a skill.';
$1n tells you, 'I do not know how to teach this skill.';
$1n tells you, 'You havn't got %s for me.';
$1n tells you, 'You havn't got %d skill points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;  100; consider                      ;  14; 14000;  10; 20;      0;
  0;  100; diagnostics                   ;  14; 14000;  10; 20;      0;
  0;   70; fleeing                       ;  14; 14000;  10;          0;
  0;  100; bash                          ;   9;  9000;   8;          0;
  0;  100; shield                        ;   4;  4000;   5; 10;      0;
  1;   70; rescue                        ;  14; 14000;  10;          0;
 10;  100; disarming                     ;   9;  9000;   8;          0;
 15;   50; leadership                    ;   9;  9000;   8;          0;
"
#elif defined(_WEAPON)

special SFUN_TEACH_INIT
"&lweapons;0;
$1n tells you, 'I have never heard of such a weapon.';
$1n tells you, 'I do not know how to teach that weapon.';
$1n tells you, 'You havn't got %s for me.';
$1n tells you, 'You havn't got %d skill points.';
$1n tells you, 'I can not teach you any more.';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;  100; fist                          ;   9;  9000;   8;          0;
  0;  100; dagger                        ;   4;  4000;   5;          0;
  0;  100; club                          ;   4;  4000;   5;          0;
  0;   80; whip                          ;   4;  4000;   5;          0;
  0;  100; hand axe                      ;   9;  9000;   8;          0;
  0;  100; quarterstaff                  ;   9;  9000;   8;          0;
  1;  100; rapier                        ;   9;  9000;   8;          0;
  2;  100; spear                         ;   9;  9000;   8;          0;
  4;  100; scimitar                      ;   9;  9000;   8;          0;
  4;   80; kick                          ;   4;  4000;  12;          0;
  4;  100; mace                          ;   9;  9000;   8;          0;
  4;  100; short sword                   ;   9;  9000;   8;          0;
  8;  100; war hammer                    ;   9;  9000;   8;          0;
  8;  100; broad sword                   ;   9;  9000;   8;          0;
 12;  100; morning star                  ;  14;  9000;  10;          0;
 14;  100; flail                         ;  14; 14000;  10;          0;
 16;   90; long sword                    ;   9;  9000;   8;          0;
 16;   70; trident                       ;  18; 18000;  15;          0;
 16;  100; battle club                   ;  14; 14000;  10;          0;
 18;  100; battle mace                   ;  14; 14000;  10;          0;
 18;  100; bardiche                      ;  18; 18000;  15;          0;
 22;  100; battle axe                    ;  14; 14000;  15;          0;
 25;  100; war mattock                   ;  18; 18000;  15;          0;
 30;  100; halberd                       ;  18; 18000;  15;          0;
"
#else
ERROR: NO TYPE FOR FIGHTER GUILD
#endif

#elif defined(_GUILD_UDGAARD_CONJURER)

#if defined(_ABILITY)

special SFUN_TEACH_INIT
"&labilities;0;
$1n tells you, 'I have never heard of such an ability.';
$1n tells you, 'I do not know how to teach this ability.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d ability points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach
you.';
$1n tells you, 'Remove all equipment, please.';

  2;   50; Strength                      ;  23; 23000;  13;    0;
  1;   60; Dexterity                     ;  18; 18000;  13;    0;
  2;   60; Constitution                  ;  14; 14000;  10;    0;
  2;   60; Hitpoints                     ;  18; 18000;  14;    0;
  0;  100; Charisma                      ;   9;  9000;  10;    0;
  0;   90; Brain                         ;   4;  4000;   8;    0;
  0;  100; Magic                         ;   4;  4000;   8;    0;
"
/* Sum (max) 520, Average = 10.85 */

#elif defined(_SKILL)

special SFUN_TEACH_INIT
"&lskills;0;
$1n tells you, 'I have never heard of such a skill.';
$1n tells you, 'I do not know how to teach this skill.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d skill points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;   80; consider                      ;   4;  4000;   5;          0;
  0;   80; diagnostics                   ;   4;  4000;   5;          0;
  0;  100; scroll use                    ;   4;  4000;   5; 10;      0;
  0;  100; wand use                      ;   4;  4000;   5; 10;      0;
  0;   60; fleeing                       ;  14; 14000;  10;          0;
"
#elif defined(_WEAPON)

special SFUN_TEACH_INIT
"&lweapons;0;
$1n tells you, 'I have never heard of such a weapon.';
$1n tells you, 'I do not know how to teach that weapon.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d skill points.';
$1n tells you, 'I can not teach you any more.';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;   80; dagger                        ;  18; 18000;  15;          0;
  8;   60; quarterstaff                  ;  18; 18000;  15;          0;
  4;   60; fist                          ;  23; 23000;  18;          0;
"
#elif defined(_SPELL)

special SFUN_TEACH_INIT
"&lspells;0;
$1n tells you, 'I have never heard of such magic.';
$1n tells you, 'I do not know how to teach this kind of magic.';
$1n tells you, 'You have not got %s for me.';
$1n tells you, 'You have not got %d skill points.';
$1n tells you, 'I can not teach you any more, find a better teacher!';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;  100; detect magic                  ;   4;  4000;   5; 10;      0;
  0;  100; raise magic                   ;   9;  9000;   8;          0;
  0;  100; energy bolt                   ;   9;  9000;   5; 10;      0;
  2;  100; brain                         ;   4;  4000;   5; 10;      0;
  3;  100; magic candle                  ;   9;  9000;   5; 10;      0;
  6;  100; dexterity                     ;  14; 14000;  10; 20;      0;
  8;  100; constitution                  ;  14; 14000;  10; 20;      0;
  8;  100; leaving                       ;  14; 14000;  10;          0;
  9;  100; dust devil                    ;  14; 14000;  10;          0;
 10;  100; strength                      ;  18; 18000;  15;          0;
 10;  100; detect invisible              ;  14; 14000;  10; 20;      0;
 11;  100; identify                      ;   9;  9000;   8; 16;      0;
 12;  100; storm call                    ;   9;  9000;   8;          0;
 13;  100; clenched fist                 ;   9;  9000;   8; 16;      0;
 14;   80; mind shield                   ;  14; 14000;  10; 20;      0;
 14;  100; lock                          ;  14; 14000;  10;          0;
 14;  100; summon earth                  ;   9;  9000;   10;         0;
 15;  100; quicken                       ;  14; 14000;  10;          0;
 15;   80; slow                          ;  18; 18000;  15;          0;
 15;   80; heat resistance               ;   4;  4000;   5; 10;      0;
 16;   80; cold resistance               ;  14; 14000;  10; 20;      0;
 17;  100; transport                     ;   9;  9000;   8;          0;
 17;   80; electricity resistance        ;  14; 14000;  10; 20;      0;
 18;  100; sun globe                     ;   9;  9000;   10;         0;
 18;   80; poison resistance             ;  14; 14000;  10; 20;      0;
 18;  100; summon air                    ;   9;  9000;   10;         0;
 19;   80; acid resistance               ;  14; 14000;  10; 20;      0;
 20;  100; invisibility                  ;   9;  9000;   8; 16;      0;
 21;  100; improved identify             ;  14; 14000;  10; 20;      0;
 22;  100; random teleport               ;  14; 14000;  10;          0;
 23;  100; wizard eye                    ;   9;  9000;   8;          0;
 24;  100; summon water                  ;   9;  9000;   10;         0;
 27;  100; unlock                        ;  14; 14000;  10;          0;
 27;  100; minor gate                    ;   9;  9000;   8; 16;      0;
 30;  100; summon fire                   ;   9;  9000;   10;         0;
 30;  100; dispel magic                  ;  18; 18000;  15;          0;
 30;  100; haste                         ;  18; 18000;  15;          0;
 32;  100; cone of shards                 ;   9;  9000;   10; 20;     0;
 35;  100; controlled teleport           ;  14; 14000;  10;          0;
 35;  100; gate                          ;  14; 14000;  10;          0;
 40;  100; summon demon                  ;   9;  9000;  10;          0;
 50;  100; summon devil                  ;   9;  9000;  10;          0;
"
#else
ERROR NO TYPE FOR THE CONJURER GUILD
#endif

#elif defined(_GUILD_UDGAARD_HEALER)

#if defined(_ABILITY)

special SFUN_TEACH_INIT
"&labilities;0;
$1n tells you, 'I have never heard of such an ability.';
$1n tells you, 'I do not know how to teach this ability.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d ability points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach
you.';
$1n tells you, 'Remove all equipment, please.';

  0;   50; Strength                      ;  14; 14000;  14;       0;
  0;   50; Dexterity                     ;  14; 14000;  10;       0;
  0;   70; Constitution                  ;  14; 14000;  13;       0;
  0;   60; Hitpoints                     ;  18; 18000;  13;       0;
  0;  100; Charisma                      ;   4;  4000;  10;       0;
  0;   90; Brain                         ;   9;  9000;   8;       0;
  0;  100; Divine                        ;   4;  4000;   8;       0;
"

/* Sum (max) 520, Average = 10.86 */

#elif defined(_SKILL)

special SFUN_TEACH_INIT
"&lskills;0;
$1n tells you, 'I have never heard of such a skill.';
$1n tells you, 'I do not know how to teach this skill.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d skill points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;  100; diagnostics                   ;   4;  4000;   5;          0;
  0;   60; fleeing                       ;  14; 14000;  10;          0;
  0;  100; first aid                     ;   4;  4000;   5; 10;      0;
  2;  100; turn undead                   ;   4;  4000;   5; 10;      0;
  3;   80; scroll use                    ;   9;  9000;   8;          0;
  3;   80; consider                      ;   9;  9000;   8;          0;
  3;   80; shield                        ;  14; 14000;  10;          0;
"
#elif defined(_WEAPON)

special SFUN_TEACH_INIT
"&lweapons;0;
$1n tells you, 'I have never heard of such a weapon.';
$1n tells you, 'I do not know how to teach that weapon.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d skill points.';
$1n tells you, 'I can not teach you any more.';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  4;   70; war hammer                    ;  14; 14000;  10;          0;
  0;   70; mace                          ;  14; 14000;  10;          0;
  8;   60; quarterstaff                  ;  14; 14000;  10;          0;
  1;   80; fist                          ;  18; 18000;  15;          0;
 20;   70; morning star                  ;  18; 18000;  15;          0;
"

#elif defined(_SPELL)

special SFUN_TEACH_INIT
"&lspells;0;
$1n tells you, 'I have never heard of such magic.';
$1n tells you, 'I do not know how to teach this kind of magic.';
$1n tells you, 'You have not got %s for me.';
$1n tells you, 'You have not got %d skill points.';
$1n tells you, 'I can not teach you any more, find a better teacher!';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;  100; cure light wounds             ;   4;  4000;   5; 10;      0;
  1;   80; cause light wounds            ;  14; 14000;  10;          0;
  2;  100; aid                           ;  14; 14000;  10;          0;
  4;  100; summer rain                   ;  14; 14000;  10;          0;
  4;  100; raise divine                  ;   4;  4000;   5; 10;      0;
  4;  100; divine resistance             ;   4;  4000;   5; 10;      0;
  4;   75; command                       ;  14; 14000;  10; 20;      0;
  5;  100; detect alignment              ;   9;  9000;   8; 16;      0;
  5;  100; detect undead                 ;   9;  9000;   8; 16;      0;
  6;  100; detect poison                 ;   9;  9000;   8;          0;
  6;  100; charisma                      ;   4;  4000;   5; 10;      0;
  6;  100; sustain                       ;   9;  9000;   8; 16;      0;
  7;  100; awaken                        ;   9;  9000;   8;          0;
  8;   50; blind                         ;  14; 14000;  10;          0;
  8;  100; leather skin                  ;   9;  9000;   8;          0;
  9;  100; bless                         ;  14; 14000;  10;          0;
 10;  100; word of recall                ;  14; 14000;  10;          0;
 10;  100; sanctuary                     ;  14; 14000;  10; 20;      0;
 11;  100; detect curse                  ;   9;  9000;   8; 16;      0;
 11;  100; remove disease                ;  14; 14000;  10;          0;
 12;  100; light                         ;   4;  4000;   5; 10;      0;
 13;  100; remove poison                 ;  14; 14000;  10;          0;
 14;  100; sense life                    ;  18; 18000;  15; 30;      0;
 14;  100; cure serious wounds           ;   9;  9000;   8; 16;      0;
 15;  100; cure blind                    ;  14; 14000;  10; 20;      0;
 16;  100; create water                  ;   9;  9000;   8; 16;      0;
 16;  100; bark skin                     ;  14; 14000;  10;          0;
 17;  100; life protection               ;  14; 14000;  10;          0;
 18;  100; find path                     ;  14; 14000;  10; 20;      0;
 18;  100; remove curse                  ;   9;  9000;   8; 16;      0;
 18;  100; create food                   ;  14; 14000;  10; 20;      0;
 19;  100; locate object                 ;  18; 18000;  15;          0;
 20;  100; locate person                 ;  14; 14000;  10;          0;
 20;  100; total recall                  ;  18; 18000;  15;          0;
 22;  100; protection from evil          ;  14; 14000;  10;          0;
 23;   50; hold undead                   ;  23; 23000;  20;          0;
 24;  100; continual light               ;  14; 14000;  10;          0;
 27;  100; raise dead                    ;  18; 18000;  15;          0;
 28;   80; cause serious wounds          ;  18; 18000;  15;          0;
 29;  100; dispel evil                   ;  14; 14000;  10;          0;
 30;  100; heal                          ;  14; 14000;  10;          0;
 31;   80; sun ray                       ;  14; 14000;  10;          0;
 33;  100; clear skies                   ;  18; 18000;  15;          0;
 37;   80; harm                          ;  14; 14000;  20;          0;
 40;  100; resurrection                  ;  23; 23000;  20;          0;
"
#else
ERROR IN HEALER GUILD, NO TYPE
#endif

#elif defined(_GUILD_NECROMANCER)

#if defined(_ABILITY)

special SFUN_TEACH_INIT
"&labilities;0;
$1n tells you, 'I have never heard of such an ability.';
$1n tells you, 'I do not know how to teach this ability.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d ability points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach
you.';
$1n tells you, 'Remove all equipment, please.';

  1;   50; Strength                      ;  23; 23000;  13;      0;
  0;   60; Dexterity                     ;  18; 18000;  13;      0;
  0;   60; Constitution                  ;  14; 14000;  14;      0;
  2;   60; Hitpoints                     ;  18; 18000;  10;      0;
  0;   90; Charisma                      ;  14; 14000;  10;      0;
  0;  100; Brain                         ;   4;  4000;   8;      0;
  0;  100; Divine                        ;   4;  4000;   8;      0;
"
/* Sum (max) 520, Average = 10.86 */

#elif defined(_SKILL)

special SFUN_TEACH_INIT
"&lskills;0;
$1n tells you, 'I have never heard of such a skill.';
$1n tells you, 'I do not know how to teach this skill.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d ability points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;   60; fleeing                       ;  14; 14000;  10;          0;
  1;   80; wand use                      ;  14; 14000;  10;          0;
  2;   80; scroll use                    ;   9;  9000;   8;          0;
  3;   80; leadership                    ;   9;  9000;  10;          0;
  5;   75; diagnostics                   ;   4;  4000;   5;          0;

"

#elif defined(_WEAPON)

special SFUN_TEACH_INIT
"&lweapons;0;
$1n tells you, 'I have never heard of such a weapon.';
$1n tells you, 'I do not know how to teach that weapon.';
$1n tells you, 'You haven't got %s for me.';
$1n tells you, 'You haven't got %d ability points.';
$1n tells you, 'I can not teach you any more.';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;   80; dagger                        ;   9;  9000;  10;          0;
  1;   50; fist                          ;  12; 12000;  10;          0;
  4;   60; quarterstaff                  ;  12; 12000;  10;          0;
 10;   70; sickle                        ;  14; 14000;   5;          0;
 14;   60; flail                         ;  14; 14000;  10;          0;
 20;   70; scythe                        ;  18; 18000;  15;          0;
"

#elif defined(_SPELL)

special SFUN_TEACH_INIT
"&lspells;0;
$1n tells you, 'I have never heard of such magic.';
$1n tells you, 'I do not know how to teach this kind of magic.';
$1n tells you, 'You have not got %s for me.';
$1n tells you, 'You have not got %d skill points.';
$1n tells you, 'I can not teach you any more, find a better teacher!';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;  100; raise divine                  ;   9;  9000;   8;          0;
  0;  100; detect undead                 ;   4;  4000;   5; 10;      0;
  1;  100; fart                          ;   4;  4000;   5; 10;      0;
  1;  100; cause light wounds            ;   9;  9000;   8; 16;      0;
  1;  100; detect curse                  ;   4;  4000;   5; 10;      0;
  2;  100; sense life                    ;   4;  4000;   5; 10;      0;
  2;  100; repel lesser undead           ;   4;  4000;   5; 10;      0;
  3;   50; sustain                       ;  14; 14000;  10;          0;
  4;  100; animate dead                  ;   4;  4000;   5; 10;      0;
  5;  100; detect poison                 ;   4;  4000;   5; 10;      0;
  5;  100; darkness                      ;   4;  4000;   5; 10;      0;
  6;  100; detect alignment              ;  14; 14000;  10; 20;      0;
  7;  100; madness                       ;  14; 14000;  10; 20;      0;
  9;   80; blind                         ;   9;  9000;   8; 16;      0;
 12;  100; cause serious wounds          ;  14; 14000;  10; 20;      0;
 13;  100; toxicate                      ;  14; 14000;  10;          0;
 14;  100; hold undead                   ;   4;  4000;   5;          0;
 14;   80; sleep                         ;  14; 14000;  10;          0;
 14;  100; continual darkness            ;   9;  9000;   8; 16;      0;
 16;   90; bad breath                    ;   9;  9000;   8; 16;      0;
 18;  100; curse                         ;   4;  4000;   5; 10;      0;
 19;  100; fear                          ;   9;  9000;   8; 16;      0;
 24;   50; remove curse                  ;  23; 23000;  20;          0;
 25;  100; undead door                   ;  18; 18000;  15;          0;
 25;  100; plague                        ;   4;  4000;   5;          0;
 28;   80; command                       ;   9;  9000;   8;          0;
 29;  100; harm                          ;  14; 14000;  15;          0;
 30;   90; stinking cloud                ;  14; 14000;  10; 20;      0;
 31;  100; repel greater undead          ;   9;  9000;   8; 16;      0;
 33;  100; protection from good          ;   9;  9000;   8; 16;      0;
 34;  100; slow rot                      ;   9;  9000;   5; 10;      0;
 36;  100; control undead                ;   4;  4000;   5; 10;      0;
 38;  100; energy drain                  ;   9;  9000;   8;          0;
 39;  100; dispel good                   ;   4;  4000;   5;          0;
 42;  100; cause panic                   ;  14; 14000;   7; 15;      0; 
"

#else

ERROR NO TYPE IN NECROMANCERS GUILD

#endif

#elif defined(_GUILD_MIDGAARD_PALADIN)

#if defined(_ABILITY)

special SFUN_TEACH_INIT
"&labilities; 0;
$1n tells you, 'I have never heard of such an ability.';
$1n tells you, 'I am not a master on that ability.';
$1n tells you, 'You do not have %s for the tithe.';
$1n tells you, 'You are not ready to learn more. You lack %d points.';
$1n tells you, 'I cannot teach you more than I have.';
$1n tells you, 'You must have no traces of magic about you to learn.';
$1n tells you, 'Remove all equipment, please.';

  0;  100; Strength                      ;   4;  4000;   8;       0;
  0;   70; Dexterity                     ;  14; 14000;  10;       0;
  0;   85; Constitution                  ;   9;  9000;  10;       0;
  0;   95; Hitpoints                     ;   9;  9000;   8;       0;
  0;   80; Charisma                      ;  14; 14000;  13;       0;
  0;   50; Brain                         ;  23; 23000;  14;       0;
  0;   40; Divine                        ;  14; 14000;  13;       0;
"

/* Sum (max) 520, Average = 10.86 */

#elif defined(_SKILL)

special SFUN_TEACH_INIT
"&lskills;0;
$1n tells you, 'I have never heard of such a skill.';
$1n tells you, 'I do not know how to teach this skill.';
$1n tells you, 'You havn't got %s for the tithe.';
$1n tells you, 'You havn't got %d skill points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;  100; shield                        ;   9;  9000;   8;          0;
  0;  100; rescue                        ;   4;  4000;   5; 10;      0;
  0;   90; consider	                     ;   9;  9000;   8; 16;      0;
  1;   70; diagnostics                   ;   9;  9000;   8;          0;
  3;   50; first aid                     ;   9;  9000;   8;          0;
  3;   80; bash                          ;   9;  9000;   8;          0;
  5;   70; turn undead                   ;  18; 18000;  15;          0;
 12;  100; leadership                    ;   4;  4000;   5; 10;      0;
"
#elif defined(_WEAPON)

special SFUN_TEACH_INIT
"&lweapons;0;
$1n tells you, 'I have never heard of such a weapon.';
$1n tells you, 'I do not know how to teach that weapon.';
$1n tells you, 'You havn't enough money to tithe for that training.';
$1n tells you, 'You havn't got %d skill points.';
$1n tells you, 'I can not teach you any more.';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;   70; fist                          ;  14; 14000;  12;          0;
  0;   80; dagger                        ;   4;  4000;   8;          0;
  1;  100; rapier                        ;   4;  4000;   8;          0;
  2;  100; short sword                   ;   4;  4000;   8;          0;
  4;  100; scimitar                      ;   4;  4000;   8;          0;
  8;  100; broad sword                   ;   9;  4000;   8;          0;
 16;  100; long sword                    ;   9;  9000;   8;          0;
 25;  100; great sword                   ;   9;  9000;  10;          0;
"

#elif defined(_SPELL)

special SFUN_TEACH_INIT
"&lspells;0;
$1n tells you, 'I have never heard of such magic.';
$1n tells you, 'I do not know how to teach this kind of magic.';
$1n tells you, 'You have not got %s for the tithe.';
$1n tells you, 'You have not got %d skill points.';
$1n tells you, 'I can not teach you any more, find a better teacher!';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all equipment, please.';

  0;   50; detect alignment              ;  14; 14000;  10; 20;      0;
  4;   50; detect poison                 ;  14; 14000;  10;          0;
  9;   50; detect undead                 ;  14; 14000;  10; 20;      0;
 10;   50; cure light wounds             ;  18; 18000;  15;          0;
 12;   50; light                         ;  14; 14000;  10; 20;      0;
 15;   50; repel lesser undead           ;  18; 18000;  15;          0;
 18;   80; sustain                       ;  14; 14000;  10;          0;
 20;   50; remove disease                ;  18; 18000;  15;          0;
 24;   50; remove curse                  ;  14; 14000;  10; 20;      0;
 30;   60; sanctuary                     ;  18; 18000;  15;          0;
 32;  100; protection from evil          ;  14; 14000;  10;          0;
 35;   50; bless                         ;  14; 14000;  10;          0;
 40;   50; cure serious wounds           ;  18; 18000;  15;          0;
 41;   50; continual light               ;  23; 23000;  20;          0;
 45;   50; dispel evil                   ;   4;  4000;   5; 10;      0;
"
#else

ERROR NO TYPE IN MIDGAARD PALADIN

#endif

#elif defined(_GUILD_MIDGAARD_MYSTIC)

#if defined(_ABILITY)

  0;   60; Strength                      ;  15; 15000;  10; 20;      0;
  0;   60; Dexterity                     ;  15; 15000;  10; 20;      0;
  0;   60; Constitution                  ;  15; 15000;  10; 20;      0;
  0;   60; Hitpoints                     ;  15; 15000;  10; 20;      0;
  0;   90; Charisma                      ;   5;  5000;   5; 10;      0;
  0;  100; Brain                         ;  10; 10000;   8; 16;      0;
  0;   90; Divine                        ;   5;  5000;   5; 10;      0;

#elif defined(_SKILL)

  1;   80; scroll use                    ;  20; 20000;  15;          0;
  1;   90; wand use                      ;  20; 20000;  15;          0;
  1;   40; diagnostics                   ;  15; 15000;  10;          0;
  0;   60; fleeing                       ;  20; 20000;  15;          0;
  6;   30; first aid                     ;  15; 15000;  10;          0;
  3;   60; shield                        ;  20; 20000;  15;          0;

#elif defined(_WEAPON)

 35;   80; war mattock                   ;  25; 25000;  20; 40;      0;
  0;   80; war hammer                    ;  25; 25000;  20;          0;
  8;   70; battle mace                   ;  25; 25000;  20;          0;
  0;   80; mace                          ;  15; 15000;  10;          0;
  0;   80; quarterstaff                  ;  20; 20000;  15;          0;
  6;   70; fist                          ;  20; 20000;  15;          0;

#elif defined (_SPELL)

 14;   80; awaken                        ;  20; 20000;  15;          0;
 12;   50; bless                         ;  10; 10000;   8;          0;
 13;  100; blind                         ;  15; 15000;  10; 20;      0;
 30;   80; call lightning                ;  15; 15000;  10;          0;
  0;  100; cause light wounds            ;   5;  5000;   5; 10;      0;
 13;  100; cause serious wounds          ;  10; 10000;   8; 16;      0;
 10;   50; cure blind                    ;  15; 15000;  10;          0;
  5;   70; cure light wounds             ;   5;  5000;   5;          0;
 25;   50; curse                         ;  15; 15000;  10;          0;
 25;  100; harm                          ;  15; 15000;  10;          0;
 16;   60; locate object                 ;  10; 10000;   8; 16;      0;
 14;   75; locate person                 ;  10; 10000;   8; 16;      0;
 20;   50; remove curse                  ;  15; 15000;  10;          0;
 15;  100; raise divine                  ;   5;  5000;   5; 10;      0;
  5;   50; strength                      ;  10; 10000;   8;          0;
  6;  100; dexterity                     ;  15; 15000;  10; 20;      0;
  6;   80; charisma                      ;  15; 15000;  10;          0;
  5;  100; divine resistance             ;  15; 15000;  10; 20;      0;
 30;   80; protection from good          ;  10; 10000;   8; 16;      0;
 32;  100; protection from evil          ;  15; 15000;  10;          0;
  9;   60; sanctuary                     ;  15; 15000;  10; 20;      0;
 22;   75; quicken                       ;  15; 15000;  10;          0;
  5;   50; sustain                       ;  15; 15000;  10; 20;      0;
  7;  100; detect alignment              ;  15; 15000;  10; 20;      0;
  6;  100; detect invisible              ;  15; 15000;  10; 20;      0;
  8;  100; detect magic                  ;  15; 15000;  10; 20;      0;
  5;  100; detect poison                 ;  15; 15000;  10; 20;      0;
  4;  100; detect undead                 ;  15; 15000;  10; 20;      0;
  3;  100; detect curse                  ;  15; 15000;  10; 20;      0;
  2;  100; sense life                    ;  15; 15000;  10; 20;      0;
 18;  100; storm call                    ;  15; 15000;  10;          0;
 20;   50; word of recall                ;  20; 20000;  15;          0;
  5;   50; create food                   ;  10; 10000;   8; 16;      0;
  9;   50; create water                  ;  10; 10000;   8; 16;      0;
  8;   50; light                         ;  10; 10000;   8; 16;      0;
 28;   50; continual light               ;  15; 15000;  10;          0;
  3;   50; darkness                      ;  10; 10000;   8; 16;      0;
 26;   50; continual darkness            ;  10; 10000;   8; 16;      0;
  8;  100; leather skin                  ;  10; 10000;   8;          0;
 16;  100; bark skin                     ;  15; 15000;  10;          0;
 24;  100; bone skin                     ;  20; 20000;  15;          0;
 20;   80; colourbeam                    ;  10; 10000;   8; 16;      0;
 40;   80; colourspray                   ;  15; 15000;  10; 20;      0;
  8;  100; command                       ;   5;  5000;   5; 10;      0;
  0;   80; flash                         ;   5;  5000;   5; 10;      0;
 25;   50; remove disease                ;  15; 15000;  10;          0;

#else

ERROR NO TYPE IN MYSTIC GUILD

#endif

#elif defined(_GUILD_MIDGAARD_SORCERER)

#if defined(_ABILITY)

special SFUN_TEACH_INIT
"&labilities;0;
$1n tells you, 'I have never heard of such an ability.';
$1n tells you, 'I do not know how to teach this ability.';
$1n tells you, 'You haven't got the %s for me.';
$1n tells you, 'You haven't got %d ability points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach
you.';
$1n tells you, 'Remove it all, then we proceed!';

  2;   50; Strength                      ;  18; 18000;  13;      0;
  0;   60; Dexterity                     ;  18; 18000;  10;      0;
  2;   60; Constitution                  ;  14; 14000;  13;      0;
  4;   60; Hitpoints                     ;  14; 14000;  14;      0;
  0;   90; Charisma                      ;   9;  9000;  10;      0;
  0;  100; Brain                         ;   4;  4000;   8;      0;
  0;  100; Magic                         ;   4;  4000;   8;      0;
"

/* Sum (max) 520, Average = 10.86 */

#elif defined(_SKILL)

special SFUN_TEACH_INIT
"&lskills;0;
$1n tells you, 'I have never heard of such a skill.';
$1n tells you, 'I do not know how to teach this skill.';
$1n tells you, 'You haven't got the %s for me.';
$1n tells you, 'You haven't got %d skill points.';
$1n tells you, 'I can not teach you any more';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove all, then we proceed!';

  0;   60; fleeing                       ;  14; 14000;  10;          0;
  0;  100; wand use                      ;   4;  4000;   5; 10;      0;
  1;  100; scroll use                    ;   4;  4000;   5; 10;      0;
  3;   60; consider                      ;   9;  9000;  10;          0;
 12;   35; weather watching              ;   4;  4000;   5;          0;
"

#elif defined(_WEAPON)

special SFUN_TEACH_INIT
"&lweapons;0;
$1n tells you, 'I have never heard of such a weapon.';
$1n tells you, 'I do not know how to teach that weapon.';
$1n tells you, 'You haven't got the %s for me.';
$1n tells you, 'You haven't got %d skill points.';
$1n tells you, 'I can not teach you any more.';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove it all, then we proceed!';

  0;   70; dagger                        ;  15; 15000;  10;          0;
  8;   50; short sword                   ;  20; 20000;  15;          0;
 16;   50; scimitar                      ;  25; 25000;  20;          0;
"
#elif defined(_SPELL)

special SFUN_TEACH_INIT
"&lspells;0;
$1n tells you, 'I have never heard of such sorcery.';
$1n tells you, 'I do not know how to teach this kind of sorcery.';
$1n tells you, 'You have not got the %s for me.';
$1n tells you, 'You have not got %d skill points.';
$1n tells you, 'I can not teach you any more, find a better teacher!';
$1n tells you, 'You must be unaffected by magic, otherwise I can't teach you';
$1n tells you, 'Remove it all, then we proceed!';

  0;  100; detect magic                  ;  14; 14000;  10; 20;      0;
  0;  100; raise magic                   ;   4;  4000;   5; 10;      0;
  2;  100; brain                         ;  14; 14000;  10;          0;
  3;  100; electrical shock              ;   4;  4000;   5; 10;      0;
  4;  100; dexterity                     ;  14; 14000;  10;          0;
  5;  100; mind shield                   ;  14; 14000;  10;          0;
  6;  100; constitution                  ;  14; 14000;  10;          0;
  6;  100; heat resistance               ;   4;  4000;   5; 10;      0;
  6;  100; snowball                      ;   4;  4000;   5; 10;      0;
  7;  100; cold resistance               ;   4;  4000;   5; 10;      0;
  7;   75; magic candle                  ;  14; 14000;  10;          0;
  8;  100; electricity resistance        ;   4;  4000;   5; 10;      0;
  9;  100; poison resistance             ;   4;  4000;   5; 10;      0;
  9;  100; slow                          ;  14; 14000;  10;          0;
 10;  100; strength                      ;  14; 14000;  10;          0;
 10;  100; acid resistance               ;   4;  4000;   5; 10;      0;
 11;  100; detect invisible              ;  14; 14000;  10; 20;      0;
 12;  100; identify                      ;  18; 18000;  15; 30;      0;
 13;   80; confusion                     ;  18; 18000;  15; 30;      0;
 14;  100; quicken                       ;  14; 14000;  10;          0;
 14;  100; fireball                      ;   9;  9000;   8; 16;      0;
 14;  100; mending                       ;   4;  4000;   5;          0;
 15;   80; stun                          ;  14; 14000;  10;          0;
 15;  100; charge wand                   ;  14; 14000;  10; 20;      0;
 15;  100; sending                       ;   9;  9000;   5;          0;
 16;  100; jolt                          ;   9;  9000;   8; 16;      0;
 16;   50; lock                          ;  18; 18000;  15; 30;      0;
 17;  100; charge staff                  ;  14; 14000;  10; 20;      0;
 17;  100; dispel magic                  ;  14; 14000;  10;          0;
 17;   50; xray vision                   ;   9;  9000;   8; 16;      0;
 18;   50; unlock                        ;  18; 18000;  15; 30;      0;
 19;  100; frostball                     ;   9;  9000;   8; 16;      0;
 20;  100; repair                        ;   9;  9000;   8;          0;
 24;  100; bone skin                     ;  18; 18000;  15;          0;
 25;   75; sun globe                     ;  14; 14000;  15;          0;
 25;  100; drowse                        ;  18; 18000;  15;          0;
 27;  100; hold monster                  ;  14; 14000;  10;          0;
 28;  100; improved identify             ;  23; 23000;  20;          0;
 28;  100; firestorm                     ;  14; 14000;  10; 20;      0;
 29;  100; haste                         ;  14; 14000;  10;          0;
 30;   50; minor gate                    ;  18; 18000;  15;          0;
 30;  100; lightning bolt                ;  14; 14000;  10; 20;      0;
 31;  100; reconstruct                   ;  14; 14000;  10;          0;
 32;   80; transport                     ;  18; 18000;  15;          0;
 34;  100; icestorm                      ;  14; 14000;  10; 20;      0;
 45;  100; stone skin                    ;  23; 23000;  20;          0;
"
#else

ERROR NO TYPE IN SORCERER GUILD

#endif

#elif defined(_GUILD_ASSASSIN)

#if defined(_ABILITY)

special SFUN_TEACH_INIT
"&labilities;0;
$1n tells you, 'I have never heard of such an ability.';
$1n tells you, 'I am not a master on that ability.';
$1n tells you, 'You are too poor to afford %s right now.';
$1n tells you, 'You are not ready to learn more. You lack %d points.';
$1n tells you, 'We are equals in that now.';
$1n tells you, 'You must have no traces of magic about you to learn.';
$1n tells you, 'Stand before me as you entered the world.';

  0;   50; Magic                         ;  18; 18000;  14;       0;
  0;   80; Strength                      ;   9;  9000;  10;       0;
  0;   80; Constitution                  ;  14; 14000;  10;       0;
  0;   50; Charisma                      ;  14; 14000;  13;       0;
  0;   60; Brain                         ;  18; 18000;  13;       0;
  0;  100; Hitpoints                     ;  14; 14000;   8;       0;
  0;  100; Dexterity                     ;   4;  4000;   8;       0;
"

/* Sum (max) 520, Average = 10.86 */

#elif defined(_SKILL)

special SFUN_TEACH_INIT
"&lskills;0;
$1n tells you, 'I have never heard of such a skill.';
$1n tells you, 'I am not a master on that skill.';
$1n tells you, 'You are too poor to afford %s right now.';
$1n tells you, 'You are not ready to learn more. You lack %d points.';
$1n tells you, 'We are equals in that now.';
$1n tells you, 'You must have no traces of magic about you to learn.';
$1n tells you, 'Stand before me as you entered the world.';

  0;   60; diagnostics                   ;   4;  4000;   5;          0;
  0;  100; consider                      ;   4;  4000;   5;          0;
  0;  100; fleeing                       ;   4;  4000;   5;          0;
  0;  100; sneak                         ;   9;  9000;   8;          0;
  0;  100; backstab                      ;   4;  4000;   5; 10;      0;
  0;  100; hide                          ;   4;  4000;   5; 10;      0;
  0;  100; climb                         ;  14; 14000;  10;          0;
  2;   80; search                        ;  14; 14000;  10;          0;
  3;   50; shield                        ;  14; 14000;  10;          0;
  4;   90; disarming                     ;   9;  9000;  10; 20;      0;
  5;   50; swimming                      ;  14; 14000;  10;          0;
 10;  100; two weapon style              ;   9;  9000;   8;          0;
 13;   70; scroll use                    ;  18; 18000;  15;          0;
"


#elif defined(_WEAPON)

special SFUN_TEACH_INIT
"&lweapons;0;
$1n hisses, 'What sort of weapon is that?';
$1n hisses, 'Bah! What true assassin would want to learn such a weapon.';
$1n hisses, 'I require cash up front for my services.';
$1n hisses, 'You mind is not ready to further the weapon arts.';
$1n hisses, 'I congratulate you, you are as now well learned as I.';
$1n hisses, 'Auras surround you that interfere with your training.';
$1n hisses, 'What you wear interferes with what I teach.';

  0;  100; dagger                        ;   4;  4000;   5;          0;
  0;  100; fist                          ;   4;  4000;   5;          0;
  1;  100; short sword                   ;   9; 10000;   8;          0;
  2;  100; wakizashi                     ;   9; 14000;  10;          0;
  4;   80; rapier                        ;  14; 14000;  10;          0;
  8;   80; kopesh                        ;  14; 14000;  10;          0;
  8;   80; falchion                      ;  14; 14000;  10;          0;
 10;   70; spear                         ;  18; 18000;  15;          0;
 12;   50; kick                          ;  18; 18000;  15;          0;
 14;   90; katana                        ;   9; 27000;   8;          0;
 20;   70; long sword                    ;  18; 18000;  15;          0;
"

#elif defined(_SPELL)

special SFUN_TEACH_INIT
"&lspells;0;
$1n tells you, 'I have never heard of such a way to use magic.';
$1n tells you, 'I do not know how to teach this sort of magic.';
$1n tells you, 'You have not got %s for me.';
$1n tells you, 'You have not got %d skill points.';
$1n tells you, 'I can not teach you any more, you are my equal in this.';
$1n tells you, 'You must be without magical aura in order to learn.';
$1n tells you, 'You must remove all that you wear.';


 15;   50; detect invisible              ;  23; 23000;  20;          0;
 20;   80; toxicate                      ;  18; 18000;  15;          0;
 25;   50; invisibility                  ;  18; 18000;  15;          0;
 30;   50; dexterity                     ;  18; 18000;  15; 30;      0;
 35;   50; quicken                       ;  23; 23000;  20;          0;
"
#else

ERROR NO TYPE IN ASSASSIN GUILD

#endif

#elif defined(_POLICE_SCHOOL)

#if defined(_SPELL)
special SFUN_TEACH_INIT
"&lspells;0;
$1n tells you, 'I have never heard of such a way to use magic.';
$1n tells you, 'I do not know how to teach this sort of magic.';
$1n tells you, 'You have not got %s for me.';
$1n tells you, 'You have not got %d skill points.';
$1n tells you, 'I can not teach you any more, you are my equal in this.';
$1n tells you, 'You must be without magical aura in order to learn.';
$1n tells you, 'You must remove all that you wear.';

  5;  100; locate wanted          ;  25; 2500;  20;          0;
 10;  100; find wanted            ;  25; 2500;  20;          0;
"
#elif defined(_SKILL)
special SFUN_TEACH_INIT
"&lskills;0;
$1n tells you, 'I have never heard of such a skill.';
$1n tells you, 'I am not a master on that skill.';
$1n tells you, 'You are too poor to afford %s right now.';
$1n tells you, 'You are not ready to learn more. You lack %d points.';
$1n tells you, 'We are equals in that now.';
$1n tells you, 'You must have no traces of magic about you to learn.';
$1n tells you, 'Stand before me as you entered the world.';

  0;  100; hand cuffing                   ;   5;  5000;   20;          0;
"
#endif

#else

ERROR NO GUILD TYPE DEFINED

#endif
