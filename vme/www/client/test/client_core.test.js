// Tests for the shared web client core. Run with: node --test vme/www/client/test/*.test.js
"use strict";
const test = require("node:test");
const assert = require("node:assert/strict");
const H = require("../core/html.js");
const P = require("../core/protocol.js");
const E = require("../core/engine.js");
const C = require("../core/connection.js");

/* ---------- html ---------- */

test("parseAttrs reads single, double and bare values and decodes entities", () => {
    assert.deepEqual(H.parseAttrs(" class='room_title' zone=\"midgaard\" exits=n map='1,2' x='&lt;b&gt;'"),
        { class: "room_title", zone: "midgaard", exits: "n", map: "1,2", x: "<b>" });
});

test("parse tolerates unbalanced and stray tags", () => {
    const t = H.parse("<div class='wiz'>open <b>bold</div></span>after");
    assert.equal(t.children.length, 2);
    assert.equal(t.children[0].tag, "div");
    assert.equal(H.textOf(t), "open boldafter");
});

test("textOf turns <br> and blocks into newlines and decodes entities", () => {
    assert.equal(H.htmlToText("a<br/>b<br>c &amp; &lt;d&gt;"), "a\nb\nc & <d>");
    assert.equal(H.htmlToText("<h1>Title</h1>text"), "Title\ntext");
});

test("hasText ignores whitespace and empty elements", () => {
    assert.equal(H.hasText(H.parse("<go-ahead/> <br/>\u00a0<div class='x'></div>")), false);
    assert.equal(H.hasText(H.parse("<div><b> a</b></div>")), true);
});

test("quoted attributes may contain >", () => {
    const t = H.parse("<a cmd='look >' href='#'>x</a>");
    assert.equal(t.children[0].attrs.cmd, "look >");
});

/* ---------- framing ---------- */

test("Framer passes plain frames through", () => {
    const f = new P.Framer();
    assert.deepEqual(f.push("hello<br/>"), [{ type: "html", html: "hello<br/>" }]);
});

test("Framer joins a <paged> block split over several frames", () => {
    const f = new P.Framer();
    assert.deepEqual(f.push("<paged><h1>Help</h1>part one "), []);
    assert.deepEqual(f.push("part two"), []);
    assert.deepEqual(f.push(" end</paged>"), [{ type: "paged", html: "<h1>Help</h1>part one part two end" }]);
});

test("Framer finds an end tag split across frames", () => {
    const f = new P.Framer();
    assert.deepEqual(f.push("<paged>" + "x".repeat(50) + "</pa"), []);
    assert.deepEqual(f.push("ged>after"), [{ type: "paged", html: "x".repeat(50) }, { type: "html", html: "after" }]);
});

test("Framer splits text, data and script inside one frame", () => {
    const f = new P.Framer();
    const out = f.push("before<data type='$alias'><li>k kill $1</li></data><script>PasswordOn()</script>after");
    assert.deepEqual(out.map(s => s.type), ["html", "data", "script", "html"]);
    assert.equal(out[1].dtype, "$alias");
    assert.equal(out[2].code, "PasswordOn()");
});

test("Framer buffers a large $json split mid-way", () => {
    const f = new P.Framer();
    assert.deepEqual(f.push("<data type='$json'>{\"a\":"), []);
    const out = f.push("[1,2,3]}</data>");
    assert.deepEqual(out, [{ type: "data", dtype: "$json", body: "{\"a\":[1,2,3]}" }]);
});

/* ---------- classification ---------- */

const PROMPT = "<div class='prompt' bars='45/60 80/80 10/20'>45H 80E 10M &gt;</div>";
const ROOM = "<h1 class='room_title' zone='midgaard' exits='n e s w ' map='480,505' roomid='market_sq@midgaard'>The Market Square [market_sq@midgaard]</h1><br/>"
    + "<div class='room_descr'>   A huge ancient oak tree stands here.</div><br/><div class='exit'>Exits:  north, east</div><br/>"
    + "<div class='npc_title'>A cityguard is here.</div><br/>";

test("prompt bars become numbers", () => {
    const { events, shape } = P.classify(PROMPT);
    assert.deepEqual(events[0].bars, { hp: 45, maxhp: 60, ep: 80, maxep: 80, mp: 10, maxmp: 20 });
    assert.equal(events[0].text, "45H 80E 10M >");
    assert.equal(shape.prompt, true);
    assert.equal(shape.other, false);
});

