#!/usr/bin/env node
// Chaos test for the websocket mplex: players come and go, vme and the mplex restart, and the
// test watches whether the mplex crashes, spins, doubles its connection to vme or hangs.
//
//   node vme/scripts/mplex_chaos.mjs [--mplex PATH] [--seconds 300] [--workers 8] [--players 4] [--out DIR]
//
// It runs, from vme/bin, a vme (mother port 4999 from etc/server.cfg) and the mplex in websocket
// mode on port 4280, so both ports must be free. Build first (make all). Needs Node 22 or later
// (the built-in WebSocket).
//
// Worker processes each run a few websocket players. A player creates its character (Chaosaa,
// Chaosab ...: they save themselves into vme/lib like any other), logs in, plays (walks, looks, says,
// fights, asks for help), sends bursts of commands, idles, and leaves by closing its socket or
// by quitting. At random the test:
//   - kills a worker outright, so its players' sockets die without a goodbye, then starts another;
//   - kills vme (SIGKILL) or shuts it down (SIGTERM), then starts it again;
//   - restarts the mplex while the players reconnect.
// It reports:
//   - mplex crashes: exits it didn't ask for (the last lines of its stderr give the assertion);
//   - vme crashes, the same way (systemd-coredump keeps the core: coredumpctl list);
//   - the mplex spinning: over 70% CPU for 9 s;
//   - more than one connection from the mplex to vme;
//   - hangs: every 10 s a new player must get the greeting within 8 s, once vme has been up 15 s.
// The exit status is 1 if any of that happened. Logs go to --out (default: a new temp directory).
import { spawn, fork, execSync } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";

const SELF = fileURLToPath(import.meta.url);
const BIN_DIR = path.resolve(path.dirname(SELF), "../bin");
const WS_URL = "ws://localhost:4280/echo";
const rnd = n => Math.floor(Math.random() * n);
const pick = a => a[rnd(a.length)];
const sleep = ms => new Promise(r => setTimeout(r, ms));
const strip = s =>
    s.replace(/<data[^>]*>.*?<\/data>/gs, "").replace(/<br\s*\/?>/g, "\n").replace(/<[^>]+>/g, "")
        .replace(/&gt;/g, ">").replace(/&lt;/g, "<").replace(/&amp;/g, "&").replace(/&nbsp;/g, " ");

if (process.argv[2] === "--worker") worker(process.argv[3].split(",").map(Number));
else await driver(parseArgs(process.argv.slice(2)));

function parseArgs(argv) {
    const o = { mplex: path.join(BIN_DIR, "mplex"), seconds: 300, workers: 8, players: 4, out: "" };
    for (let i = 0; i < argv.length; i += 2) {
        const k = argv[i].replace(/^--/, "");
        if (!(k in o) || argv[i + 1] === undefined) {
            console.error("usage: mplex_chaos.mjs [--mplex PATH] [--seconds N] [--workers N] [--players N] [--out DIR]");
            process.exit(2);
        }
        o[k] = typeof o[k] === "number" ? Number(argv[i + 1]) : argv[i + 1];
    }
    o.mplex = path.resolve(o.mplex);
    o.out = o.out ? path.resolve(o.out) : fs.mkdtempSync(path.join(os.tmpdir(), "mplex-chaos-"));
    fs.mkdirSync(o.out, { recursive: true });
    return o;
}

