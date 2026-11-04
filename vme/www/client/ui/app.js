/*
 * The visual web client: wires the shared core (protocol, engine, connection) to the page.
 *
 * Panels: output log, combat dock (Summary or Scroll view), input bar, vitals, room + compass, map, conversations,
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
    // Before v2 every page stored skin "auto"; Valhalla Classic is the default now
    function storedSettings() {
        const st = store.get("dk.settings", {});
        if (!st.v && st.skin === "auto") delete st.skin;
        st.v = 2;
        return st;
    }
    const settings = Object.assign({ v: 2, skin: "classic", size: "m", combatView: "scroll", fold: true, showPrompt: false, echo: true, stamps: true }, storedSettings());

    const prefersLight = window.matchMedia("(prefers-color-scheme: light)");

    function applySettings() {
        root.dataset.skin = settings.skin !== "auto" ? settings.skin : prefersLight.matches ? "vellum" : "hall";
        root.dataset.size = settings.size;
        root.dataset.stamps = settings.stamps ? "on" : "off";
        $("#skin").value = settings.skin;
        $$("#sizes [data-size]").forEach(b => b.setAttribute("aria-pressed", String(b.dataset.size === settings.size)));
        $("#opt-fold").checked = settings.fold;
        $("#combat").dataset.view = settings.combatView;
        const cbview = $("#opt-cbview"); // missing while a browser still has the previous page cached
        if (cbview) cbview.value = settings.combatView;
        $$("[data-cbview]").forEach(b => b.setAttribute("aria-pressed", String(b.dataset.cbview === settings.combatView)));
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
        lastLine: "",
        password: false,
        quitting: false,
        vit: null,
    };
    const protocol = new D.protocol.Protocol();
    const engine = new D.engine.Engine();
    const history = new D.engine.History(200, store.get("dk.history", []));

    /* ======================================================================
     * Output log
     * ====================================================================== */
    const out = { el: $("#output"), pill: $("#newlines"), stick: true, unseen: 0, gap: false, group: null, scrollQueued: false, mark: null };
    const MAX_ENTRIES = 3000;

    function atBottom() {
        return out.el.scrollHeight - out.el.scrollTop - out.el.clientHeight < 48;
    }
    // Scrolling the player did (wheel, touch, scrollbar, Page Up) as opposed to the browser's own.
    const userScrolled = () => (out.userScrollAt = performance.now());
    for (const ev of ["wheel", "touchmove", "pointerdown"]) out.el.addEventListener(ev, userScrolled, { passive: true });
    out.el.addEventListener("scroll", () => {
        // Only the player scrolls away from the end. The browser scrolls by itself too, e.g.
        // while new output waits for the next frame, or when an image above (the logo) loads
        // and the log grows: then keep following. 1 s covers a touch flick's glide.
        if (performance.now() - (out.userScrollAt || 0) > 1000) {
            if (out.stick && !atBottom()) scrollToEnd();
            return;
        }
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
            if (!out.stick) return;
            out.el.scrollTop = out.el.scrollHeight;
            out.unseen = 0;
            out.pill.hidden = true;
        });
    }

    // An image (the logo) that loads after the scroll to the end makes the log taller; follow it
    out.el.addEventListener("load", e => {
        if (e.target.tagName === "IMG" && out.stick) scrollToEnd();
    }, true);

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

    // The password dialog stays until the server ends the password step (closePasswordDialog).
    function closeScrim() {
        if (scrim.hidden || scrim.querySelector(".pwdlg")) return;
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
        const bar = el("div", "reader-bar"); // sticky: Close stays in view, above the text
        bar.append(x);
        r.append(bar, serverHtml(html));
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
     * Who's online, from the server's hidden <data type='$who'> packets: the full list when
     * you enter the game, then one change per player entering or leaving. Clicking a name
     * starts a tell.
     * ====================================================================== */
    const who = new Map(); // name -> tags

    function whoData(items) {
        for (const it of items) {
            if (it === "*") {
                who.clear();
            } else if (it[0] === "+") {
                const [name, ...tags] = it.slice(1).trim().split(/\s+/);
                if (name) who.set(name, tags);
            } else if (it[0] === "-") {
                who.delete(it.slice(1).trim());
            }
        }
        renderWho();
    }

    function renderWho() {
        const card = $("#whocard"); // missing while a browser still has the previous page cached
        if (!card) return;
        const list = $("#wholist");
        list.replaceChildren();
        for (const [name, tags] of [...who].sort((a, b) => a[0].localeCompare(b[0]))) {
            const b = el("button", null, name);
            b.type = "button";
            b.title = `Send ${name} a tell`;
            for (const t of tags) b.append(el("small", null, t));
            b.addEventListener("click", () => {
                input.value = `tell ${name.toLowerCase()} `;
                input.focus();
            });
            const li = el("li");
            li.append(b);
            list.append(li);
        }
        $("#whocount").textContent = String(who.size);
        card.hidden = !who.size;
    }

    /* ======================================================================
     * Combat dock: your vitals in the header, then one card per being (Summary)
     * or the fight's own scrolling log (Scroll). Alt+C switches between them.
     * ====================================================================== */
    const combat = { rows: new Map(), deadOrder: 0, hasData: false, active: false, round: 0, lines: [], hideTimer: null, idleTimer: null };
    const dock = $("#combat");

    // Built once, so the Scroll log keeps its lines and scroll position between redraws.
    const cb = { tag: el("span", "cb-tag"), round: el("span", "cb-round"), took: el("span", "cb-took"), flee: el("button", "btn warn", "Flee"), hide: el("button", "iconbtn cb-hide", "✕"), body: el("div", "cb-body"), log: el("div", "cb-log"), stick: true, userAt: 0 };
    {
        const head = el("div", "cb-head");
        const vit = el("div", "cb-vitals");
        vit.append(vitalRow("hp", "HP"), vitalRow("ep", "End"), vitalRow("mp", "Mana"));
        const sw = el("div", "cb-view");
        sw.setAttribute("role", "group");
        sw.setAttribute("aria-label", "Combat view");
        for (const [v, label] of [["summary", "Summary"], ["scroll", "Scroll"]]) {
            const b = el("button", null, label);
            b.type = "button";
            b.dataset.cbview = v;
            b.title = "Alt+C switches view";
            sw.append(b);
        }
        cb.flee.type = "button";
        cb.flee.dataset.cmd = "flee";
        cb.hide.type = "button";
        cb.hide.title = "Hide until this fight is over";
        cb.hide.setAttribute("aria-label", cb.hide.title);
        head.append(cb.tag, cb.round, cb.took, vit, sw, cb.flee, cb.hide);
        cb.log.setAttribute("role", "log");
        cb.log.setAttribute("aria-label", "Combat log");
        dock.append(head, cb.body, cb.log);
    }

    function setCombatView(view) {
        settings.combatView = view;
        applySettings();
        if (!dock.hidden) renderDock();
        else sys(`Combat panel: ${view} view.`);
    }
    cb.hide.addEventListener("click", () => {
        combat.dismissed = true; // until the fight is over, see endCombat
        dock.hidden = true;
        focusInput();
    });
    dock.addEventListener("click", e => {
        const b = e.target.closest("[data-cbview]");
        if (!b) return;
        setCombatView(b.dataset.cbview);
        focusInput();
    });

    // Only the player scrolls the combat log away from its end, as in the main log.
    for (const t of ["wheel", "touchmove", "pointerdown"]) cb.log.addEventListener(t, () => (cb.userAt = Date.now()), { passive: true });
    cb.log.addEventListener("scroll", () => {
        if (Date.now() - cb.userAt < 500) cb.stick = cb.log.scrollHeight - cb.log.scrollTop - cb.log.clientHeight < 30;
    });

    // Life in 5 boxes (the server sends no more than that), in the prompt's own words
    const LIFE = ["dead", "gushing", "bleeding", "scraped", "bruised", "healthy"];
    const lifeOf = c => (c.life ?? (c.hp > 0 ? Math.ceil(c.hp / 20) : 0)); // hp: a server from before "life"

    // A blow of step 5-8 (of the 0-8 that choose the combat message's words) is a big one
    const BIG = 5;

    // The fight is over: show that, then hide the dock and forget the fight after a while.
    function endCombat(delay) {
        combat.active = false;
        clearTimeout(combat.staleTimer);
        closeCombatGroup();
        clearTimeout(combat.hideTimer);
        if (combat.dismissed) delay = 0; // put away by the player: just forget the fight
        else scheduleDock();
        combat.dismissed = false; // the next fight shows again
        combat.hideTimer = setTimeout(() => {
            cancelAnimationFrame(combat.drawFrame); // a redraw queued by endCombat would show it again
            combat.drawQueued = false;
            dock.hidden = true;
            combat.rows.clear();
            combat.lines = [];
            cb.log.replaceChildren();
            cb.stick = true;
        }, delay);
    }

    function combatData(j) {
        combat.hasData = true;
        if (j.event === "death") {
            const r = combat.rows.get(j.id);
            if (r) {
                r.dead = true;
                r.life = 0;
                r.deadAt = ++combat.deadOrder;
            }
            if (combat.active) scheduleDock();
            else endCombat(6000); // the death can follow the end of the fight
            return;
        }
        // Nobody hostile left in the list (e.g. moved to another room by force): the fight is over
        if (j.end || !Array.isArray(j.list) || !j.list.some(c => c.foe)) {
            endCombat(j.end ? 6000 : 3000);
            return;
        }
        clearTimeout(combat.hideTimer);
        combat.active = true;
        combat.round = j.round || combat.round + 1;
        const seen = new Set();
        combat.took = [];
        for (const c of j.list) {
            if (c.me) {
                combat.took = [...(c.byYou || []), ...(c.byOthers || [])];
                continue;
            }
            seen.add(c.id);
            const r = combat.rows.get(c.id) || { id: c.id, last: "", dead: false };
            r.prevLife = r.life ?? lifeOf(c);
            Object.assign(r, { name: c.name, life: lifeOf(c), target: c.target || "", foe: !!c.foe, byYou: c.byYou || [], byOthers: c.byOthers || [] });
            combat.rows.set(c.id, r);
        }
        combat.blowsAt = Date.now();
        // A packet comes every round (3 s); without one for 4 rounds the fight is gone
        clearTimeout(combat.staleTimer);
        combat.staleTimer = setTimeout(() => endCombat(2000), 12000);
        for (const [id, r] of combat.rows) if (!seen.has(id) && !r.dead) combat.rows.delete(id); // fled or left
        scheduleDock();
    }

    // Every combat line goes into the Scroll log, and onto the card of the being it is about (by name).
    function combatLine(c) {
        cb.log.append(el("div", c.cls, c.text));
        while (cb.log.childElementCount > 300) cb.log.firstElementChild.remove();
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
            }
            scheduleDock();
            return;
        }
        // No $combat data from this server: Summary shows the latest lines, and the lines decide when the fight is over.
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
        combat.drawFrame = requestAnimationFrame(() => {
            combat.drawQueued = false;
            renderDock();
        });
    }

    function renderDock() {
        dock.hidden = !!combat.dismissed;
        if (combat.dismissed) return;
        dock.classList.toggle("over", !combat.active);
        cb.tag.replaceChildren(el("i"), combat.active ? "Fighting" : "Combat over");
        cb.round.textContent = combat.hasData && combat.round ? `Round ${combat.round}` : "";
        cb.took.replaceChildren(...(combat.active && combat.took?.length ? blows("✖", combat.took, "took-hits", "Hits you took this round") : []));
        cb.flee.hidden = !combat.active;
        const scroll = settings.combatView === "scroll";
        cb.log.hidden = !scroll;
        if (combat.hasData) {
            const rows = [...combat.rows.values()].sort((a, b) => rank(a) - rank(b) || (a.deadAt || 0) - (b.deadAt || 0));
            const box = el("div", scroll ? "cb-strip" : "cb-grid");
            for (const r of rows) box.append(scroll ? chip(r) : card(r));
            cb.body.replaceChildren(box);
        } else if (scroll) {
            cb.body.replaceChildren();
        } else {
            const ls = el("div", "cb-lines");
            for (const c of combat.lines) ls.append(el("div", c.cls, c.text));
            cb.body.replaceChildren(ls);
        }
        if (scroll && cb.stick) cb.log.scrollTop = cb.log.scrollHeight;
    }

    // Attacking you first, then other foes, then allies, then the dead.
    function rank(r) {
        if (r.dead) return 4;
        if (r.foe && r.target === "you") return 0;
        if (r.foe) return 1;
        return 2;
    }

    function rowClass(r) {
        return (r.foe ? "" : " ally") + (r.dead ? " dead" : "") + (!r.dead && r.foe && r.target === "you" ? " at-me" : "");
    }

    // Five boxes; one just lost blinks
    function lifeBar(r) {
        const bar = el("div", "life");
        bar.title = LIFE[r.life];
        for (let i = 1; i <= 5; i++) {
            const box = el("i", i <= r.life ? "on" : i <= (r.prevLife ?? 0) && fresh() ? "lost" : null);
            bar.append(box);
        }
        return bar;
    }

    const fresh = () => Date.now() - (combat.blowsAt || 0) < 900; // the round that just came in

    // "⚔×3 💥": hits this round, and a 💥 for each big one
    function blows(glyph, steps, cls, title) {
        if (!steps.length) return [];
        const b = el("span", "blows " + cls + (fresh() ? " pop" : ""));
        b.title = title;
        b.append(`${glyph}×${steps.length}`);
        const big = steps.filter(s => s >= BIG).length;
        if (big) b.append(el("span", "big", " " + "💥".repeat(Math.min(big, 3))));
        return [b];
    }

    function hitsOn(r) {
        const h = el("div", "cb-hits");
        h.append(...blows("⚔", r.byYou || [], "by-you", "Your hits this round"), ...blows("⚔", r.byOthers || [], "by-others", "Others' hits this round"));
        return h;
    }

    function card(r) {
        const c = el("div", "cb-row" + rowClass(r));
        const name = el("div", "cb-name");
        name.append(el("b", null, cap(r.name)));
        name.append(el("span", "cb-target", r.dead ? "dead" : r.target ? "→ " + r.target : ""));
        const hp = el("div", "cb-hp");
        hp.append(lifeBar(r), el("span", null, LIFE[r.life]), hitsOn(r));
        const last = el("div", "cb-last" + (r.flash && Date.now() - r.flash < 1500 ? " flash" : ""));
        if (r.last) last.append(el("span", r.lastCls, r.last));
        c.append(name, hp, last);
        return c;
    }

    // Scroll view: one small entry per being above the log.
    function chip(r) {
        const c = el("div", "cb-chip" + rowClass(r));
        c.title = `${cap(r.name)}: ${LIFE[r.life]}${!r.dead && r.target ? ", fighting " + r.target : ""}`;
        c.append(el("b", null, cap(r.name)), lifeBar(r), hitsOn(r));
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
            base = { id: "room", kind: "room", name: "Room", sub: "say, ask, whisper and emote", cmd: "say" };
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
                    if (relogin.active && relogin.inMenu) stopRelogin();
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
            if (relogin.active || relogin.pending) loginText(D.html.textOf(ev.tree));
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
            else if (ev.dtype === "$who") whoData(ev.items);
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

    /*
     * Passwords go in a dialog of their own. Turning the command field into a password field made
     * Firefox treat it as a login field for good, with a password manager popup on every click.
     * The dialog's form names the character, so a saved password belongs to that character.
     * It covers the log, so it repeats what the server said since the last line was sent:
     * a refused password is followed by the rules and a new prompt.
     */
    function serverLinesSinceSend(max = 6) {
        let node = out.mark?.isConnected ? out.mark.nextElementSibling : out.el.lastElementChild;
        const lines = [];
        for (; node; node = node.nextElementSibling) {
            if (node.classList.contains("echo")) continue;
            lines.push(...node.innerText.split("\n").map(l => l.trim()).filter(Boolean));
        }
        return lines.slice(-max);
    }

    function showPasswordDialog() {
        const lines = serverLinesSinceSend();
        const question = lines.pop() || "";
        const form = el("form", "panel connect pwdlg");
        form.append(el("h1", null, "Password"));
        if (lines.length) {
            const said = el("div", "said");
            said.setAttribute("role", "alert");
            lines.forEach(l => said.append(el("p", null, l)));
            form.append(said);
        }
        form.append(el("p", null, question || "Enter your password."));
        const user = el("input", "sr");
        Object.assign(user, { type: "text", name: "username", autocomplete: "username", readOnly: true, tabIndex: -1, value: S.lastLine });
        const label = el("label", "wide", "Password");
        const pw = el("input");
        Object.assign(pw, { type: "password", name: "password", id: "pw", autocomplete: /new|retype/i.test(question) ? "new-password" : "current-password" });
        label.append(pw);
        const fields = el("div", "fields");
        fields.append(label);
        const go = el("button", "btn primary", "Enter");
        go.type = "submit";
        const row = el("div", "row");
        row.append(el("span"), go);
        form.append(user, fields, row);
        form.addEventListener("submit", e => {
            e.preventDefault();
            if (EXISTING_PWD.test(question)) relogin.pending = { name: S.lastLine, pw: pw.value };
            if (S.conn && S.conn.isOpen) S.conn.send(pw.value);
            pw.value = "";
            out.mark = out.el.lastElementChild;
        });
        openScrim(form);
        pw.focus();
    }

    function closePasswordDialog() {
        if (!scrim.querySelector(".pwdlg")) return;
        scrim.hidden = true;
        scrim.replaceChildren();
    }

    function setPassword(ev) {
        S.password = ev.on;
        if (ev.on && reloginPassword()) return;
        if (ev.on) showPasswordDialog();
        else closePasswordDialog();
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
     * Logging back in after a reconnect
     *
     * The name and password of the last successful login stay in this page's memory (never
     * in storage), so a reconnect can answer the login questions itself. It stops at anything
     * unexpected, and a refused password is forgotten so it is never tried twice.
     * ====================================================================== */
    const EXISTING_PWD = /please enter your password/i;
    const relogin = { creds: null, pending: null, active: false, sentPw: false, timer: null };

    function startRelogin() {
        if (!relogin.creds) return;
        Object.assign(relogin, { active: true, sentPw: false, inMenu: false });
        clearTimeout(relogin.timer);
        relogin.timer = setTimeout(stopRelogin, 30000);
    }

    function stopRelogin() {
        relogin.active = false;
        clearTimeout(relogin.timer);
    }

    // The text of each server message while a login is under way.
    function loginText(text) {
        if (/Welcome back .*, you last visited/.test(text)) {
            if (relogin.pending) relogin.creds = relogin.pending;
            relogin.pending = null;
            if (relogin.active) relogin.inMenu = true; // a relogin goes on into the game
            else stopRelogin();
        } else if (/Wrong password/.test(text)) {
            relogin.creds = relogin.pending = null;
            stopRelogin();
        } else if (relogin.active && /Throw the other copy out\?/.test(text)) {
            send("Yes"); // that copy is this page's lost connection (comes after "Welcome back"); some servers want the whole word
        } else if (relogin.active && !relogin.inMenu) {
            if (/By what name do they call you\?/.test(text)) send(relogin.creds.name);
            else if (/Press \[enter\] and wait for the password prompt/.test(text)) send("");
            else if (/Did I get that right/.test(text)) stopRelogin(); // the character is gone: leave it to the player
        }
        // The login menu, maybe in the same message: "1) Enter Valhalla" (gods), "3) Enter the game." (players)
        if (relogin.active && relogin.inMenu && /Make your choice/.test(text)) {
            const m = /(\w)\)\s*Enter (the game|Valhalla)/i.exec(serverLinesSinceSend(40).join("\n"));
            if (m) send(m[1]);
            stopRelogin();
        }
    }

    // Answers the password prompt of a relogin; false when the player should be asked.
    function reloginPassword() {
        if (!relogin.active || relogin.sentPw || !EXISTING_PWD.test(serverLinesSinceSend().pop() || "")) return false;
        relogin.sentPw = true;
        S.conn.send(relogin.creds.pw);
        out.mark = out.el.lastElementChild;
        return true;
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
        out.mark = out.el.lastElementChild;
        return S.conn.send(cmd);
    }

    // A typed line: history, aliases and ';' splitting. The password is sent exactly as typed.
    function runLine(line) {
        if (S.password) {
            $("#pw")?.focus(); // the password goes in its dialog, never through here
            return;
        }
        if (line.trim()) S.lastLine = line.trim(); // before a password prompt, that's the character's name
        if (/^\s*quit\s*$/i.test(line)) S.quitting = true; // a closed connection then is no surprise
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
            userScrolled();
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
        if (e.altKey && e.code === "KeyC" && !e.ctrlKey && !e.metaKey) {
            e.preventDefault();
            setCombatView(settings.combatView === "scroll" ? "summary" : "scroll");
            return;
        }
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
    $("#opt-cbview")?.addEventListener("change", e => setCombatView(e.target.value));
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

    // What a websocket close code means to a player.
    function closeText(code, reason) {
        const what = {
            1000: "The server closed the connection",
            1001: "The server or the browser went away",
            1006: "The connection dropped without a goodbye (network trouble, the device sleeping, or the server)",
            1011: "The server hit an error",
            1015: "The secure (TLS) connection failed",
        }[code] || "The connection closed";
        return `${what}. Code ${code || "?"}${reason ? ": " + reason : ""}, at ${hhmm()}.`;
    }

    function showLostDialog(text, reconnect) {
        const d = el("div", "panel connect lost");
        d.setAttribute("role", "alertdialog");
        d.setAttribute("aria-label", "Connection lost");
        d.append(el("h1", null, "Connection lost"), el("p", null, text));
        if (relogin.creds) d.append(el("p", null, `Reconnecting logs you back in as ${cap(relogin.creds.name)}.`));
        const row = el("div", "row");
        const change = el("button", "btn", "Change server");
        const go = el("button", "btn primary", "Reconnect");
        change.type = go.type = "button";
        change.addEventListener("click", showConnect);
        go.addEventListener("click", () => {
            closeScrim();
            reconnect();
        });
        row.append(change, go);
        d.append(row);
        openScrim(d);
        go.focus();
    }

    function connect(server) {
        if (S.conn) S.conn.close();
        const url = D.connection.buildUrl(server);
        if (S.server && D.connection.buildUrl(S.server) !== url) relogin.creds = null; // another MUD
        S.server = server;
        store.set("dk.server", server);
        setConnState("connecting", server.host);
        banner("");
        sys(`Connecting to ${url}…`);
        // No automatic retries: an unattended page would loop through the name prompt's idle kick.
        const conn = new D.connection.Connection(url, {
            onOpen() {
                setConnState("open", server.host);
                banner("");
                sys(`Connected to ${server.host}.`);
                S.quitting = false;
                startRelogin();
                focusInput();
            },
            onFrame,
            onClose({ code, reason }) {
                if (S.conn !== conn) return;
                setConnState("closed", "Disconnected");
                setPassword({ on: false });
                stopRelogin();
                whoData(["*"]); // known again when the game sends the list
                combat.active = false;
                const again = () => {
                    banner("");
                    setConnState("connecting", server.host);
                    sys("Reconnecting…");
                    conn.connect();
                };
                const text = closeText(code, reason);
                sys(text, !S.quitting && code !== 1000);
                banner(S.quitting ? "You left the game." : "Disconnected.", [["Reconnect", again], ["Change server", showConnect]]);
                if (S.quitting) relogin.creds = null;
                else if (!conn.userClosed) showLostDialog(text, again);
            },
        }, { autoReconnect: false });
        S.conn = conn;
        conn.connect();
    }

    /* ======================================================================
     * Start
     * ====================================================================== */
    // ?debug=1 lets the developer console feed server frames: DikuDebug.feed("<div class='hit_me'>…</div>"),
    // and drop the connection as the network would: DikuDebug.drop()
    if (new URLSearchParams(location.search).get("debug") === "1") window.DikuDebug = { feed: onFrame, out, drop: () => S.conn?.ws?.close() };

    prefersLight.addEventListener("change", applySettings);
    applySettings();
    renderThreads();
    updateTitle();
    const start = serverFromUrl() || store.get("dk.server", null);
    if (start) connect(start);
    else showConnect();
    focusInput();
})();
