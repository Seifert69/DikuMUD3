# wiz: review questions

## Shared zone review

Fixed in place:
- rebootdaemon, line 996: the room's line "$3n bites $1n  very hard in $1s finger." went TO_CHAR (so the biter got both lines and the room none); now TO_ROOM, double space removed.
- cadopt, line 1101: the room was told "$3n initiates the universe adoption process." with $3n the universe itself; now "$1n" (the god).
- sticker object, line 1288: "personal <playername> <item>" was eaten by the client as HTML tags; now `&lt;playername&gt; &lt;item&gt;`, as the gnarly wand's usage does.
- reboot_daemon: descr "A reboot daemon is playing here" -> full stop; extra "do you." -> "do you?", "ruffle its hair" -> "her hair" (she).
- ringbell, line 1371: "You here a loud bell clanging ... someone wants coffee!." -> "You hear a loud bell clanging... someone wants coffee!".
- personal, line 325: "...and are logged" -> full stop.
- watch_catch, line 284: sendtext without `<br/>`; added.
- submit, lines 475-476: "Well thats" -> "Well, that's"; "paper.  buy one and see" -> "paper. Buy one and see.".
- nod_dil, line 524: "shake your head" + "otherwise." -> missing space added.
- Room texts: quest_hall "echoed of" -> "echoed off"; board_room "a endless void" -> "an endless void"; builders_bar "Cananda" -> "Canada", "builders agreement" -> "builders' agreement"; gnort_home "wellstocked" -> "well-stocked", "noone" -> "no one"; wolf_home "with in inner light" -> "with an inner light", "someones" -> "someone's"; aaaaa "the Builder's Bar" -> "the Builders' Bar." (the room's title); dil_board and wiz_board "unexplicably" -> "inexplicably".

Questions:
1. quest_hall (line 601) says it has no exits ("none are provided") and history_hall (line 630) calls itself "exitless", but both have exits to the other halls (added when the rooms were interconnected, see the zone notes). Suggest dropping those phrases. Fixed (text follows the world): dropped the no-exits phrases from quest_hall and history_hall.
2. watchdog2 / watch_catch, lines 240-288 (marked "Not tested yet"): watchdog2 quits unless the watched player has watch_catch, and watch_catch only shows messages whose name is its own, so it reports a player's commands to that same player. Nothing uses them; delete, or rewrite for a watcher?
3. make_personal, line 403: "No such player here to remake $2n for." uses $2n = the unit that was not a player (or null); suggest "No such player here."  Fixed: make_personal says "No such player here."
4. personal, line 366: `isset(PC_SPIRIT, self.outside.pcflags)` has its arguments the other way round from every other isset; it works only because isset is a plain bitwise and. Suggest `isset(self.outside.pcflags, PC_SPIRIT)` when the zone is next touched. Fixed: personal now calls `isset(self.outside.pcflags, PC_SPIRIT)`.