/* ======================= a worker: a few players ======================= */
function worker(slots) {
    const PLAY = ["look", "n", "s", "e", "w", "u", "d", "who", "score", "inventory", "equipment", "say hello there",
        "emote stretches", "help", "help kill", "time", "weather", "exits", "get all", "consider guard", "wimpy",
        "practice", "areas", "look at sign", "kill rat", "flee", "where", "news", "commands"];
    const name = slot => "Chaos" + String.fromCharCode(97 + (Math.floor(slot / 26) % 26), 97 + (slot % 26));
    const stats = { frames: 0, sent: 0, ingame: 0 };
    setInterval(() => process.send && process.send(stats), 2000);

    function player(slot) {
        const ws = new WebSocket(WS_URL);
        let text = "", inGame = false, loop;
        const say = s => { try { ws.send(s); stats.sent++; } catch {} };
        ws.onopen = () => {
            say(""); // mplex swallows the first message
            loop = setInterval(() => {
                const t = text;
                text = "";
                // Through the login: name, new password twice or the password, the questions, the menu
                if (/By what name/i.test(t)) return say(name(slot));
                if (/Did I get that right/i.test(t)) return say("y");
                if (/Wrong password/i.test(t)) return;
                if (/password/i.test(t)) return say("chaos123");
                if (/Throw the other copy/i.test(t)) return say("y");
                const enter = t.match(/(\w)\) Enter (the game|Valhalla)/);
                if (enter) return say(enter[1]);
                if (/Please select|Make your choice/i.test(t)) return say(String(1 + rnd(2)));
                if (/Press RETURN|Return for more/i.test(t)) return say("");
                // In the game: the prompt ("105m/204e/44h>"). A new character is a guest until it
                // saves, so it saves at once: later logins go through the password and "throw out"
                if (!inGame && /\d+m\/\d+e\/\d+h>/.test(t)) { inGame = true; stats.ingame++; return say("save"); }
                const r = Math.random();
                if (r < 0.01) { clearInterval(loop); return ws.close(); } // leaves, closing the socket
                if (r < 0.015 && inGame) return say("quit"); // leaves through the game
                if (r < 0.05) { for (let i = 0; i < 20; i++) say(pick(PLAY)); return; } // a burst
                if (r < 0.3) return; // idles
                say(inGame ? pick(PLAY) : pick(["", "y", "1", "look", name(slot)]));
            }, 20 + rnd(300));
        };
        ws.onmessage = e => { stats.frames++; text += strip(String(e.data)); };
        ws.onclose = () => { clearInterval(loop); setTimeout(() => player(slot), rnd(3000)); };
        ws.onerror = () => {};
    }
    for (const s of slots) player(s);
}

