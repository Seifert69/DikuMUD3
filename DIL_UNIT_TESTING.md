# DIL Testing

DIL, the game's scripting language, has two sets of tests that run inside the game itself:

- **The suite** (`vme/zone/test.zon`, tests T001, T002, ...): run by the server with no player online.
  CI runs it on every push.
- **The player tests** (`vme/zone/diltest.zon`, tests P001, P002, ...): what only works on a player
  (player fields, colours, the editor, ...). A god runs them on their own character; run them by hand.

The DIL language itself is documented in the DIL Manual on <https://wiki.dikumud.com>
(`Manual:DIL Manual`).

## The suite

```bash
cd vme/zone && ../bin/vmc -m -d ../etc/ -I../include/ test.zon
cd ../bin && ./intergration_test.sh
```

`intergration_test.sh` uses `vme/etc/zonelist.test` for the run and puts the zonelist back
afterwards. It runs `./vme -w`: time-warped, so waits and heartbeats pass as fast as the CPU
allows, and a run takes a few minutes. It fails when vme exits with a non-zero code, times out, or
stops before the lab director is done. Read `vme/bin/vme.log`:

- `test/T030//INFO ...`, `test/T030//ERROR ...`: a test's lines; every ERROR is a failed check.
- `test/T000//SUCCESS Lab director finished running all tests.`: the run is complete.
- `The following tests failed : ...`: the failed tests, and vme exits with the first one's number.

Many other lines are normal (missing board files, the tests' own deliberate errors such as
"DIL '...' assigned ... to a field that takes ...").

### How it works

The lab director, an NPC that `%reset` loads into `labcontrol@test`, copies `T<TEST_MAXIMUM>@test`
onto the lab room, waits for it to send `"done"` to `labdirector@test`, and goes on down to T001.
At the end it reboots the server with exit code 0, or with the number of the first failed test.

### Writing a test

- Add `dilbegin T0nn(tstno : integer);` above the newest test (newest first) and raise
  `TEST_MAXIMUM`. One test per theme, with a comment block saying what it checks.
- Report each check with `logtest(tstno, LOGT_ERROR, "...")` when it fails; LOGT_INFO for progress.
- Always end with `sendtoalldil("done", "labdirector@test");`, also after a failure, and `destroy`
  what the test loaded so later tests start from the same world.
- Units for a test go in the zone with the test's number as prefix (`t032box`).
- Make each check able to fail: change its expected value once and see the ERROR.
- `and` and `or` bind looser than the comparisons, as in C (since #603). More traps are in
  `.claude/skills/dil/SKILL.md`.

### What the tests cover

| Tests | Theme |
|---|---|
| T001-T025 | followers, lists, extras, affects, ships, swimming, the older built-ins |
| T026 | `and` / `or` precedence |
| T027-T029 | the crash fixes: fields, functions and procedures that crashed or gave wrong results |
| T030-T032 | strings, numbers and logic, lookups, the types, files, template options |
| T033-T035 | events and their built-ins, fights, affects, followers, equipment, money, sight, finders |
| T036-T038 | character fields |
| T039-T041 | object, room, exit, light, zone and command fields |
| T042-T044 | control flow, messages and commands, procedures that change units |
| T045 | UTF-8 in DIL strings |
| T046 | teamwork@function: an NPC joins its own zone's fights |
| T047 | random treasure: average weapons and armour never below quality 0 (RndTreasure), exactly 0 when made to order (Treasure) |
| T048 | corpse_wait@death: an anchored corpse comes home after 2 to 30 minutes, by level |
| T049 | use on a staff: a helpful spell goes to the user and the group, an attack to the others |
| T050 | random magic items (randomt): potion, scroll, wand, magic staff, gem and bag made to order |
| T051 | a zone's bonusband (treasure band) as DIL reads it |
| T052 | random treasure loaded into corpses (rndCorpseLoad@randomt): bands by home zone and level, the chance, none without worn gear, XP or band |

## The player tests

A level 255 god goes to `lab@diltest` (a room without exits that nobody can teleport into) and
types `diltest`. The tests run on the god's own character and put back what they change; each
check is shown to the god and logged as `diltest/P002//INFO ...` or `diltest/P002//ERROR ...`, and
the last line says whether all passed.

To run them on a local server without logging in by hand:

```bash
cd vme/zone && ../bin/vmc -m -d ../etc/ -I../include/ diltest.zon
cd ../bin && ./pc_integration_test.sh
```

It boots the game with the zones compiled here (a public clone has no private zones), starts a
telnet mplex on port 4243 (`PCTEST_PORT`), logs in the `immortal_name` god from `server.cfg`
(creating the character on a fresh `lib/`, password `PCTEST_PASSWORD`, default `pctest12`), types `diltest`
in the lab and exits 0 when every player test passed.
