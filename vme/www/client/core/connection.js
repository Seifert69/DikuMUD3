/*
 * The websocket connection to an mplex, with reconnect and keep-alive.
 *
 * mplex expects one (ignored) message right after connecting, then one command per message.
 * Commands are cut at the first newline and at 512 bytes by mplex, so a command never
 * contains a newline here.
 */
(function (root, factory) {
    const mod = factory();
    if (typeof module === "object" && module.exports) module.exports = mod;
    else (root.Diku = root.Diku || {}).connection = mod;
})(typeof globalThis !== "undefined" ? globalThis : this, function () {
    "use strict";

    const RETRY_SECONDS = [2, 5, 10, 20, 30, 60];
    const KEEPALIVE_SECONDS = 250; // mplex/vme drop idle links; the old client used the same

    function buildUrl({ protocol = "wss", host, port, endpoint = "" }) {
        const ep = endpoint && !endpoint.startsWith("/") ? "/" + endpoint : endpoint;
        return `${protocol}://${host}${port ? ":" + port : ""}${ep}`;
    }

    /*
     * handlers: onOpen(), onFrame(text), onError(message),
     * onClose({code, reason, willRetry, retryIn, opened, upFor, quietFor}): opened is false when
     * the connection never got through; upFor is how long it was open and quietFor how long
     * since the server last sent anything, in ms.
     */
    class Connection {
        constructor(url, handlers = {}, { WebSocketImpl = globalThis.WebSocket, autoReconnect = true } = {}) {
            this.url = url;
            this.h = handlers;
            this.WS = WebSocketImpl;
            this.autoReconnect = autoReconnect;
            this.ws = null;
            this.attempt = 0;
            this.userClosed = false;
            this.lastSend = Date.now();
            this.retryTimer = null;
            this.keepTimer = null;
            this.openedAt = 0;
            this.lastFrameAt = 0;
        }

        get isOpen() {
            return !!this.ws && this.ws.readyState === 1;
        }

        connect() {
            clearTimeout(this.retryTimer);
            this.userClosed = false;
            this.openedAt = 0;
            let ws;
            try {
                ws = new this.WS(this.url);
            } catch (e) {
                // The browser refused before trying (a bad address, ws:// from an https page):
                // its message is the reason, and code 0 says no connection was attempted
                const why = String((e && e.message) || e);
                this.h.onError && this.h.onError(why);
                this.scheduleRetry(0, why);
                return;
            }
            this.ws = ws;
            ws.onopen = () => {
                this.attempt = 0;
                this.openedAt = this.lastFrameAt = Date.now();
                ws.send(""); // mplex swallows the first message
                this.lastSend = Date.now();
                clearInterval(this.keepTimer);
                this.keepTimer = setInterval(() => this.keepAlive(), 30000);
                this.h.onOpen && this.h.onOpen();
            };
            ws.onmessage = ev => {
                this.lastFrameAt = Date.now();
                if (typeof ev.data === "string") this.h.onFrame && this.h.onFrame(ev.data);
            };
            ws.onerror = () => {
                this.h.onError && this.h.onError("The connection failed.");
            };
            ws.onclose = ev => {
                if (this.ws !== ws) return;
                clearInterval(this.keepTimer);
                this.ws = null;
                this.scheduleRetry(ev.code, ev.reason);
            };
        }

        scheduleRetry(code, reason) {
            const willRetry = this.autoReconnect && !this.userClosed && this.attempt < RETRY_SECONDS.length;
            const retryIn = willRetry ? RETRY_SECONDS[this.attempt] : 0;
            const now = Date.now();
            const opened = this.openedAt > 0;
            const upFor = opened ? now - this.openedAt : 0;
            const quietFor = opened ? now - this.lastFrameAt : 0;
            this.h.onClose && this.h.onClose({ code, reason, willRetry, retryIn, opened, upFor, quietFor });
            if (willRetry) {
                this.attempt++;
                this.retryTimer = setTimeout(() => this.connect(), retryIn * 1000);
            }
        }

        // Sends one command line. Returns false when not connected.
        send(cmd) {
            if (!this.isOpen) return false;
            this.ws.send(String(cmd).replace(/[\r\n]+/g, " "));
            this.lastSend = Date.now();
            return true;
        }

        keepAlive() {
            if (this.isOpen && Date.now() - this.lastSend >= KEEPALIVE_SECONDS * 1000) this.send("");
        }

        close() {
            this.userClosed = true;
            clearTimeout(this.retryTimer);
            clearInterval(this.keepTimer);
            if (this.ws) this.ws.close();
        }
    }

    // "40 s", "15 min", "2 h 5 min"; a clock set back gives 0, not a negative time
    function span(ms) {
        ms = Math.max(0, ms);
        const s = Math.round(ms / 1000);
        if (s < 60) return `${s} s`;
        const m = Math.round(ms / 60000); // from ms, not s: rounding twice made 89.5 s "2 min"
        if (m < 60) return `${m} min`;
        return `${Math.floor(m / 60)} h${m % 60 ? ` ${m % 60} min` : ""}`;
    }

    const CLOSE_WHAT = {
        1000: "The server closed the connection",
        1001: "The server or the browser went away",
        1006: "The connection dropped without a goodbye",
        1011: "The server hit an error",
        1012: "The server is restarting",
        1015: "The secure (TLS) connection failed",
    };

    /*
     * The clues the page has for a close, from the times it noted (ms, 0 for never): when it
     * was last hidden and shown, whether it is hidden now, when the device last went offline
     * and whether it is online now. Only what happened while this connection was open counts:
     * the page hidden for at most the time it was open; the device offline since the server
     * last sent anything. A connection that never opened has only "offline now".
     */
    function closeClues({ opened, upFor, quietFor, now, hiddenNow, hiddenAt, shownAt, offlineAt, onLine }) {
        if (!opened) return { hiddenFor: 0, offline: !onLine };
        const openedAt = now - upFor;
        let hiddenFor = 0;
        if (hiddenNow && hiddenAt) hiddenFor = now - Math.max(hiddenAt, openedAt);
        // Shown again only just now: a phone waking up delivers the close late
        else if (hiddenAt && shownAt >= hiddenAt && now - shownAt < 10000) hiddenFor = shownAt - Math.max(hiddenAt, openedAt);
        return { hiddenFor: Math.max(0, hiddenFor), offline: !onLine || (offlineAt > 0 && offlineAt >= now - quietFor) };
    }

    /*
     * What a close means to a player, with what we know about it. The browser gives no reason
     * for 1006 (on purpose), so the clues are ours: how long the link was up, how long the
     * server had been quiet, whether the page was in the background or the device offline.
     * info: onClose()'s, plus at (a clock time "11:27"), where ("host:port"), and
     * closeClues()'s hiddenFor and offline.
     */
    function describeClose({ code, reason, opened, upFor, quietFor, at, where, hiddenFor = 0, offline = false }) {
        // the server's reason, unless it only repeats what the code says
        const said = reason && reason !== CLOSE_WHAT[code] ? ": " + reason : "";
        const c = `code ${code || "?"}${said}`;
        const restarting = code === 1012;
        const out = [];
        if (!opened) {
            // 1006 is what every failed connect gives; the others say something
            const what = code !== 1006 && CLOSE_WHAT[code] ? `: ${CLOSE_WHAT[code].toLowerCase()}` : "";
            out.push(`Couldn't connect to ${where} at ${at} (${c})${what}.`);
        } else {
            out.push(`${CLOSE_WHAT[code] || "The connection closed"} at ${at} (${c}), after ${span(upFor)} connected.`);
            if (code !== 1000 && !restarting) out.push(`The server last sent something ${span(quietFor)} before.`);
        }
        // A restart is the server's own doing (mplex says so when systemd stops it): no guesses
        if (!restarting) {
            if (offline) out.push("Your device said it was offline.");
            if (hiddenFor >= 60000) out.push(`The page had been in the background for ${span(hiddenFor)}: a sleeping device or a browser saving power drops connections.`);
        }
        return out.join(" ");
    }

    // Where a server is, as a player reads it: "valhalla.com:4280", "[::1]:4280"
    function hostPort(server) {
        try {
            return new URL(buildUrl({ ...server, protocol: "wss" })).host;
        } catch (e) {
            return server.host;
        }
    }

    return { Connection, buildUrl, closeClues, describeClose, hostPort, span };
});