/* ======================= the driver ======================= */
async function driver(o) {
    const t0 = Date.now();
    const log = line => {
        const l = `${String(Math.round((Date.now() - t0) / 1000)).padStart(4)}s ${line}`;
        console.log(l);
        fs.appendFileSync(path.join(o.out, "chaos.log"), l + "\n");
    };
    const res = { mplexCrashes: [], vmeCrashes: [], spins: 0, doubles: 0, hangs: 0, greetings: 0, vmeKills: 0, mplexRestarts: 0, workerKills: 0 };
    let vme = null, mplex = null, vmeUpSince = 0, stopping = false;
    const wstats = new Map();
    const workers = new Set();

    const listening = port => execSync(`ss -ltn 'sport = :${port}'`).toString().trim().split("\n").length > 1;
    const cpuMs = pid => { try { const f = fs.readFileSync(`/proc/${pid}/stat`, "utf8").split(") ")[1].split(" "); return (+f[11] + +f[12]) * 10; } catch { return 0; } };
    const connsToVme = pid => execSync(`ss -tnp 'dport = :4999'`).toString().split("\n").filter(l => l.includes(`pid=${pid},`)).length;
    const tail = file => { try { return fs.readFileSync(file, "utf8").trim().split("\n").slice(-2).join(" | "); } catch { return ""; } };

    if (listening(4999) || listening(4280)) {
        console.error("port 4999 or 4280 is in use: stop that vme or mplex first");
        process.exit(2);
    }
    log(`mplex ${o.mplex}, ${o.seconds} s, ${o.workers} x ${o.players} players, logs in ${o.out}`);

    // Starts a server process; an exit nobody asked for is a crash: logged, and it starts again
    function server(what, file, args, crashes, onUp) {
        if (stopping) return null;
        const errFile = path.join(o.out, `${what}-stderr.log`);
        const err = fs.openSync(errFile, "a");
        const p = spawn(file, args, { cwd: BIN_DIR, stdio: ["ignore", err, err] });
        fs.closeSync(err);
        p.on("exit", (code, sig) => {
            if (p.expected || stopping) return;
            crashes.push({ at: Math.round((Date.now() - t0) / 1000), code, sig, stderr: tail(errFile) });
            log(`*** ${what.toUpperCase()} CRASHED (code ${code}, signal ${sig}) ${tail(errFile)}`);
            setTimeout(onUp, 2000);
        });
        return p;
    }
    function startVme() {
        vmeUpSince = 0;
        vme = server("vme", "./vme", ["-l", path.join(o.out, "vme.log")], res.vmeCrashes, startVme);
        const me = vme;
        (async () => {
            for (let i = 0; i < 180 && me && vme === me && me.exitCode === null; i++) {
                if (listening(4999)) { vmeUpSince = Date.now(); log("vme up"); return; }
                await sleep(1000);
            }
        })();
    }
    function startMplex() {
        mplex = server("mplex", o.mplex, ["-w", "-p", "4280", "-s", "4999", "-l", path.join(o.out, "mplex.log")], res.mplexCrashes, startMplex);
        if (mplex) { mplex.cpu = cpuMs(mplex.pid); mplex.cpuAt = Date.now(); mplex.hot = 0; }
    }
    async function stop(p, sig) {
        if (!p || p.exitCode !== null || p.signalCode !== null) return;
        p.expected = true;
        const gone = new Promise(r => p.on("exit", r));
        p.kill(sig);
        await gone;
    }

    let nextSlot = 0;
    function startWorker() {
        if (stopping) return;
        const slots = Array.from({ length: o.players }, () => nextSlot++ % (o.workers * o.players + 8)); // names reused
        const w = fork(SELF, ["--worker", slots.join(",")], { stdio: "ignore" });
        w.on("message", m => wstats.set(w.pid, m));
        w.on("exit", () => workers.delete(w));
        workers.add(w);
    }

    async function probe() {
        if (!mplex || !vme || !vmeUpSince || Date.now() - vmeUpSince < 15000 || connsToVme(mplex.pid) !== 1) return;
        const ok = await new Promise(resolve => {
            const ws = new WebSocket(WS_URL);
            let got = "";
            const timer = setTimeout(() => { try { ws.close(); } catch {} resolve(false); }, 8000);
            ws.onopen = () => ws.send("");
            ws.onmessage = e => {
                got += strip(String(e.data));
                if (/By what name/i.test(got)) { clearTimeout(timer); ws.close(); resolve(true); }
            };
            ws.onerror = () => {};
        });
        if (ok) res.greetings++;
        else if (!stopping && mplex && vme && vmeUpSince && Date.now() - vmeUpSince > 15000) {
            res.hangs++;
            log("*** HANG? a new player got no greeting within 8 s");
        }
    }

    startVme();
    for (let i = 0; i < 180 && !vmeUpSince; i++) await sleep(1000);
    if (!vmeUpSince) { log("vme didn't come up; see vme.log"); await stop(vme, "SIGKILL"); process.exit(2); }
    startMplex();
    await sleep(500);
    for (let i = 0; i < o.workers; i++) startWorker();

    const timers = [
        setInterval(() => { // a worker dies, and with it its players' sockets
            const w = pick([...workers]);
            if (!w) return;
            w.kill("SIGKILL");
            res.workerKills++;
            setTimeout(startWorker, rnd(3000));
        }, 2500),
        setInterval(async () => { // vme dies or shuts down
            if (!vme || !vmeUpSince || Math.random() < 0.5) return;
            const how = Math.random() < 0.5 ? "SIGKILL" : "SIGTERM";
            log(`stopping vme (${how})`);
            res.vmeKills++;
            await stop(vme, how);
            await sleep(1000 + rnd(6000));
            startVme();
        }, 40000),
        setInterval(async () => { // the mplex restarts while players reconnect
            if (!mplex || Math.random() < 0.4) return;
            log("restarting the mplex");
            res.mplexRestarts++;
            const m = mplex;
            mplex = null;
            await stop(m, "SIGTERM");
            await sleep(rnd(1500));
            startMplex();
        }, 30000),
        setInterval(() => { // spin and double connection
            if (!mplex) return;
            const now = Date.now(), c = cpuMs(mplex.pid);
            const pct = (100 * (c - mplex.cpu)) / Math.max(1, now - mplex.cpuAt);
            mplex.cpu = c;
            mplex.cpuAt = now;
            mplex.hot = pct > 70 ? mplex.hot + 1 : 0;
            if (mplex.hot === 3) { res.spins++; log(`*** MPLEX SPINNING at ${pct.toFixed(0)}% CPU`); }
            const n = connsToVme(mplex.pid);
            if (n > 1) { res.doubles++; log(`*** ${n} CONNECTIONS FROM THE MPLEX TO VME`); }
        }, 3000),
        setInterval(probe, 10000),
        setInterval(() => {
            const s = [...wstats.values()].reduce((a, b) => ({ frames: a.frames + b.frames, sent: a.sent + b.sent, ingame: a.ingame + b.ingame }), { frames: 0, sent: 0, ingame: 0 });
            log(`mplex ${mplex ? "up" : "down"}, vme ${vme ? (vmeUpSince ? "up" : "booting") : "down"}, ${workers.size} workers | so far: ${s.sent} commands sent, ${s.frames} messages received, ${s.ingame} times in the game`);
        }, 30000),
    ];

    await sleep(o.seconds * 1000);
    stopping = true;
    timers.forEach(clearInterval);
    for (const w of workers) w.kill("SIGKILL");
    await Promise.all([stop(mplex, "SIGTERM"), stop(vme, "SIGTERM")]);
    log("RESULT " + JSON.stringify(res));
    const bad = res.mplexCrashes.length + res.vmeCrashes.length + res.spins + res.doubles + res.hangs;
    log(bad ? "FAILED" : "PASSED");
    process.exit(bad ? 1 : 0);
}
