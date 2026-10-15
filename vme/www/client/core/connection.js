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
     * handlers: onOpen(), onFrame(text), onClose({code, reason, willRetry, retryIn}), onError(message)
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
        }

        get isOpen() {
            return !!this.ws && this.ws.readyState === 1;
        }

        connect() {
            clearTimeout(this.retryTimer);
            this.userClosed = false;
            let ws;
            try {
                ws = new this.WS(this.url);
            } catch (e) {
                this.h.onError && this.h.onError(String(e.message || e));
                this.scheduleRetry(0, "");
                return;
            }
            this.ws = ws;
            ws.onopen = () => {
                this.attempt = 0;
                ws.send(""); // mplex swallows the first message
                this.lastSend = Date.now();
                clearInterval(this.keepTimer);
                this.keepTimer = setInterval(() => this.keepAlive(), 30000);
                this.h.onOpen && this.h.onOpen();
            };
            ws.onmessage = ev => {
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
            this.h.onClose && this.h.onClose({ code, reason, willRetry, retryIn });
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

    return { Connection, buildUrl };
});
