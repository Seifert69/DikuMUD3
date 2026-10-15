/*
 * Turns server HTML into safe DOM nodes.
 *
 * Server output contains text typed by other players. It is HTML-escaped by the server, but
 * the client still only lets through a fixed set of tags and attributes: no scripts, no event
 * handlers, no javascript: links. <a cmd='help #'> links become command links (data-cmd), with
 * '#' replaced by the link text, as the server intends.
 */
(function (root) {
    "use strict";

    const ALLOWED = new Set(("div span p br b i u em strong small big sup sub pre code tt h1 h2 h3 h4 h5 h6 "
        + "ul ol li dl dt dd table thead tbody tfoot tr td th caption colgroup col a img hr blockquote center font").split(" "));
    // Dropped together with everything inside them
    const DROP = new Set(("script style iframe frame frameset object embed applet form input textarea select option "
        + "button link meta base svg math noscript template audio video canvas title head").split(" "));
    const ATTRS = {
        "*": ["class", "title", "align"],
        a: ["href", "cmd"],
        img: ["src", "alt", "width", "height"],
        td: ["colspan", "rowspan", "valign", "width"],
        th: ["colspan", "rowspan", "valign", "width"],
        table: ["border", "cellpadding", "cellspacing", "width"],
        col: ["span", "width"],
        ol: ["start", "type"],
        font: ["color"],
        p: ["style"], div: ["style"], span: ["style"],
    };
    const SAFE_URL = /^(https?:\/\/|\/|\.\.?\/|[\w.-]+(\/|$)|#)/i;
    const BAD_STYLE = /url\s*\(|expression|javascript|@import|behavior|position\s*:\s*fixed/i;

    function allowedAttr(tag, name) {
        return ATTRS["*"].includes(name) || (ATTRS[tag] || []).includes(name);
    }

    function cleanNode(src, doc) {
        if (src.nodeType === 3) return doc.createTextNode(src.data);
        if (src.nodeType !== 1) return null;
        const tag = src.localName;
        if (DROP.has(tag)) return null;
        let out;
        if (ALLOWED.has(tag)) {
            out = doc.createElement(tag);
            for (const a of src.attributes) {
                const name = a.name.toLowerCase();
                if (!allowedAttr(tag, name)) continue;
                const v = a.value;
                if (name === "class") {
                    const cls = v.split(/\s+/).filter(c => /^[\w-]+$/.test(c)).join(" ");
                    if (cls) out.setAttribute("class", cls);
                } else if (name === "href") {
                    if (/^https?:\/\//i.test(v.trim())) {
                        out.setAttribute("href", v.trim());
                        out.setAttribute("target", "_blank");
                        out.setAttribute("rel", "noopener noreferrer");
                    }
                } else if (name === "src") {
                    if (SAFE_URL.test(v.trim()) && !/^\s*\w+script:/i.test(v)) out.setAttribute("src", v.trim());
                } else if (name === "style") {
                    if (!BAD_STYLE.test(v)) out.setAttribute("style", v);
                } else if (name !== "cmd") {
                    out.setAttribute(name, v);
                }
            }
        } else {
            out = doc.createDocumentFragment(); // unknown element: keep its contents only
        }
        for (const c of src.childNodes) {
            const n = cleanNode(c, doc);
            if (n) out.appendChild(n);
        }
        if (tag === "a" && src.hasAttribute("cmd")) {
            out.dataset.cmd = src.getAttribute("cmd").replace("#", out.textContent.trim());
            out.setAttribute("href", "#");
            out.classList.add("cmdlink");
        }
        return out;
    }

    const parser = new DOMParser();

    // Returns a DocumentFragment of safe nodes for the given server HTML.
    function sanitize(html) {
        const src = parser.parseFromString("<!doctype html><body>" + html, "text/html").body;
        const frag = document.createDocumentFragment();
        for (const c of src.childNodes) {
            const n = cleanNode(c, document);
            if (n) frag.appendChild(n);
        }
        return frag;
    }

    (root.Diku = root.Diku || {}).sanitize = sanitize;
})(globalThis);
