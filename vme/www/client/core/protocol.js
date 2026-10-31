/*
 * The DikuMUD3 web protocol, as the server and mplex send it over the websocket.
 *
 * Every websocket message is an arbitrary chunk of HTML. Inside the stream there are a few
 * in-band containers, which can span several messages (the server cuts frames at ~32KB):
 *   <paged>...</paged>              a page of text (help, who, score...)
 *   <data type='$x'>...</data>      structured data ($alias, $trigger, $var, $json, $combat)
 *   <script>...</script>            PasswordOn() / PasswordOff('name', 'mud') only
 * Ordinary HTML carries semantic classes: the prompt (<div class='prompt' bars='hp/max ep/max mp/max'>),
 * the room title (<h1 class='room_title' zone= exits= map= roomid=>), comm lines (tell_other...)
 * and combat lines (hit_me...). See PROTOCOL.md next to the client for the full list.
 *
 * Protocol.feed(frame) returns a list of events; it never touches the DOM.
 */
(function (root, factory) {
    const html = typeof module === "object" && module.exports ? require("./html.js") : root.Diku.html;
    const mod = factory(html);
    if (typeof module === "object" && module.exports) module.exports = mod;
    else (root.Diku = root.Diku || {}).protocol = mod;
})(typeof globalThis !== "undefined" ? globalThis : this, function (H) {
    "use strict";

    const BUFFER_LIMIT = 4 * 1024 * 1024; // an unterminated container is flushed as text past this

    const END_TAG = { paged: /<\/paged\s*>/i, data: /<\/data\s*>/i, script: /<\/script\s*>/i };
    const END_TAG_MAX = 10; // longest end tag without whitespace: </script>

    /* Splits the frame stream into html / paged / data / script segments. */
    class Framer {
        constructor() {
            this.mode = null;
            this.buf = "";
            this.dataType = "";
        }

        push(frame) {
            const out = [];
            // Inside a container only the new text (and an end tag split across frames) needs searching.
            let from = Math.max(0, this.buf.length - END_TAG_MAX);
            let s = this.buf + frame;
            this.buf = "";
            while (s.length) {
                if (this.mode) {
                    const end = END_TAG[this.mode].exec(from ? s.slice(from) : s);
                    const i = end ? end.index + from : -1;
                    from = 0;
                    if (i < 0) {
                        if (s.length > BUFFER_LIMIT) {
                            out.push({ type: "html", html: s });
                            this.mode = null;
                        } else {
                            this.buf = s;
                        }
                        return out;
                    }
                    const body = s.slice(0, i);
                    s = s.slice(i + end[0].length);
                    if (this.mode === "paged") out.push({ type: "paged", html: body });
                    else if (this.mode === "data") out.push({ type: "data", dtype: this.dataType, body });
                    else out.push({ type: "script", code: body });
                    this.mode = null;
                    continue;
                }
                const m = /<(paged)>|<(data)\b([^>]*)>|<(script)>/i.exec(s);
                if (!m) {
                    out.push({ type: "html", html: s });
                    break;
                }
                if (m.index > 0) out.push({ type: "html", html: s.slice(0, m.index) });
                s = s.slice(m.index + m[0].length);
                this.mode = (m[1] || m[2] || m[4]).toLowerCase();
                if (this.mode === "data") this.dataType = H.parseAttrs(m[3]).type || "";
            }
            return out;
        }
    }

    /* ---------- Classifying HTML ---------- */

    const COMBAT = /^(hit|miss|nodam|shield)_(me|opponent|other)$/;
    // class -> [kind, self]; self null means "work it out from the text"
    const COMM = {
        tell_self: ["tell", true], tell_other: ["tell", false],
        say_self: ["say", true], say_other: ["say", false],
        ask_self: ["ask", true], ask_other: ["ask", false],
        shout_self: ["shout", true], shout_other: ["shout", false],
        whisper: ["whisper", null],
        emote_self: ["emote", true], emote_other: ["emote", false],
        comm_say: ["channel", null], comm_emote: ["channel", null],
        comm_join: ["channel", null], comm_leave: ["channel", null],
    };

    // The words between the first and the last single quote: Name says 'the body'
    function quoted(text) {
        const a = text.indexOf("'"), b = text.lastIndexOf("'");
        return a >= 0 && b > a ? text.slice(a + 1, b) : null;
    }

    /*
     * A comm line. Server attributes (from=, to=, chan=) are used when present; otherwise the
     * names are read from the message text, which works with servers that don't send them yet.
     */
    function commEvent(cls, node) {
        const [kind, selfHint] = COMM[cls];
        const text = H.textOf(node).trim();
        const a = node.attrs;
        const ev = { type: "comm", kind, cls, text, self: false, from: a.from || "", to: a.to || "", chan: a.chan || "", body: "" };
        let t = text;
        if (kind === "channel") {
            const m = /^<([^>]+)>\s*([\s\S]*)$/.exec(t);
            if (m) {
                ev.chan = ev.chan || m[1].trim();
                t = m[2];
            }
            ev.system = cls === "comm_join" || cls === "comm_leave";
        }
        ev.self = selfHint !== null ? selfHint : /^You\b/.test(t) || (ev.from !== "" && ev.from === "You");
        const body = quoted(t);
        ev.body = body !== null ? body : t;
        if (kind === "tell") {
            if (ev.self && !ev.to) {
                const m = /^You (?:tell|respond to) (.+?)(?:,)? '/.exec(t);
                if (m) ev.to = m[1];
            } else if (!ev.self && !ev.from) {
                const m = /^(.+?) (?:tells|responds to) you/.exec(t);
                if (m) ev.from = m[1];
            }
        } else if (kind === "emote") {
            // "Papi grins and points at the fountain.": the action, without the name
            const who = ev.from || t.split(/\s+/)[0] || "";
            if (!ev.self) ev.from = who;
            ev.body = t.startsWith(who) ? t.slice(who.length).trim() : t;
        } else if (!ev.self && !ev.from) {
            const m = /^(.+?) (?:says|asks|shouts|exclaims|whispers|yells|emotes|tells)\b/.exec(t);
            if (m) ev.from = m[1];
            else if (body !== null) ev.from = t.slice(0, t.indexOf("'")).trim().split(/\s+/)[0] || "";
        }
        if (ev.self) ev.from = "You";
        return ev;
    }

    function promptEvent(node) {
        const ev = { type: "prompt", text: H.textOf(node).trim(), bars: null };
        const m = /^\s*(-?\d+)\/(-?\d+)\s+(-?\d+)\/(-?\d+)\s+(-?\d+)\/(-?\d+)\s*$/.exec(node.attrs.bars || "");
        if (m) {
            const n = m.slice(1).map(Number);
            ev.bars = { hp: n[0], maxhp: n[1], ep: n[2], maxep: n[3], mp: n[4], maxmp: n[5] };
        }
        return ev;
    }

    function roomEvent(node) {
        const a = node.attrs;
        let map = null;
        const m = /^\s*(-?\d+)\s*,\s*(-?\d+)/.exec(a.map || "");
        if (m) map = { x: Number(m[1]), y: Number(m[2]) };
        // Immortals see " [room@zone]" after the title; it is also in roomid.
        const title = H.textOf(node).replace(/\s*\[[^\]]*@[^\]]*\]\s*$/, "").trim();
        return { type: "room", title, zone: a.zone || "", exits: (a.exits || "").trim().split(/\s+/).filter(Boolean), map, roomid: a.roomid || "" };
    }

    /*
     * Classifies one HTML segment.
     * Returns {events, shape}. shape says what the segment's top level is made of:
     * blank (only whitespace / <br>), prompt, combat (combat lines only), other.
     */
    function classify(html) {
        const tree = H.parse(html);
        const events = [];
        const shape = { blank: false, prompt: false, combat: false, other: false };

        function visit(node, top) {
            if (node.text !== undefined) {
                if (top && H.hasText(node)) shape.other = true;
                return;
            }
            const cls = H.classes(node);
            if (cls.includes("prompt")) {
                events.push(promptEvent(node));
                if (top) shape.prompt = true;
                return;
            }
            if (node.tag === "h1" && cls.includes("room_title") && node.attrs.zone !== undefined) {
                events.push(roomEvent(node));
                if (top) shape.other = true;
                return;
            }
            const combat = cls.find(c => COMBAT.test(c));
            if (combat) {
                const [, what, who] = COMBAT.exec(combat);
                events.push({ type: "combat", cls: combat, what, who, text: H.textOf(node).trim() });
                if (top) shape.combat = true;
                return;
            }
            const comm = cls.find(c => COMM[c]);
            if (comm) {
                events.push(commEvent(comm, node));
                if (top) shape.other = true;
                return;
            }
            if (cls.includes("death")) events.push({ type: "death", text: H.textOf(node).trim() });
            if (cls.includes("xpgain")) events.push({ type: "xp", text: H.textOf(node).trim() });
            // Only something visible makes a segment "other"; <go-ahead/>, <mud-init/> and empty divs don't.
            if (top && (H.hasText(node) || /^(img|hr|table)$/.test(node.tag))) shape.other = true;
            for (const c of node.children) visit(c, false);
        }

        for (const c of tree.children) visit(c, true);
        shape.blank = !shape.prompt && !shape.combat && !shape.other;
        if (shape.other) shape.combat = false; // "combat" means combat lines and nothing else
        return { events, shape, tree };
    }

    /* ---------- Data and script containers ---------- */

    function parseJson(body) {
        try {
            return JSON.parse(body);
        } catch (e) {
            try {
                return JSON.parse(H.decodeEntities(body));
            } catch (e2) {
                return undefined;
            }
        }
    }

    function dataEvent(dtype, body) {
        if (dtype === "$alias" || dtype === "$trigger" || dtype === "$var" || dtype === "$who") {
            const items = [];
            (function walk(n) {
                if (n.tag === "li") {
                    items.push(H.textOf(n).trim());
                    return;
                }
                for (const c of n.children || []) walk(c);
            })(H.parse(body));
            return { type: "data", dtype, items: items.filter(Boolean) };
        }
        if (dtype === "$json" || dtype === "$combat") {
            const json = parseJson(body);
            if (json === undefined) return { type: "data", dtype, error: "bad JSON", body };
            return { type: "data", dtype, json };
        }
        return { type: "data", dtype, body };
    }

    const PW_OFF = /^\s*PasswordOff\(\s*(?:'((?:[^'\\]|\\.)*)'|"((?:[^"\\]|\\.)*)")?\s*(?:,\s*(?:'((?:[^'\\]|\\.)*)'|"((?:[^"\\]|\\.)*)"))?\s*\)\s*;?\s*$/;

    // Only these two calls are understood; any other script is reported and never run.
    function scriptEvent(code) {
        if (/^\s*PasswordOn\(\s*\)\s*;?\s*$/.test(code)) return { type: "password", on: true };
        const m = PW_OFF.exec(code);
        if (m) return { type: "password", on: false, name: m[1] ?? m[2] ?? "", mud: m[3] ?? m[4] ?? "" };
        return { type: "script", code };
    }

    /* ---------- The whole pipeline ---------- */

    /*
     * feed(frame) -> events, in order:
     *   {type:'html', html, tree, events:[...], shape}   ordinary output (events: prompt, room, comm, combat, death, xp)
     *   {type:'paged', html}                       a page of text
     *   {type:'data', dtype, ...}                  structured data
     *   {type:'password', on, name, mud}           login password mode on/off
     *   {type:'script', code}                      an unknown script (ignored by the clients)
     */
    class Protocol {
        constructor() {
            this.framer = new Framer();
        }

        feed(frame) {
            const out = [];
            for (const seg of this.framer.push(frame)) {
                if (seg.type === "html") {
                    out.push({ type: "html", html: seg.html, ...classify(seg.html) });
                } else if (seg.type === "paged") {
                    out.push(seg);
                } else if (seg.type === "data") {
                    out.push(dataEvent(seg.dtype, seg.body));
                } else {
                    out.push(scriptEvent(seg.code));
                }
            }
            return out;
        }
    }

    return { Framer, Protocol, classify, dataEvent, scriptEvent, quoted };
});