test("an editing prompt has no bars", () => {
    const { events } = P.classify("<div class='prompt'>]</div>");
    assert.equal(events[0].bars, null);
});

test("room title gives zone, exits, coordinates and id; the immortal suffix is dropped", () => {
    const { events, shape } = P.classify(ROOM);
    assert.deepEqual(events[0], { type: "room", title: "The Market Square", zone: "midgaard", exits: ["n", "e", "s", "w"], map: { x: 480, y: 505 }, roomid: "market_sq@midgaard" });
    assert.equal(shape.other, true);
});

test("a plain <h1> (help pages, wiz-eye) is not a room", () => {
    assert.equal(P.classify("<h1>Combat</h1>").events.length, 0);
    assert.equal(P.classify("<h1 class='room_title'>Far away</h1>").events.length, 0);
});

test("tells are read from text when the server sends no attributes", () => {
    const [a] = P.classify("<div class='tell_other'>Moira tells you 'meet at the Boar?'</div><br/>").events;
    assert.equal(a.kind, "tell");
    assert.equal(a.self, false);
    assert.equal(a.from, "Moira");
    assert.equal(a.body, "meet at the Boar?");
    const [b] = P.classify("<div class='tell_self'>You tell Moira 'on my way'</div>").events;
    assert.equal(b.self, true);
    assert.equal(b.to, "Moira");
    assert.equal(b.from, "You");
    assert.equal(b.body, "on my way");
});

test("server attributes win over the text", () => {
    const [a] = P.classify("<div class='tell_other' from='Moira'>The hooded figure tells you 'psst'</div>").events;
    assert.equal(a.from, "Moira");
    const [c] = P.classify("<div class='comm_say' chan='gossip' from='Thorin'>&lt;gossip&gt; Thorin says 'lantern?'</div>").events;
    assert.equal(c.chan, "gossip");
    assert.equal(c.from, "Thorin");
    assert.equal(c.body, "lantern?");
});

test("channel lines without attributes", () => {
    const [c] = P.classify("<div class='comm_say'>&lt;gossip&gt; Thorin says 'it's dark in here'</div>").events;
    assert.equal(c.kind, "channel");
    assert.equal(c.chan, "gossip");
    assert.equal(c.from, "Thorin");
    assert.equal(c.body, "it's dark in here");
    const [j] = P.classify("<div class='comm_join'>&lt;gossip&gt; Thorin has joined the channel.</div>").events;
    assert.equal(j.system, true);
});

test("say lines and nested whisper divs give one event", () => {
    const [s] = P.classify("<div class='say_other'>The town crier says 'Hear ye!'</div>").events;
    assert.equal(s.from, "The town crier");
    const ev = P.classify("<div class='whisper'>Moira whispers to you, <div class='whisper'>'psst'</div></div>").events;
    assert.equal(ev.length, 1);
    assert.equal(ev[0].from, "Moira");
    assert.equal(ev[0].body, "psst");
});

test("a segment of only combat lines has the combat shape", () => {
    const html = "<div class='hit_me'>You cut the rat.</div><br/><div class='hit_opponent'>The rat bites your leg.</div><br/>";
    const { events, shape } = P.classify(html);
    assert.deepEqual(events.map(e => [e.what, e.who]), [["hit", "me"], ["hit", "opponent"]]);
    assert.equal(shape.combat, true);
    assert.equal(P.classify(html + "You feel better.").shape.combat, false);
});

test("blank segments", () => {
    assert.equal(P.classify("<br/>").shape.blank, true);
    assert.equal(P.classify("  <br/>\n").shape.blank, true);
    assert.equal(P.classify("x").shape.blank, false);
});

/* ---------- data and script ---------- */

test("$who items: full list marker, entering and leaving players", () => {
    const ev = P.dataEvent("$who", "<li>*</li><li>+Papi Admin Builder</li><li>-Dwili</li>");
    assert.deepEqual(ev.items, ["*", "+Papi Admin Builder", "-Dwili"]);
});

test("alias / trigger / var lists", () => {
    const ev = P.dataEvent("$alias", "<li>k kill $1</li><li>ga get all;look</li>");
    assert.deepEqual(ev.items, ["k kill $1", "ga get all;look"]);
});

