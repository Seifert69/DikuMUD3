/*
 * Command handling shared by the clients: command history, aliases, variables and triggers.
 *
 * Aliases, triggers and variables live on the server (the alias / trigger / variable commands)
 * and arrive as <data type='$alias'> etc. lists of "name value" items; the client applies them.
 *   alias   <name> cmd;cmd $1 $2      $0 = everything after the alias name, $1..$9 = its words
 *   trigger <name> /regexp/ commands  $0 = whole match, $1.. = capture groups; @var works in the regexp
 *   var     <name> value
 */
(function (root, factory) {
    const mod = factory();
    if (typeof module === "object" && module.exports) module.exports = mod;
    else (root.Diku = root.Diku || {}).engine = mod;
})(typeof globalThis !== "undefined" ? globalThis : this, function () {
    "use strict";

    const MAX_DEPTH = 5;
    const DIRNAMES = { n: "north", e: "east", s: "south", w: "west", u: "up", d: "down", ne: "northeast", nw: "northwest", se: "southeast", sw: "southwest" };
    const DIRECTIONS = new Set(Object.entries(DIRNAMES).flat());

    /* Command history. Movement is not worth remembering, nor is repeating the last command. */
    class History {
        constructor(max = 200, items = []) {
            this.max = max;
            this.items = items.slice(-max);
            this.pos = 0; // 0 = not browsing, 1 = newest
        }

        // Returns true if cmd was added
        add(cmd) {
            const s = cmd.trim();
            this.pos = 0;
            if (!s || DIRECTIONS.has(s.toLowerCase()) || this.items[this.items.length - 1] === s) return false;
            this.items.push(s);
            if (this.items.length > this.max) this.items.shift();
            return true;
        }

        // Older entry; returns the text to show
        up() {
            if (this.pos < this.items.length) this.pos++;
            return this.pos ? this.items[this.items.length - this.pos] : "";
        }

        down() {
            if (this.pos > 0) this.pos--;
            return this.pos ? this.items[this.items.length - this.pos] : "";
        }

        reset() {
            this.pos = 0;
        }
    }

    // Splits on ';' except an escaped '\;', which becomes a literal ';'.
    function splitCommands(line) {
        const out = [];
        let cur = "";
        for (let i = 0; i < line.length; i++) {
            const c = line[i];
            if (c === "\\" && line[i + 1] === ";") {
                cur += ";";
                i++;
            } else if (c === ";") {
                out.push(cur);
                cur = "";
            } else {
                cur += c;
            }
        }
        out.push(cur);
        return out.map(s => s.trim()).filter(Boolean);
    }

    // Replaces $0..$9 in template with args[0..9]; a missing argument becomes "".
    function substitute(template, args) {
        return template.replace(/\$(\d)/g, (m, d) => args[Number(d)] ?? "");
    }

    function firstWord(s) {
        const i = s.search(/\s/);
        return i < 0 ? s : s.slice(0, i);
    }

    function rest(s) {
        const i = s.search(/\s/);
        return i < 0 ? "" : s.slice(i + 1).trim();
    }

    function nameValue(items) {
        const out = new Map();
        for (const it of items) {
            const name = firstWord(it.trim());
            if (name) out.set(name.toLowerCase(), { name, value: rest(it.trim()) });
        }
        return out;
    }

    class Engine {
        constructor({ now = () => Date.now(), triggerRate = 5 } = {}) {
            this.aliases = new Map();
            this.vars = new Map();
            this.triggerItems = [];
            this.triggers = [];
            this.now = now;
            this.triggerRate = triggerRate; // max firings per second before triggers are paused
            this.fired = [];
        }

        setAliases(items) {
            this.aliases = nameValue(items);
        }

        setVariables(items) {
            this.vars = nameValue(items);
            return this.setTriggers(this.triggerItems); // triggers use @vars
        }

        // Items look like "name /regexp/ commands". Returns a list of error messages.
        setTriggers(items) {
            this.triggerItems = items.slice();
            this.triggers = [];
            const errors = [];
            for (const item of items) {
                const a = item.indexOf("/"), b = item.lastIndexOf("/");
                if (a < 0 || a === b) {
                    errors.push(`Trigger ${item}: put the pattern between slashes, like /pattern/ commands.`);
                    continue;
                }
                const pattern = this.applyVars(item.slice(a + 1, b).trim());
                const cmds = item.slice(b + 1).trim();
                if (!pattern) {
                    errors.push(`Trigger ${item}: the pattern between the slashes is empty.`);
                    continue;
                }
                if (!cmds) {
                    errors.push(`Trigger ${item}: there are no commands after the pattern.`);
                    continue;
                }
                try {
                    this.triggers.push({ name: firstWord(item.trim()), re: new RegExp(pattern, "gim"), cmds });
                } catch (e) {
                    errors.push(`Trigger ${item}: ${e.message}`);
                }
            }
            return errors;
        }

        applyVars(s) {
            // Longest names first so @foobar is not taken as @foo + "bar"
            const names = [...this.vars.values()].sort((x, y) => y.name.length - x.name.length);
            for (const v of names) s = s.split("@" + v.name).join(v.value);
            return s;
        }

        /*
         * Expands a typed line into the commands to send: splits on ';' and applies aliases.
         * Returns {commands, errors}.
         */
        expand(line, depth = 0) {
            const commands = [], errors = [];
            if (depth > MAX_DEPTH) {
                errors.push("Alias nesting is more than five levels deep; stopped.");
                return { commands, errors };
            }
            for (const cmd of splitCommands(line)) {
                const alias = this.aliases.get(firstWord(cmd).toLowerCase());
                if (!alias) {
                    commands.push(cmd);
                    continue;
                }
                const args = [rest(cmd), ...rest(cmd).split(/\s+/).filter(Boolean)];
                const sub = this.expand(substitute(alias.value, args), depth + 1);
                commands.push(...sub.commands);
                errors.push(...sub.errors);
                if (sub.errors.length) break;
            }
            return { commands, errors };
        }

        /*
         * Runs the triggers over a piece of plain output text. Returns {commands, errors}.
         * If triggers fire too fast (a loop with the server), they pause for a second.
         */
        checkTriggers(text) {
            const commands = [], errors = [];
            if (!this.triggers.length || !text) return { commands, errors };
            const t = this.now();
            this.fired = this.fired.filter(x => t - x < 1000);
            for (const trig of this.triggers) {
                trig.re.lastIndex = 0;
                for (const m of text.matchAll(trig.re)) {
                    if (this.fired.length >= this.triggerRate) {
                        errors.push("Triggers are firing too fast; paused for a second.");
                        return { commands, errors };
                    }
                    this.fired.push(t);
                    const sub = this.expand(substitute(trig.cmds, m));
                    commands.push(...sub.commands);
                    errors.push(...sub.errors);
                    if (m[0] === "") break; // an empty match would loop forever
                }
            }
            return { commands, errors };
        }
    }

    return { History, Engine, splitCommands, substitute, DIRNAMES };
});
