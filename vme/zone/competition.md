# competition: review questions

## Step 1

1. **comp_board** (line 1443): the sign says "read [number] - shows individual competition
   information", but the board's DIL only handles `look` and `list`; `read` falls through to the
   ordinary board command. Is there a `read` handler somewhere else, or should the line go?
   (The "top 200 leaders" on the same sign was changed to "top 20", since the board lists 20.) Fixed (text follows the world): removed the "read [number]" line from the sign.
2. **xp_board** (line 1422): the object is the "Legends of Valhalla board" (now with "the" in
   front), but the table it shows is headed "Heroes of Valhalla". Should one name be used for both? Fixed (text follows the world): table heading changed from Heroes of Valhalla to Legends of Valhalla.
3. **death_brd** (line 70): for a player who isn't on the death list, the DIL checks
   `"$TOTAL_DEATHS" in rm.extra` (the board object) but reads `activator.extra`, so it always
   shows 0 deaths. Probably meant `activator.extra` (DIL logic, for step 2). Fixed in step 2.

## Step 2

Fixed in place:
- **death_brd** (line 70): for a player not on the death list it tested `$TOTAL_DEATHS` on the
  board object (`rm.extra`) and so always showed 0; now it tests `activator.extra`, which the next
  line reads (step 1, question 3).

Questions:
1. **rlc_format** (lines 359-390): meant to pad a column with spaces, but it adds
   `"&nbsp;" + itoa(count)`, the number itself, so `list <n>` shows text like `&nbsp;14Players`.
   (Probably an old `&s<n>` colour/space code turned into `&nbsp;` when the client changed; the
   comment at line 593 wonders the same about `&x`.) Rewrite `list` as an HTML table like the
   death and XP boards, or pad with a loop of `&nbsp;`? Fixed: a new `rlc_pad` adds that many `&nbsp;`; `list <n>` no longer shows the stray numbers.
2. **run_comp / boot_comp** (lines 517, 688): `run_comp` (the update of a competition every minute or two
   and the "has just won" announcement) is `dilcopy`'d onto `board_obj` and isn't `recall`, so it
   is not saved by `store(rm, "comp_obj", TRUE)`. After a reboot `boot_comp` restores `comp_obj`
   and doesn't start `run_comp` again (it reads the `.comp` files only when nothing was stored),
   so a running competition is only updated when someone types `list <n>` and its end is never
   announced. Start `run_comp` for each unfinished competition after the restore? Left: `store` saves a unit's DIL programs with it (db_file.cpp bwrite_func; `recall` only decides whether one resumes where it stopped), so `run_comp` comes back with `comp_obj` and starting it again would run each competition twice.
3. **upd_rebirth** (lines 1353-1359): does nothing. It looks for `"<time> INFO"` among the player's
   `$comp` names, which hold only `"<time>"`, so it returns at once (and `return`s instead of going
   on to the next competition); and if found, `t := u.exp - v; v := u.exp - t;` sets `v` to
   itself. Nothing calls it. Remove it, or what should a rebirth do to a competition score?
4. **del_cply** (line 313): never called, and it tests `"competition loaded"` on `board_obj`,
   while `boot_comp` (line 718) puts that extra on the board room. Remove it?
5. **cmd_comp** (line 1027): in the `competition` menu, input that is none of 1-9, C or D leaves
   the menu silently (back to `GS_PLAY`) and runs `upd_comp` with the competition's title instead
   of its number. Should an unknown choice show the menu again (`goto comp_menu`, as the PK and race
   menus do with "Try again.")? Also line 770: any argument of two words (`competition foo 2`)
   deletes competition 2 without asking; only `delete <n>` was meant? Fixed: an unknown menu choice says "Try again." and shows the menu again; only `competition delete <n>` (or an abbreviation of delete) deletes directly.