test("$combat JSON", () => {
    const ev = P.dataEvent("$combat", "{\"round\":3,\"list\":[{\"id\":\"a\",\"name\":\"the rat\",\"hp\":50,\"target\":\"you\",\"me\":false,\"foe\":true}]}");
    assert.equal(ev.json.list[0].name, "the rat");
    assert.equal(P.dataEvent("$combat", "{oops").error, "bad JSON");
});

test("only PasswordOn / PasswordOff scripts are understood", () => {
    assert.deepEqual(P.scriptEvent("PasswordOn()"), { type: "password", on: true });
    assert.deepEqual(P.scriptEvent("PasswordOff('Papi', 'Valhalla')"), { type: "password", on: false, name: "Papi", mud: "Valhalla" });
    assert.deepEqual(P.scriptEvent("PasswordOff('', 'Valhalla')"), { type: "password", on: false, name: "", mud: "Valhalla" });
    assert.equal(P.scriptEvent("alert(document.cookie)").type, "script");
    assert.equal(P.scriptEvent("PasswordOn();alert(1)").type, "script");
});

test("Protocol.feed runs the whole pipeline", () => {
    const p = new P.Protocol();
    const out = p.feed("<script>PasswordOff('Papi', 'Valhalla')</script>" + PROMPT);
    assert.equal(out[0].type, "password");
    assert.equal(out[1].type, "html");
    assert.equal(out[1].events[0].type, "prompt");
});

/* ---------- engine ---------- */

test("History skips movement and repeats and browses both ways", () => {
    const h = new E.History(3);
    assert.equal(h.add("n"), false);
    assert.equal(h.add("look"), true);
    assert.equal(h.add("look"), false);
    for (const c of ["kill rat", "kill rat", "say hi", "score"]) h.add(c);
    assert.deepEqual(h.items, ["kill rat", "say hi", "score"]);
    assert.equal(h.up(), "score");
    assert.equal(h.up(), "say hi");
    assert.equal(h.down(), "score");
    assert.equal(h.down(), "");
});

test("splitCommands honours \\;", () => {
    assert.deepEqual(E.splitCommands("n; e ;say a\\;b;;"), ["n", "e", "say a;b"]);
});

test("aliases expand with arguments, recursively, with a depth limit", () => {
    const e = new E.Engine();
    e.setAliases(["k kill $1;look", "kk k $1;k $2", "loop loop"]);
    assert.deepEqual(e.expand("kk rat dog").commands, ["kill rat", "look", "kill dog", "look"]);
    assert.deepEqual(e.expand("K Rat").commands, ["kill Rat", "look"]);
    assert.equal(e.expand("loop").errors.length, 1);
});

test("triggers fire with capture groups, use @vars and rate-limit", () => {
    let now = 0;
    const e = new E.Engine({ now: () => now });
    e.setVariables(["friend Moira"]);
    assert.deepEqual(e.setTriggers(["hi /^(@friend) has arrived\\.$/ wave $1", "bad /x/", "re /(/ x"]).length, 2);
    assert.deepEqual(e.checkTriggers("Moira has arrived.").commands, ["wave Moira"]);
    assert.deepEqual(e.checkTriggers("Thorin has arrived.").commands, []);
    for (let i = 0; i < 4; i++) e.checkTriggers("Moira has arrived.");
    assert.equal(e.checkTriggers("Moira has arrived.").errors.length, 1);
    now = 2000;
    assert.deepEqual(e.checkTriggers("Moira has arrived.").commands, ["wave Moira"]);
});

/* ---------- connection ---------- */

test("buildUrl", () => {
    assert.equal(C.buildUrl({ protocol: "wss", host: "valhalla.com", port: "4280", endpoint: "/echo" }), "wss://valhalla.com:4280/echo");
    assert.equal(C.buildUrl({ protocol: "ws", host: "localhost", port: "4280", endpoint: "" }), "ws://localhost:4280");
});

test("Connection sends the empty hello and strips newlines", () => {
    const sent = [];
    class FakeWS {
        constructor() {
            this.readyState = 1;
            setImmediate(() => this.onopen());
        }
        send(s) {
            sent.push(s);
        }
        close() {}
    }
    return new Promise(resolve => {
        const c = new C.Connection("ws://x", {
            onOpen() {
                assert.equal(c.send("say a\nb"), true);
                assert.deepEqual(sent, ["", "say a b"]);
                c.close();
                resolve();
            },
        }, { WebSocketImpl: FakeWS, autoReconnect: false });
        c.connect();
    });
});
