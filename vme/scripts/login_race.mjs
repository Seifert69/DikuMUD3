#!/usr/bin/env node
// Login races, played out against a running vme through a websocket mplex (default
// ws://localhost:4280/echo; give another address as the first argument). Exit status 0 if vme
// came through, 1 if it went away. Needs Node 22 or later (the built-in WebSocket).
//
// Two connections log in under the same new name. A gets as far as the gender question, so its
// character has a password but isn't saved. B then gives the name: the game borrows A's
// password and asks B for it. A goes away, and B gives the password. vme used to let B into the
// game as a character that existed nowhere (no player file, no file index), and the first DIL
// log() at login (login_clean@guilds) crashed it. B must now be turned away.
const URL = process.argv[2] || "ws://localhost:4280/echo";
const NAME = "Racer" + String.fromCharCode(97 + Math.floor(Math.random() * 26), 97 + Math.floor(Math.random() * 26), 97 + Math.floor(Math.random() * 26));
const PASSWORD = "race1234";
const sleep = ms => new Promise(r => setTimeout(r, ms));
const strip = s => s.replace(/<data[^>]*>.*?<\/data>/gs, "").replace(/<br\s*\/?>/g, "\n").replace(/<[^>]+>/g, "").replace(/&gt;/g, ">").replace(/&lt;/g, "<").replace(/&amp;/g, "&");

// A connection that collects what the game says and can wait for a pattern
function connect(label) {
    const ws = new WebSocket(URL);
    const c = { ws, text: "", closed: false };
    ws.onmessage = e => (c.text += strip(String(e.data)));
    ws.onclose = () => (c.closed = true);
    ws.onerror = () => {};
    c.open = new Promise((resolve, reject) => {
        ws.onopen = () => { ws.send(""); resolve(); };
        setTimeout(() => reject(new Error(`${label}: no connection to ${URL}`)), 5000);
    });
    c.say = s => ws.send(s);
    c.waitFor = async (re, ms = 10000) => {
        for (let t = 0; t < ms; t += 50) {
            if (re.test(c.text)) { const all = c.text; c.text = ""; return all; }
            await sleep(50);
        }
        throw new Error(`${label}: no ${re} within ${ms} ms; it said: ${c.text.slice(-300)}`);
    };
    return c;
}

// Does the game still greet a new connection?
async function alive() {
    const p = connect("probe");
    try {
        await p.open;
        await p.waitFor(/By what name/i, 8000);
        return true;
    } catch {
        return false;
    } finally {
        p.ws.close();
    }
}

if (!(await alive())) {
    console.error(`no game at ${URL}`);
    process.exit(2);
}

const a = connect("A");
await a.open;
await a.waitFor(/By what name/i);
a.say(NAME);
await a.waitFor(/Did I get that right/i);
a.say("y");
await a.waitFor(/password/i);
a.say(PASSWORD);
await a.waitFor(/retype/i);
a.say(PASSWORD);
await a.waitFor(/gender|select/i); // A's character has a password, and isn't saved

const b = connect("B");
await b.open;
await b.waitFor(/By what name/i);
b.say(NAME);
await b.waitFor(/password/i); // borrowed from A

a.ws.close(); // A goes away
await sleep(1500);
b.say(PASSWORD);
await sleep(3000);
const said = strip(b.text);
b.ws.close();

const ok = await alive();
console.log(`B was told: ${said.trim().split("\n").slice(-3).join(" / ") || "(nothing)"}`);
console.log(ok ? "PASSED: vme is still up" : "FAILED: vme went away (see vme.log, coredumpctl list)");
process.exit(ok ? 0 : 1);
