# CLAUDE.md

DikuMUD3 — a C++ MUD server. Three main binaries: `vme` (the game server),
`mplex` (network multiplexer — telnet and websocket frontends connecting to
vme), and `vmc` (the zone/DIL compiler). All land in `vme/bin/` after build.

## Build

```bash
cmake .                                # Debug: -g at -O0, full warnings
cmake -DCMAKE_BUILD_TYPE=Release .     # -O2 — use this for anything performance-related
make all -j$(nproc)
ctest                                  # 5 Boost.Test suites, must all pass
```

- The codebase builds **warning-free**; keep it that way (add `-DWERROR=1` to enforce).
- `make all` also compiles all zones (`zone_files` target) and runs the unit
  tests as a post-build step.
- Production servers should run a Release build; the Debug default is
  noticeably slower (no optimization + libstdc++ assertions).

## Run locally

```bash
cd vme/bin
./vme                       # game server; reads ../etc/server.cfg, listens on 4999 for mplexers
./mplex -p 4242             # telnet multiplexer -> telnet localhost 4242
./mplex -w -t -p 4280       # websocket multiplexer for the web client
```

- Web client: open `vme/www/client/index.html` in a browser, set host
  `localhost`, port `4280`.
- Several mplexers (telnet and websocket) can run against one vme.
- First god character: log in as `Papi` (the `immortal_name` in
  `vme/etc/server.cfg`).
- "Can't bind Mother Connection port 4999" means a stale server is still
  running: `pkill -x vme`.
- `runvme.sh` / `runmplex.sh <port>` are the keep-alive wrapper scripts used
  for real deployments.

## Logs

- `vme/bin/vme.log` — the game server log. **Truncated on every boot**, so
  save it before restarting if you need evidence.
- `mplex.log` in the directory mplex was started from — mplex connection log.
- `tail -f` both while developing; DIL runtime errors, zone reset problems and
  crashes in scripts all land in `vme.log`.

## Compiling zones with vmc

Zone sources are `vme/zone/*.zon` (world + embedded DIL scripts). They compile
to `.data`/`.reset` files in the same directory, which the server loads at
boot (`zondir` in server.cfg).

```bash
cd vme/zone
../bin/vmc -m -d ../etc/ -I../include/ *.zon
```

- **`-m` compiles only changed zones** — the fast loop while editing a zone.
- `-s` suppresses data file output (pure syntax check), `-l` makes warnings
  fatal, `-v` verbose, `-q` quiet.
- `make zone_files` from the repo root does the same through cmake.
- The server only reads zone data at boot — **restart vme after recompiling**.
- Compile leftovers per zone: `.err` (errors), `.dh`, `.dot`.

## Debugging crashes

- Core dumps go to systemd-coredump, not a `core` file:
  `coredumpctl list`, then `coredumpctl gdb` for the most recent one.
- Debug builds already carry full symbols; for Release use
  `-DCMAKE_BUILD_TYPE=RelWithDebInfo` if you need both speed and symbols.
- `-DPROFILING=1` adds gprof instrumentation; on modern kernels
  `perf record -p $(pgrep -x vme)` is usually better.
- In-game `wstat` shows per-DIL-template CPU (`fCPU`) and instruction counts —
  the first stop for performance questions.
- Defining `MEMORY_DEBUG` routes allocations through the membug tracker
  (see `vme/src/membug.h`).

## Repository layout

| path | contents |
|---|---|
| `vme/src/` | game server sources (`vme` binary) |
| `vme/src/mplex/` | network multiplexer (telnet/websocket, vendored websocketpp) |
| `vme/src/vmc/` | zone compiler (bison/flex grammars: `vmcpar.y`, `dilpar.y`) |
| `vme/src/pp/` | the preprocessor used by vmc |
| `vme/src/dil*.cpp` | the DIL script interpreter (runs in the server) |
| `vme/zone/` | zone sources (`.zon`) and their compiled output |
| `vme/lib/` | runtime data: players, boards, DIL files — treat as live data |
| `vme/etc/` | `server.cfg`, money definitions, compile-time includes |
| `vme/www/client/` | browser client |
| `unit_tests/` | Boost.Test suites |

## Conventions

- DIL is the MUD's scripting language; language docs live at
  <https://wiki.dikumud.net/wiki/DikuMUD>, and `DIL_UNIT_TESTING.md` covers
  in-game DIL test zones.
- Unit tests: one `*_tests.cpp` per class in `unit_tests/`; class-level
  changes (e.g. to `cNamelist`) should come with test cases.
- `vme/lib/` contains live player data on a real server — never assume it is
  disposable.
- Generated files (Doxygen configs, `compile_commands.json`, zone `.data`)
  are gitignored or build products — don't commit them.
