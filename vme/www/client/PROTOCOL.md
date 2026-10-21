# Web client protocol

What the server (vme, through a websocket mplex) sends to the browser clients, and
what `core/protocol.js` makes of it. `core/` has no browser dependencies so that a
new screen reader client can share it; the current one (`client-a.html`) still
uses the older `wsclient.js`.

## Transport

- One websocket per player, to an mplex started with `-w` (`wss://valhalla.com:4280/echo`).
- Client → server: one command per message. The first message after connecting is
  ignored by mplex (the client sends an empty one). mplex cuts a command at the first
  newline and at 512 bytes. An empty message is a plain Enter.
- Server → client: each message is an arbitrary chunk of HTML, usually one `send_to_char`.
  Chunks over ~32KB are split, so containers below can span several messages.
- mplex also sends `<mud-init/>` and `<go-ahead/>` (prompt done); the client ignores both.

## Containers

| container | meaning |
|---|---|
| `<paged>…</paged>` | a page of text (help, who, score, lists). Shown in the log; long pages are folded with a "Read in full" button. |
| `<data type='$alias'><li>name text</li>…</data>` | the player's aliases. Also `$trigger` (`<li>name /regexp/ commands</li>`) and `$var` (`<li>name value</li>`). Applied by `core/engine.js`. |
| `<data type='$json'>{…}</data>` | wstat/jstat unit data. Not used by the client yet. |
| `<data type='$combat'>{…}</data>` | the fight the player is in, once per combat round; see below. Websocket players only. |
| `<data type='$who'><li>…</li></data>` | who's online, as changes; see below. Sent to every player; telnet drops `<data>`. |
| `<script>PasswordOn()</script>` | the next input is a password: hide it, don't echo it, keep it out of history. |
| `<script>PasswordOff('name', 'mud')</script>` | password done. `name` is the player once logged in (`''` before). |

Any other script is ignored; the client never runs server code.

### `$combat`

```
{"round":17,"list":[
  {"id":"9f3a0c1b22e4","name":"the fallow deer","hp":62,"target":"you","me":false,"foe":true},
  {"id":"51c0de4a9b17","name":"Moira","hp":88,"target":"the fallow deer","me":false,"foe":false},
  {"id":"0b7e2d55aa10","name":"you","hp":80,"target":"the fallow deer","me":true,"foe":false}]}
{"round":18,"list":[],"end":true}
{"event":"death","id":"9f3a0c1b22e4"}
```

- `list`: everyone in the player's fight in the same room: their opponents, anyone
  attacking them, their allies (fighting the same opponents, or fighting group members)
  and whatever the allies fight. Foes first, then allies, then the player (`me`); at most 12.
- `id`: opaque, stable for the unit while the server runs. `hp`: percent of max.
  `target`: who that being is hitting, as the player sees the name, or `"you"`.
- `end` when the player's fight is over; `event: death` when a listed being dies.
- Not sent when the fight ends within its first round (a one-hit kill).

### `$who`

```
<data type='$who'><li>*</li><li>+Brynja</li><li>+Papi Admin Builder</li></data>   to a player entering
<data type='$who'><li>+Ylva</li></data>                                            someone entered
<data type='$who'><li>-Ylva</li></data>                                            someone left
```

- `*`: the full list follows. `+Name tags…`: Name is in the game (tags: `Admin`,
  `Creator`, `Immortal`, `Builder`, `Friend`, `Guide`). `-Name`: Name left the game.
- Only players the receiver can see are listed or announced. Leaving goes to everyone.
- Server side: `who_enter@basis` from `informer` on every login, and `who_leave@basis` on
  `void@basis`, which hears `CMD_AUTO_LEAVE`. The server sends that from
  `pc_data::gstate_tomenu()`, which every way out of the game passes through.
- A message of nothing but `<data>` doesn't make the server send a new prompt.

## Classes in ordinary output

Server output uses `<div class='x'>` as an inline span. The client styles these and
reacts to some of them:

| class / element | client use |
|---|---|
| `<div class='prompt' bars='hp/max ep/max mana/max'>prompt text</div>` | vitals bars; the prompt text sits in the input bar. Hidden in the log by default. |
| `<h1 class='room_title' zone='' exits='n e ' map='x,y' roomid='room@zone'>` | room card, compass (lit exits), map (`../img/<zone>.jpg`, centred on `map`) |
| `tell_self` `tell_other` (`from=` `to=`) | conversation per person |
| `say_*` `ask_*` `whisper` (`from=`) | the Room conversation |
| `shout_*` (`from=`) | Shouts |
| `comm_say` `comm_emote` `comm_join` `comm_leave` (`chan=` `from=`) | conversation per channel |
| `hit_*` `miss_*` `nodam_*` `shield_*` with `_me` (you attack), `_opponent` (you are attacked), `_other` | combat dock and the folded combat line in the log |
| `death`, `xpgain` | shown as is |
| `<a cmd='help #'>topic</a>` | command link; `#` is the link text |

Without the `from=`/`to=`/`chan=` attributes (older servers) the client reads the names
from the message text instead.

## Server wishlist

Extra output from vme / DIL that would make the client better. Each is small on the
server side and replaces guessing in the client.

1. **Combat lines name both sides.** `<div class='hit_me' att='<id>' def='<id>' dmg='7'>`
   with the `$combat` ids. The dock could then show each being's last action and damage
   without matching names in the text. (Today the client looks for the being's name.)
2. **Death when the fight ends in one blow.** A one-round kill sends no `$combat` at all,
   so the dock never shows it. A single `{"round":1,"list":[…],"end":true}` would do.
3. **Room contents as data.** `<data type='$room'>{"id":"temple@udgaard","exits":{"n":{"to":"…","door":"open"}},
   "mobs":[…],"objs":[…]}</data>` would allow an automapper (rooms joined by exits) and
   clickable people and things in the room card.
4. **Inventory / equipment / affects as data**, sent when they change, for live side panels
   without typing `inventory` and `equipment`.
5. **Channel list.** Which channels the player is on (`comm`), so the conversations panel
   can offer them before anyone has spoken.
6. **Who list as data** (`$who`), for a friends/online panel.
7. **Group as data** (`$group`): members with hp/ep/mana percent, for a group panel.
8. **Opponent condition in the prompt** for players without websocket combat packets
   (telnet clients would benefit too).
9. **Mark private text.** A flag on tells/whispers so the client can offer notifications
   only for messages addressed to the player.
10. **A version handshake**, e.g. `<data type='$hello'>{"protocol":2}</data>` at connect, so
    the client knows which of the above the server supports.
