/*
 * Minimal, DOM-free HTML reader for the server's output.
 *
 * The game server sends HTML fragments that are often unbalanced (a <div> left open, a stray
 * </div>). This reader turns a fragment into a small tree the same forgiving way a browser
 * would, so the protocol layer can find prompts, room titles and comm lines without a DOM.
 * It is shared by the visual and the accessible client and runs under Node for the tests.
 */
(function (root, factory) {
    const mod = factory();
    if (typeof module === "object" && module.exports) module.exports = mod;
    else (root.Diku = root.Diku || {}).html = mod;
})(typeof globalThis !== "undefined" ? globalThis : this, function () {
    "use strict";

    const ENTITIES = { amp: "&", lt: "<", gt: ">", quot: "\"", apos: "'", nbsp: " " };

    function decodeEntities(s) {
        return s.replace(/&(#x[0-9a-f]+|#[0-9]+|[a-z]+);/gi, (m, e) => {
            if (e[0] === "#") {
                const n = e[1] === "x" || e[1] === "X" ? parseInt(e.slice(2), 16) : parseInt(e.slice(1), 10);
                return n > 0 && n < 0x110000 ? String.fromCodePoint(n) : m;
            }
            const v = ENTITIES[e.toLowerCase()];
            return v === undefined ? m : v;
        });
    }

    function escapeHtml(s) {
        return String(s).replace(/[&<>"']/g, c => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", "\"": "&quot;", "'": "&#39;" })[c]);
    }

    const ATTR = /([^\s"'<>/=]+)(?:\s*=\s*(?:'([^']*)'|"([^"]*)"|([^\s'">]+)))?/g;

    // Attributes of a start tag body, e.g. " class='room_title' zone='midgaard'". Keys are lower-cased.
    function parseAttrs(s) {
        const out = {};
        let m;
        ATTR.lastIndex = 0;
        while ((m = ATTR.exec(s || ""))) {
            const k = m[1].toLowerCase();
            if (!(k in out)) out[k] = decodeEntities(m[2] ?? m[3] ?? m[4] ?? "");
        }
        return out;
    }

    const VOID = new Set(["br", "img", "hr", "input", "meta", "link", "wbr", "col", "area", "base", "source"]);
    const BLOCK = new Set(["p", "h1", "h2", "h3", "h4", "h5", "h6", "li", "tr", "pre", "ul", "ol", "table"]);
    const TOKEN = /<!--[\s\S]*?(?:-->|$)|<\/([a-zA-Z][a-zA-Z0-9-]*)\s*>|<([a-zA-Z][a-zA-Z0-9-]*)((?:[^>'"]|'[^']*'|"[^"]*")*)>|<|[^<]+/g;

    /*
     * Parses an HTML fragment into {tag, attrs, children} element nodes and {text} text nodes.
     * An end tag closes the nearest open element with that name (and everything inside it);
     * an end tag with no open match is ignored; anything left open is closed at the end.
     */
    function parse(html) {
        const root = { tag: "#root", attrs: {}, children: [] };
        const stack = [root];
        let m;
        TOKEN.lastIndex = 0;
        while ((m = TOKEN.exec(html))) {
            const top = stack[stack.length - 1];
            if (m[0].startsWith("<!--")) continue;
            if (m[1]) {
                const name = m[1].toLowerCase();
                for (let i = stack.length - 1; i > 0; i--) {
                    if (stack[i].tag === name) {
                        stack.length = i;
                        break;
                    }
                }
                continue;
            }
            if (m[2]) {
                const name = m[2].toLowerCase();
                let rest = m[3] || "";
                const selfClosing = /\/\s*$/.test(rest);
                if (selfClosing) rest = rest.replace(/\/\s*$/, "");
                const node = { tag: name, attrs: parseAttrs(rest), children: [] };
                top.children.push(node);
                if (!VOID.has(name) && !selfClosing) stack.push(node);
                continue;
            }
            top.children.push({ text: decodeEntities(m[0]) });
        }
        return root;
    }

    function classes(node) {
        return node && node.attrs && node.attrs.class ? node.attrs.class.split(/\s+/).filter(Boolean) : [];
    }

    // Plain text of a node: <br> and block elements become line breaks, runs of blank lines collapse.
    function textOf(node) {
        const parts = [];
        (function walk(n) {
            if (n.text !== undefined) {
                parts.push(n.text);
                return;
            }
            if (n.tag === "br") {
                parts.push("\n");
                return;
            }
            for (const c of n.children) walk(c);
            if (BLOCK.has(n.tag)) parts.push("\n");
        })(node);
        return parts.join("").replace(/ /g, " ").replace(/\n{3,}/g, "\n\n");
    }

    function htmlToText(html) {
        return textOf(parse(html));
    }

    return { decodeEntities, escapeHtml, parseAttrs, parse, classes, textOf, htmlToText };
});
