/*
 * The visual web client: wires the shared core (protocol, engine, connection) to the page.
 *
 * Panels: output log, combat dock, input bar, vitals, room + compass, map, conversations,
 * reader (for <paged> text), connect screen and settings.
 */
(function () {
    "use strict";
    const D = window.Diku;
    const $ = s => document.querySelector(s);
    const $$ = s => [...document.querySelectorAll(s)];
    const root = document.documentElement;

    function el(tag, cls, text) {
        const e = document.createElement(tag);
        if (cls) e.className = cls;
        if (text != null) e.textContent = text;
        return e;
    }
    const cap = s => (s ? s.charAt(0).toUpperCase() + s.slice(1) : s);
    const hhmm = (d = new Date()) => d.toTimeString().slice(0, 5);

    /* ---------- Storage (per browser; may be unavailable) ---------- */
    const store = {
        get(k, def) {
            try {
                const v = localStorage.getItem(k);
                return v === null ? def : JSON.parse(v);
            } catch (e) {
                return def;
            }
        },
        set(k, v) {
            try {
                localStorage.setItem(k, JSON.stringify(v));
            } catch (e) { /* private mode or full: keep going without it */ }
        },
    };

    /* ---------- Settings ---------- */
    const settings = Object.assign({ skin: "auto", size: "m", fold: true, showPrompt: false, echo: true, stamps: true }, store.get("dk.settings", {}));

    const prefersLight = window.matchMedia("(prefers-color-scheme: light)");

    function applySettings() {
        root.dataset.skin = settings.skin !== "auto" ? settings.skin : prefersLight.matches ? "vellum" : "hall";
        root.dataset.size = settings.size;
        root.dataset.stamps = settings.stamps ? "on" : "off";
        $("#skin").value = settings.skin;
        $$("#sizes [data-size]").forEach(b => b.setAttribute("aria-pressed", String(b.dataset.size === settings.size)));
        $("#opt-fold").checked = settings.fold;
        $("#opt-prompt").checked = settings.showPrompt;
        $("#opt-echo").checked = settings.echo;
        $("#opt-stamps").checked = settings.stamps;
        store.set("dk.settings", settings);
    }

    /* ---------- Session state ---------- */
    const S = {
        conn: null,
        server: null,
        player: "",
        mud: "",
        password: false,
        vit: null,
    };
    const protocol = new D.protocol.Protocol();
    const engine = new D.engine.Engine();
    const history = new D.engine.History(200, store.get("dk.history", []));

    /* ======================================================================
     * Output log
     * ====================================================================== */
    const out = { el: $("#output"), pill: $("#newlines"), stick: true, unseen: 0, gap: false, group: null, scrollQueued: false };
    const MAX_ENTRIES = 3000;

    function atBottom() {
        return out.el.scrollHeight - out.el.scrollTop - out.el.clientHeight < 48;
    }
    out.el.addEventListener("scroll", () => {
        out.stick = atBottom();
        if (out.stick) {
            out.unseen = 0;
            out.pill.hidden = true;
        }
    });
    new ResizeObserver(() => {
        if (out.stick) out.el.scrollTop = out.el.scrollHeight;
    }).observe(out.el);
    out.pill.addEventListener("click", () => out.el.scrollTo({ top: out.el.scrollHeight, behavior: "smooth" }));

    function scrollToEnd() {
        if (out.scrollQueued) return;
        out.scrollQueued = true;
        requestAnimationFrame(() => {
            out.scrollQueued = false;
            if (out.stick) out.el.scrollTop = out.el.scrollHeight;
        });
    }

    function push(node) {
        if (out.gap && out.el.lastElementChild && !out.el.lastElementChild.classList.contains("gap")) out.el.append(el("div", "gap"));
        out.gap = false;
        node.dataset.t = hhmm();
        out.el.append(node);
        while (out.el.childElementCount > MAX_ENTRIES) out.el.firstElementChild.remove();
        if (out.stick) {
            scrollToEnd();
        } else {
            out.unseen++;
            out.pill.textContent = `${out.unseen} new ${out.unseen === 1 ? "message" : "messages"} ↓`;
            out.pill.hidden = false;
        }
        return node;
    }

    function sys(text, err) {
        return push(el("div", "seg sys" + (err ? " err" : ""), text));
    }

    /*
     * Zone maps and the server's images (<img src='../img/logo.gif'>) are in ../img in the
     * repository, where the client sits in www/client next to www/img. On valhalla.com the client
     * is served from /v3/ with the images in /v3/img, i.e. img/. Both are tried until an image
     * has loaded; after that only the folder that worked.
     */
    const IMG_DIRS = ["../img/", "img/"];
    let imgDir = null;

    function loadImg(img, file, onLoad, onFail) {
        let i = imgDir ?? 0;
        img.onload = () => {
            imgDir ??= i;
            if (onLoad) onLoad();
        };
        img.onerror = () => {
            if (imgDir === null && ++i < IMG_DIRS.length) img.src = IMG_DIRS[i] + file;
            else if (onFail) onFail();
        };
        img.src = IMG_DIRS[i] + file;
    }

    // Safe DOM for server HTML, in the box the server-class CSS applies to
    function serverHtml(html, cls = "seg") {
        const box = el("div", cls);
        const srv = el("div", "srv");
        srv.append(D.sanitize(html));
        for (const img of srv.querySelectorAll("img[src^='../img/']")) loadImg(img, img.getAttribute("src").slice(IMG_DIRS[0].length));
        box.append(srv);
        return box;
    }

    function echo(cmd) {
        const e = el("div", "seg echo");
        e.append(el("span", "gt", "›"), cmd);
        push(e);
    }

    function showHtml(ev) {
        const { shape } = ev;
        if (shape.blank) {
            out.gap = true;
            return;
        }
        if (shape.prompt && !shape.other && !shape.combat && !settings.showPrompt) return;
        if (shape.combat && settings.fold) {
            foldCombat(ev);
            return;
        }
        const seg = serverHtml(ev.html);
        if (!settings.showPrompt) seg.querySelectorAll(".prompt").forEach(p => p.remove());
        push(seg);
    }

    /* Combat lines fold into one summary line per fight, which updates as the fight goes on. */
    function foldCombat(ev) {
        const now = Date.now();
        let g = out.group;
        if (!g || g.closed || now - g.last > 20000) {
            g = out.group = { closed: false, last: now, hit: 0, hurt: 0, miss: 0, other: 0, details: el("details", "seg cgroup") };
            const sum = el("summary");
            g.sum = sum;
            g.details.append(sum);
            push(g.details);
        }
        g.last = now;
        g.details.append(serverHtml(ev.html));
        for (const c of ev.events) {
            if (c.type !== "combat") continue;
            if (c.what === "hit" && c.who === "me") g.hit++;
            else if (c.what === "hit" && c.who === "opponent") g.hurt++;
            else if (c.who === "other") g.other++;
            else g.miss++;
        }
        renderGroupSummary(g);
    }

    function renderGroupSummary(g) {
        const foes = [...combat.rows.values()].filter(r => r.foe).map(r => r.name);
        g.sum.replaceChildren(el("span", "ctag", "Combat"));
        if (foes.length) g.sum.append(el("span", "cfoes", foes.slice(0, 3).join(", ") + (foes.length > 3 ? ` +${foes.length - 3}` : "")));
        g.sum.append(el("span", "t-hit", `you hit ${g.hit}×`), el("span", "t-hurt", `hit you ${g.hurt}×`), el("span", "t-miss", `misses ${g.miss}`));
        if (g.other) g.sum.append(el("span", "t-miss", `others ${g.other}`));
    }

    function closeCombatGroup() {
        if (out.group) out.group.closed = true;
    }

    /* Paged text goes into the log; long pages are folded with a button to read them in full. */
    function showPaged(html) {
        const box = el("div", "seg paged");
        const body = serverHtml(html, "paged-body");
        box.append(body);
        push(box);
        requestAnimationFrame(() => {
            if (body.scrollHeight <= out.el.clientHeight * 0.6 || body.scrollHeight < 300) return;
            box.classList.add("long");
            const tools = el("div", "paged-tools");
            const read = el("button", "btn", "Read in full");
            read.type = "button";
            read.addEventListener("click", () => openReader(html));
            const expand = el("button", "btn", "Show here");
            expand.type = "button";
            expand.addEventListener("click", () => {
                box.classList.remove("long");
                tools.remove();
            });
            tools.append(read, expand);
            box.append(tools);
            scrollToEnd();
        });
    }

    /* ======================================================================
     * Overlays: reader, big map, connect screen
     * ====================================================================== */
    const scrim = $("#scrim");

    function openScrim(node) {
        scrim.replaceChildren(node);
        scrim.hidden = false;
    }

    function closeScrim() {
        if (scrim.hidden) return;
        scrim.hidden = true;
        scrim.replaceChildren();
        focusInput();
    }
    scrim.addEventListener("click", e => {
        if (e.target === scrim || e.target.closest("[data-close]")) closeScrim();
    });

    function openReader(html) {
        const r = el("article", "panel reader");
        r.setAttribute("role", "dialog");
        r.setAttribute("aria-label", "Text");
        const x = el("button", "btn close", "Close");
        x.type = "button";
        x.dataset.close = "";
        r.append(x, serverHtml(html));
        openScrim(r);
        x.focus();
    }

    /* ======================================================================
     * Vitals
     * ====================================================================== */
    function vitalRow(key, label) {
        const v = el("div", "vital");
        v.dataset.vital = key;
        const bar = el("div", "bar");
        bar.append(el("i", "ghost"), el("i", "fill"));
        v.append(el("label", null, label), bar, el("span", "num", "–"));
        return v;
    }
    $$("[data-vitals]").forEach(box => {
        if (box.classList.contains("card")) {
            const h = el("div", "eyebrow");
            h.append(el("span", null, "Vitals"), el("span", "vit-name", ""));
            box.append(h);
        }
        box.append(vitalRow("hp", "HP"), vitalRow("ep", "End"), vitalRow("mp", "Mana"));
    });

    function renderVitals(b) {
        const prev = S.vit;
        S.vit = b;
        for (const [k, cur, max] of [["hp", b.hp, b.maxhp], ["ep", b.ep, b.maxep], ["mp", b.mp, b.maxmp]]) {
            const delta = prev ? cur - prev[k] : 0;
            const pct = max > 0 ? Math.max(0, Math.min(100, 100 * cur / max)) + "%" : "0%";
            $$(`[data-vital="${k}"]`).forEach(v => {
                v.querySelector(".fill").style.width = pct;
                v.querySelector(".ghost").style.width = pct;
                const num = v.querySelector(".num");
                num.replaceChildren(String(cur), el("small", null, "/" + max));
                if (k === "hp") v.classList.toggle("low", max > 0 && cur / max < 0.3);
                if (delta) {
                    const d = el("span", "delta" + (delta > 0 ? " plus" : ""), (delta > 0 ? "+" : "−") + Math.abs(delta));
                    v.append(d);
                    setTimeout(() => d.remove(), 1200);
                }
            });
        }
    }

    /* ======================================================================
     * Room, compass and map
     * ====================================================================== */
    const DIRNAMES = D.engine.DIRNAMES;

    function showRoom(r) {
        closeCombatGroup();
        $("#rctitle").textContent = r.title || "Somewhere";
        $("#rcsub").textContent = [cap(r.zone), r.exits.length ? `${r.exits.length} ${r.exits.length === 1 ? "exit" : "exits"}` : "no obvious exits"].filter(Boolean).join(" · ");
        $("#rcid").textContent = r.roomid || "";
        $("#where").textContent = r.title + (r.zone ? " · " + cap(r.zone) : "");
        $$("#compass [data-dir]").forEach(b => {
            const open = r.exits.includes(b.dataset.dir);
            b.classList.toggle("open", open);
            b.disabled = !open;
            b.title = open ? DIRNAMES[b.dataset.dir] : "";
        });
        setMap(r.zone, r.map);
    }

    const map = { zone: null, ok: false, w: 0, h: 0, pos: null };
    const mapImg = $("#mapimg");

    function setMap(zone, pos) {
        map.pos = pos;
        $("#mapxy").textContent = pos ? `${pos.x}, ${pos.y}` : "";
        if (zone !== map.zone) {
            map.zone = zone;
            map.ok = false;
            mapImg.hidden = true;
            $("#mapyou").hidden = true;
            $("#mapzone").textContent = cap(zone || "");
            $("#nomap").textContent = zone ? "Loading the map…" : "The map appears when you walk somewhere that has one.";
            $("#nomap").hidden = false;
            if (zone) {
                const want = zone;
                loadImg(mapImg, encodeURIComponent(zone) + ".jpg", () => {
                    if (map.zone !== want) return;
                    map.ok = true;
                    map.w = mapImg.naturalWidth;
                    map.h = mapImg.naturalHeight;
                    mapImg.style.width = map.w + "px";
                    mapImg.style.height = map.h + "px";
                    mapImg.hidden = false;
                    $("#nomap").hidden = true;
                    placeMap();
                }, () => {
                    if (map.zone === want) $("#nomap").textContent = `There is no map of ${cap(want)}.`;
                });
            }
        }
        placeMap();
    }

    function placeMap() {
        if (!map.ok) return;
        const v = $("#mapview");
        const vw = v.clientWidth, vh = v.clientHeight;
        if (!vw) return;
        const you = $("#mapyou");
        if (map.pos) {
            const s = 1.15;
            mapImg.style.transform = `translate(${vw / 2 - map.pos.x * s}px, ${vh / 2 - map.pos.y * s}px) scale(${s})`;
            you.hidden = false;
        } else {
            const s = Math.min(vw / map.w, vh / map.h);
            mapImg.style.transform = `translate(${(vw - map.w * s) / 2}px, ${(vh - map.h * s) / 2}px) scale(${s})`;
            you.hidden = true;
        }
        const big = scrim.querySelector(".mapbig .you");
        if (big && map.pos) {
            big.style.left = (100 * map.pos.x / map.w) + "%";
            big.style.top = (100 * map.pos.y / map.h) + "%";
        }
    }
    new ResizeObserver(placeMap).observe($("#mapview"));
    $("#mapview").addEventListener("click", () => {
        if (!map.ok) return;
        const m = el("div", "mapbig");
        const img = el("img");
        img.src = mapImg.src;
        img.alt = "Map of " + cap(map.zone);
        m.append(img);
        if (map.pos) m.append(el("span", "you"));
        m.dataset.close = "";
        openScrim(m);
        placeMap();
    });

    /* ======================================================================
     * Combat dock: one card per being in the fight
     * ====================================================================== */
    const combat = { rows: new Map(), deadOrder: 0, hasData: false, active: false, round: 0, lines: [], hideTimer: null, idleTimer: null };
    const dock = $("#combat");

    function condition(p) {
        return p >= 100 ? "unhurt" : p > 90 ? "scratched" : p > 70 ? "lightly wounded" : p > 50 ? "wounded" : p > 30 ? "badly wounded" : p > 10 ? "bleeding heavily" : p > 0 ? "near death" : "dead";
    }

    // The fight is over: show that, then hide the dock and forget the fight after a while.
    function endCombat(delay) {
        combat.active = false;
        closeCombatGroup();
        scheduleDock();
        clearTimeout(combat.hideTimer);
        combat.hideTimer = setTimeout(() => {
            dock.hidden = true;
            combat.rows.clear();
            combat.lines = [];
        }, delay);
    }

    function combatData(j) {
        combat.hasData = true;
        if (j.event === "death") {
            const r = combat.rows.get(j.id);
            if (r) {
                r.dead = true;
                r.hp = 0;
                r.deadAt = ++combat.deadOrder;
            }
            if (combat.active) scheduleDock();
            else endCombat(6000); // the death can follow the end of the fight
            return;
        }
        if (j.end || !Array.isArray(j.list) || !j.list.length) {
            endCombat(6000);
            return;
        }
        clearTimeout(combat.hideTimer);
        combat.active = true;
        combat.round = j.round || combat.round + 1;
        const seen = new Set();
        for (const c of j.list) {
            if (c.me) continue;
            seen.add(c.id);
            const r = combat.rows.get(c.id) || { id: c.id, last: "", dead: false };
            r.prevHp = r.hp ?? c.hp;
            Object.assign(r, { name: c.name, hp: c.hp, target: c.target || "", foe: !!c.foe });
            combat.rows.set(c.id, r);
        }
        for (const [id, r] of combat.rows) if (!seen.has(id) && !r.dead) combat.rows.delete(id); // fled or left
        scheduleDock();
    }

    // Shows a combat line on the card of the being it is about (by name), or in the plain list.
    function combatLine(c) {
        if (combat.hasData) {
            const t = c.text.toLowerCase();
            let best = null;
            for (const r of combat.rows.values()) {
                const n = r.name.toLowerCase().replace(/^(the|a|an) /, "");
                if (n && t.includes(n) && (!best || n.length > best.n)) best = { r, n: n.length };
            }
            if (best) {
                best.r.last = c.text;
                best.r.lastCls = c.cls;
                best.r.flash = Date.now();
                scheduleDock();
            }
            return;
        }
        // No $combat data from this server: show the latest lines, not a scrolling log.
        clearTimeout(combat.hideTimer);
        combat.active = true;
        combat.lines.push(c);
        combat.lines = combat.lines.slice(-4);
        clearTimeout(combat.idleTimer);
        combat.idleTimer = setTimeout(() => endCombat(4000), 8000);
        scheduleDock();
    }

    // A round brings many lines and a packet; draw the dock once per frame.
    function scheduleDock() {
        if (combat.drawQueued) return;
        combat.drawQueued = true;
        requestAnimationFrame(() => {
            combat.drawQueued = false;
            renderDock();
        });
    }

    function renderDock() {
        dock.hidden = false;
        dock.classList.toggle("over", !combat.active);
        const head = el("div", "cb-head");
        const tag = el("span", "cb-tag");
        tag.append(el("i"), combat.active ? "Fighting" : "Combat over");
        head.append(tag, el("span", "cb-round", combat.hasData && combat.round ? `Round ${combat.round}` : ""));
        if (combat.active) {
            const flee = el("button", "btn warn", "Flee");
            flee.type = "button";
            flee.dataset.cmd = "flee";
            head.append(flee);
        }
        const parts = [head];
        if (combat.hasData) {
            const rows = [...combat.rows.values()].sort((a, b) => rank(a) - rank(b) || (a.deadAt || 0) - (b.deadAt || 0));
            const grid = el("div", "cb-grid");
            for (const r of rows) grid.append(card(r));
            parts.push(grid);
        } else {
            const ls = el("div", "cb-lines");
            for (const c of combat.lines) ls.append(el("div", c.cls, c.text));
            parts.push(ls);
        }
        dock.replaceChildren(...parts);
    }

    // Attacking you first, then other foes, then allies, then the dead.
    function rank(r) {
        if (r.dead) return 4;
        if (r.foe && r.target === "you") return 0;
        if (r.foe) return 1;
        return 2;
    }

    function card(r) {
        const c = el("div", "cb-row" + (r.foe ? "" : " ally") + (r.dead ? " dead" : "") + (!r.dead && r.foe && r.target === "you" ? " at-me" : ""));
        const name = el("div", "cb-name");
        name.append(el("b", null, cap(r.name)));
        name.append(el("span", "cb-target", r.dead ? "dead" : r.target ? "→ " + r.target : ""));
        const hp = el("div", "cb-hp");
        const cond = el("div", "cond");
        const ghost = el("i", "ghost"), fill = el("i", "fill");
        ghost.style.width = (r.prevHp ?? r.hp) + "%";
        fill.style.width = r.hp + "%";
        cond.append(ghost, fill);
        hp.append(cond, el("span", null, `${r.hp}% · ${condition(r.hp)}`));
        requestAnimationFrame(() => {
            ghost.style.width = r.hp + "%";
        });
        const last = el("div", "cb-last" + (r.flash && Date.now() - r.flash < 1500 ? " flash" : ""));
        if (r.last) last.append(el("span", r.lastCls, r.last));
        c.append(name, hp, last);
        return c;
    }

    /* ======================================================================
     * Conversations
     * ====================================================================== */
    const AVATAR_COLOURS = ["#c7a5f2", "#8cbde0", "#9fd3c8", "#f1c56f", "#ef8f7a", "#a8d98a"];
    const chat = { threads: new Map(), active: null, saveTimer: null };

    // The conversation a comm line belongs to: one per person, per channel, plus Shouts and Room.
    function threadFor(ev) {
        let base;
        if (ev.kind === "tell") {
            const who = (ev.self ? ev.to : ev.from) || "Someone";
            base = { id: "tell:" + who.toLowerCase(), kind: "tell", name: cap(who), sub: "tells", cmd: "tell " + who.split(/\s+/)[0].toLowerCase() };
        } else if (ev.kind === "channel") {
            const ch = (ev.chan || "channel").toLowerCase();
            base = { id: "chan:" + ch, kind: "chan", name: cap(ch), sub: "channel", cmd: ch };
        } else if (ev.kind === "shout") {
            base = { id: "shout", kind: "shout", name: "Shouts", sub: "heard far and wide", cmd: "shout" };
        } else {
            base = { id: "room", kind: "room", name: "Room", sub: "say, ask and whisper", cmd: "say" };
        }
        return chat.threads.get(base.id) || { ...base, msgs: [], unread: 0 };
    }

    // Same breakpoints as client.css: three columns, then a side switch, then phone tabs.
    const wide = window.matchMedia("(min-width: 1280px)"), phone = window.matchMedia("(max-width: 820px)");

    function chatsVisible() {
        if (document.hidden) return false;
        if (wide.matches) return true;
        return phone.matches ? $("#app").dataset.view === "chats" : $("#app").dataset.side === "chats";
    }

    function addComm(ev) {
        const t = threadFor(ev);
        chat.threads.set(t.id, t);
        const m = { who: ev.self ? "You" : ev.from || "Someone", text: ev.body || ev.text, t: hhmm(), system: !!ev.system };
        if (ev.system) m.text = ev.text.replace(/^<[^>]+>\s*/, "");
        t.msgs.push(m);
        if (t.msgs.length > 150) t.msgs.splice(0, t.msgs.length - 150);
        t.last = Date.now();
        if (!chat.active) chat.active = t.id;
        if (t.id === chat.active && chatsVisible()) {
            appendMsg(m);
        } else if (!ev.self && !ev.system) {
            t.unread++;
        }
        scheduleThreads();
        saveChats();
    }

    function avatar(t) {
        if (t.kind !== "tell") return el("span", "av grp", { room: "Say", chan: "#", shout: "!" }[t.kind] || "·");
        const a = el("span", "av", t.name.charAt(0));
        a.style.background = AVATAR_COLOURS[[...t.name].reduce((n, c) => n + c.charCodeAt(0), 0) % AVATAR_COLOURS.length];
        return a;
    }

    // Busy channels bring several lines a second; redraw the list once per frame.
    function scheduleThreads() {
        if (chat.drawQueued) return;
        chat.drawQueued = true;
        requestAnimationFrame(() => {
            chat.drawQueued = false;
            renderThreads();
        });
    }

    function renderThreads() {
        const ul = $("#threads");
        const list = [...chat.threads.values()].sort((a, b) => (b.last || 0) - (a.last || 0));
        ul.replaceChildren();
        if (!list.length) ul.append(el("li", "empty", "Tells, says and channel talk collect here, one conversation per person or channel."));
        for (const t of list) {
            const last = t.msgs[t.msgs.length - 1];
            const b = el("button", "thr");
            b.type = "button";
            b.dataset.thread = t.id;
            b.setAttribute("aria-current", String(t.id === chat.active));
            const main = el("span", "thr-main");
            const nm = el("span", "thr-name", t.name);
            nm.append(el("small", null, t.sub));
            const prev = last ? (last.who === "You" ? "You: " : t.kind === "tell" ? "" : last.who + ": ") + last.text : "";
            main.append(nm, el("span", "thr-prev", prev));
            const meta = el("span", "thr-meta");
            meta.append(el("span", null, last ? last.t : ""));
            if (t.unread) meta.append(el("span", "badge", String(t.unread)));
            b.append(avatar(t), main, meta);
            const li = el("li");
            li.append(b);
            ul.append(li);
        }
        const total = list.reduce((n, t) => n + t.unread, 0);
        $$("[data-unread]").forEach(b => {
            b.textContent = String(total);
            b.hidden = !total;
        });
        updateTitle(total);
        $("#thread").hidden = !chat.active;
    }

    function msgNode(m) {
        const d = el("div", "msg" + (m.who === "You" ? " self" : "") + (m.system ? " system" : ""));
        const meta = el("div", "meta");
        meta.append(el("b", null, m.who), el("time", null, m.t));
        d.append(meta, el("p", null, m.text));
        return d;
    }

    function appendMsg(m) {
        const box = $("#msgs");
        box.append(msgNode(m));
        box.scrollTop = box.scrollHeight;
    }

    function openThread(id) {
        const t = chat.threads.get(id);
        if (!t) return;
        const changed = chat.active !== id || t.unread > 0;
        chat.active = id;
        t.unread = 0;
        $("#thname").textContent = t.name;
        $("#thsub").textContent = t.sub;
        $("#prefix").textContent = t.cmd;
        const box = $("#msgs");
        box.replaceChildren(...t.msgs.map(msgNode));
        box.scrollTop = box.scrollHeight;
        renderThreads();
        if (changed) saveChats();
    }

    // Conversations are kept per character; written a few seconds after a change, and on leaving the page.
    function writeChats() {
        clearTimeout(chat.saveTimer);
        chat.saveTimer = null;
        if (!S.player) return;
        const keep = [...chat.threads.values()].sort((a, b) => (b.last || 0) - (a.last || 0)).slice(0, 40)
            .map(t => ({ ...t, msgs: t.msgs.slice(-100) }));
        store.set("dk.chats." + S.player.toLowerCase(), { active: chat.active, threads: keep });
    }

    function saveChats() {
        if (!chat.saveTimer) chat.saveTimer = setTimeout(writeChats, 5000);
    }
    window.addEventListener("pagehide", () => {
        if (chat.saveTimer) writeChats();
    });

    function loadChats() {
        const saved = store.get("dk.chats." + S.player.toLowerCase(), null);
        chat.threads.clear();
        chat.active = null;
        if (saved && Array.isArray(saved.threads)) {
            for (const t of saved.threads) chat.threads.set(t.id, t);
            chat.active = chat.threads.has(saved.active) ? saved.active : null;
        }
        if (chat.active) openThread(chat.active);
        else renderThreads();
    }

    $("#threads").addEventListener("click", e => {
        const b = e.target.closest("[data-thread]");
        if (b) {
            openThread(b.dataset.thread);
            $("#reply").focus();
        }
    });
    $("#replyform").addEventListener("submit", e => {
        e.preventDefault();
        const v = $("#reply").value.trim();
        const t = chat.threads.get(chat.active);
        if (!v || !t) return;
        runLine(t.cmd + " " + v);
        $("#reply").value = "";
    });

    function updateTitle(unread = 0) {
        const name = S.player ? `${S.player} · ${S.mud || "DikuMUD"}` : S.mud || "DikuMUD";
        document.title = (unread ? `(${unread}) ` : "") + name;
    }

    /* ======================================================================
     * Server events
     * ====================================================================== */
    function onFrame(frame) {
        for (const ev of protocol.feed(frame)) {
            try {
                handle(ev);
            } catch (e) {
                console.error("Could not show server output", e, ev);
            }
        }
    }

    function handle(ev) {
        switch (ev.type) {
        case "html":
            showHtml(ev);
            for (const e of ev.events) {
                if (e.type === "prompt") {
                    if (e.bars) renderVitals(e.bars);
                    $("#ptext").textContent = e.text || "›";
                } else if (e.type === "room") {
                    showRoom(e);
                } else if (e.type === "comm") {
                    addComm(e);
                } else if (e.type === "combat") {
                    combatLine(e);
                }
            }
            if (!S.password && engine.triggers.length) runTriggers(D.html.textOf(ev.tree));
            break;
        case "paged":
            showPaged(ev.html);
            break;
        case "password":
            setPassword(ev);
            break;
        case "data":
            if (ev.dtype === "$alias") engine.setAliases(ev.items);
            else if (ev.dtype === "$var") engine.setVariables(ev.items).forEach(m => sys(m, true));
            else if (ev.dtype === "$trigger") engine.setTriggers(ev.items).forEach(m => sys(m, true));
            else if (ev.dtype === "$combat" && ev.json) combatData(ev.json);
            break;
        default:
            break; // other scripts are never run
        }
    }

    function runTriggers(text) {
        const { commands, errors } = engine.checkTriggers(text);
        errors.forEach(m => sys(m, true));
        commands.forEach(c => send(c, true));
    }

    function setPassword(ev) {
        S.password = ev.on;
        const input = $("#cmd");
        input.type = ev.on ? "password" : "text";
        input.placeholder = ev.on ? "Password" : "Type a command";
        $("#cmdform").classList.toggle("pw", ev.on);
        if (!ev.on) {
            if (ev.mud) {
                S.mud = ev.mud;
                $("#mudname").textContent = ev.mud;
            }
            if (ev.name && ev.name !== S.player) {
                if (chat.saveTimer) writeChats(); // the previous character's conversations
                S.player = cap(ev.name);
                $("#plyname").textContent = S.player;
                $$(".vit-name").forEach(n => (n.textContent = S.player));
                loadChats();
            }
        }
        updateTitle();
        focusInput();
    }

    /* ======================================================================
     * Sending commands
     * ====================================================================== */
    function send(cmd, fromTrigger) {
        if (!S.conn || !S.conn.isOpen) {
            sys("Not connected. Use Reconnect, or pick a server under Settings.", true);
            return false;
        }
        if (settings.echo && !S.password) echo(fromTrigger ? cmd + "  (trigger)" : cmd);
        return S.conn.send(cmd);
    }

    // A typed line: history, aliases and ';' splitting. The password is sent exactly as typed.
    function runLine(line) {
        if (S.password) {
            if (S.conn && S.conn.isOpen) S.conn.send(line);
            return;
        }
        if (!line.trim()) {
            send("");
            return;
        }
        if (history.add(line)) store.set("dk.history", history.items);
        const { commands, errors } = engine.expand(line);
        errors.forEach(m => sys(m, true));
        commands.forEach(c => send(c));
        if (!out.stick) {
            out.stick = true;
            scrollToEnd();
        }
    }

    const input = $("#cmd");

    function focusInput() {
        if (window.matchMedia("(max-width: 820px)").matches && document.activeElement === document.body) return; // no surprise keyboards on phones
        if (document.activeElement !== input && scrim.hidden) input.focus({ preventScroll: true });
    }

    $("#cmdform").addEventListener("submit", e => {
        e.preventDefault();
        const v = input.value;
        input.value = "";
        runLine(v);
    });

    const NUMPAD = { 1: "sw", 2: "s", 3: "se", 4: "w", 5: "look", 6: "e", 7: "nw", 8: "n", 9: "ne", 0: "d", Decimal: "u" };
    input.addEventListener("keydown", e => {
        if (S.password) return;
        if (e.key === "ArrowUp" || e.key === "ArrowDown") {
            e.preventDefault();
            input.value = e.key === "ArrowUp" ? history.up() : history.down();
            input.setSelectionRange(input.value.length, input.value.length);
        } else if (e.key === "PageUp" || e.key === "PageDown") {
            e.preventDefault();
            out.el.scrollBy({ top: (e.key === "PageUp" ? -0.85 : 0.85) * out.el.clientHeight });
        } else if (e.key === "Escape") {
            if (input.value) {
                input.value = "";
                history.reset();
                e.stopPropagation();
            }
        } else if (e.location === 3 && input.value === "" && !e.ctrlKey && !e.altKey && !e.metaKey) {
            const k = e.code.replace("Numpad", "");
            if (NUMPAD[k]) {
                e.preventDefault();
                runLine(NUMPAD[k]);
            }
        }
    });

    document.addEventListener("keydown", e => {
        if (e.key === "Escape") {
            if (!$("#settings").hidden) toggleSettings(false);
            else closeScrim();
            focusInput();
            return;
        }
        const t = e.target;
        if (e.key.length === 1 && !e.ctrlKey && !e.metaKey && !e.altKey && !/^(INPUT|SELECT|TEXTAREA)$/.test(t.tagName) && scrim.hidden) input.focus();
    });
    document.addEventListener("visibilitychange", () => {
        if (!document.hidden) {
            if (chat.active && chatsVisible()) openThread(chat.active);
            focusInput();
        }
    });

    // Every [data-cmd] / [data-dir] element runs its command: log links, chips, compass, dock.
    document.addEventListener("click", e => {
        const c = e.target.closest("[data-cmd], [data-dir]");
        if (!c || c.disabled || c.closest("#settings")) return;
        e.preventDefault();
        if (c.closest(".reader")) closeScrim();
        runLine(c.dataset.cmd || c.dataset.dir);
        if (!c.closest(".chats")) focusInput();
    });

    /* ======================================================================
     * Layout switches, settings
     * ====================================================================== */
    // Shows pane `value` ("play", "status", "chats"): the side switch on laptops sets data-side,
    // the phone tabs set data-view.
    function showPane(key, value) {
        $("#app").dataset[key] = value;
        $$(`[data-${key}]:not(#app)`).forEach(b => b.setAttribute("aria-pressed", String(b.dataset[key] === value)));
        if (value === "chats" && chat.active) openThread(chat.active);
        if (value === "status") placeMap();
        if (value === "play") scrollToEnd();
    }
    $$(".side-switch [data-side]").forEach(b => b.addEventListener("click", () => showPane("side", b.dataset.side)));
    $$(".tabs [data-view]").forEach(b => b.addEventListener("click", () => showPane("view", b.dataset.view)));

    function toggleSettings(show) {
        const s = $("#settings");
        s.hidden = show === undefined ? !s.hidden : !show;
        $("#settingsbtn").setAttribute("aria-expanded", String(!s.hidden));
    }
    $("#settingsbtn").addEventListener("click", e => {
        e.stopPropagation();
        toggleSettings();
    });
    document.addEventListener("click", e => {
        if (!$("#settings").hidden && !e.target.closest("#settings, #settingsbtn")) toggleSettings(false);
    });
    $("#skin").addEventListener("change", e => {
        settings.skin = e.target.value;
        applySettings();
    });
    $$("#sizes [data-size]").forEach(b => b.addEventListener("click", () => {
        settings.size = b.dataset.size;
        applySettings();
    }));
    for (const [id, key] of [["opt-fold", "fold"], ["opt-prompt", "showPrompt"], ["opt-echo", "echo"], ["opt-stamps", "stamps"]]) {
        $("#" + id).addEventListener("change", e => {
            settings[key] = e.target.checked;
            applySettings();
        });
    }
    $("#btn-server").addEventListener("click", () => {
        toggleSettings(false);
        showConnect();
    });
    $("#btn-disconnect").addEventListener("click", () => {
        toggleSettings(false);
        if (S.conn) S.conn.close();
    });
    $("#conn").addEventListener("click", () => {
        if (!S.conn || !S.conn.isOpen) showConnect();
    });

    /* ======================================================================
     * Connecting
     * ====================================================================== */
    const PRESETS = [
        { name: "Valhalla", protocol: "wss", host: "valhalla.com", port: "4280", endpoint: "/echo" },
        { name: "This computer", protocol: "ws", host: "localhost", port: "4280", endpoint: "/echo" },
    ];

    function serverFromUrl() {
        const q = new URLSearchParams(location.search);
        if (!q.get("host")) return null;
        return { protocol: q.get("protocol") === "ws" ? "ws" : "wss", host: q.get("host"), port: q.get("port") || "4280", endpoint: q.get("endpoint") || "/echo" };
    }

    function defaultServer() {
        if (/^https?:$/.test(location.protocol) && location.hostname && !/^(localhost|127\.)/.test(location.hostname)) {
            return { protocol: location.protocol === "https:" ? "wss" : "ws", host: location.hostname.replace(/^www\./, ""), port: "4280", endpoint: "/echo" };
        }
        return PRESETS[0];
    }

    function showConnect() {
        const form = $("#connect-tpl").content.firstElementChild.cloneNode(true);
        const cur = S.server || store.get("dk.server", null) || defaultServer();
        const fill = s => {
            form.querySelector("#f-protocol").value = s.protocol;
            form.querySelector("#f-host").value = s.host;
            form.querySelector("#f-port").value = s.port;
            form.querySelector("#f-endpoint").value = s.endpoint;
            form.querySelectorAll(".preset").forEach(b => b.setAttribute("aria-pressed", String(b.dataset.host === s.host && b.dataset.port === s.port)));
            const q = new URLSearchParams({ protocol: s.protocol, host: s.host, port: s.port, endpoint: s.endpoint, reader: "true" });
            form.querySelector("#readerlink").href = "client-a.html?" + q;
        };
        const presets = form.querySelector("#presets");
        for (const p of PRESETS) {
            const b = el("button", "preset");
            b.type = "button";
            b.dataset.host = p.host;
            b.dataset.port = p.port;
            b.append(el("b", null, p.name), el("small", null, D.connection.buildUrl(p)));
            b.addEventListener("click", () => fill(p));
            presets.append(b);
        }
        fill(cur);
        form.addEventListener("submit", e => {
            e.preventDefault();
            const s = {
                protocol: form.querySelector("#f-protocol").value,
                host: form.querySelector("#f-host").value.trim(),
                port: form.querySelector("#f-port").value.trim(),
                endpoint: form.querySelector("#f-endpoint").value.trim(),
            };
            if (!s.host) return;
            closeScrim();
            connect(s);
        });
        openScrim(form);
        form.querySelector(".btn.primary").focus();
    }

    function setConnState(state, text) {
        $("#conn").dataset.state = state;
        $("#connhost").textContent = text;
    }

    function banner(text, buttons) {
        const b = $("#banner");
        if (!text) {
            b.hidden = true;
            return;
        }
        b.replaceChildren(el("span", null, text));
        for (const [label, fn] of buttons || []) {
            const x = el("button", "btn", label);
            x.type = "button";
            x.addEventListener("click", fn);
            b.append(x);
        }
        b.hidden = false;
    }

    function connect(server) {
        if (S.conn) S.conn.close();
        S.server = server;
        store.set("dk.server", server);
        const url = D.connection.buildUrl(server);
        setConnState("connecting", server.host);
        banner("");
        sys(`Connecting to ${url}…`);
        let retryTimer = null;
        const conn = new D.connection.Connection(url, {
            onOpen() {
                setConnState("open", server.host);
                banner("");
                sys(`Connected to ${server.host}.`);
                focusInput();
            },
            onFrame,
            onClose({ willRetry, retryIn }) {
                if (S.conn !== conn) return;
                setConnState("closed", "Disconnected");
                setPassword({ on: false });
                combat.active = false;
                if (willRetry) {
                    let left = retryIn;
                    const tick = () => banner(`The connection was lost. Reconnecting in ${left} s.`, [["Reconnect now", () => conn.connect()], ["Change server", showConnect]]);
                    tick();
                    clearInterval(retryTimer);
                    retryTimer = setInterval(() => {
                        left--;
                        if (left <= 0) {
                            clearInterval(retryTimer);
                            setConnState("connecting", server.host);
                            banner("Reconnecting…");
                        } else {
                            tick();
                        }
                    }, 1000);
                } else {
                    clearInterval(retryTimer);
                    banner("Disconnected.", [["Reconnect", () => connect(server)], ["Change server", showConnect]]);
                }
            },
        });
        S.conn = conn;
        conn.connect();
    }

    /* ======================================================================
     * Start
     * ====================================================================== */
    // ?debug=1 lets the developer console feed server frames: DikuDebug.feed("<div class='hit_me'>…</div>")
    if (new URLSearchParams(location.search).get("debug") === "1") window.DikuDebug = { feed: onFrame };

    prefersLight.addEventListener("change", applySettings);
    applySettings();
    renderThreads();
    updateTitle();
    const start = serverFromUrl() || store.get("dk.server", null);
    if (start) connect(start);
    else showConnect();
    focusInput();
})();
