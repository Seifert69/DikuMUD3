var webSocket = null;
var bScreenReader = true;
var nRowCount = 0;
var bPaged = 0;
var sPaged = "";

var g_crosshairs = new Image;

var g_sImage = "img/logo.gif";
var g_nLastSend = Math.round(Date.now() / 1000);

var g_nHp = 1;
var g_nHpMax = 1;
var g_nEp = 1;
var g_nEpMax = 1;
var g_nMp = 1;
var g_nMpMax = 1;

var g_aCmdHistory = new Array(50); // History of 50 commands
var g_nCmdPos = 0;
var g_nHistoryPos = 0;

// Dont store these commands in the history array
var g_aIgnoreCommands = ['north', 'east', 'south', 'west', 'up', 'down', 'northeast', 'northwest', 'southeast', 'southwest', '!', 'look'];

// These two arrays are used for triggers.
var g_aTriggerRegExp = []; // [new RegExp("^(.+) leaves (.+?)\.\<br\/\>$", "gim"), new RegExp("^(.+) has arrived from (.+)\.\<br\/\>$", "gim")];
var g_aTriggerCmd = [];    // ["emote waves goodbye to $1", "hug $1;look $2;emote likes $1"];
var g_nTriggerCircular = 0; // Count potential circular references
var g_nTriggerCounter  = 0; // Count tiggers per time unit to detect cyclic triggers
var g_nTriggerLastTime = 0; // Timer needed for trigger counter

var g_aAliasName = [];
var g_aAliasData = [];
var g_nAliasCircular = 0; // Count potential circular references 

var g_aVariableName = [];
var g_aVariableData = [];


/* ====== A FEW STRING FUNCTIONS ======= */

// Alias   <name> cmd;cmd;cmd
// Trigger <name> regexp commandstring
//
String.prototype.replaceAll = function (str1, str2, ignore) {
    return this.replace(new RegExp(str1.replace(/([\/\,\!\\\^\$\{\}\[\]\(\)\.\*\+\?\|\<\>\-\&])/g, "\\$&"), (ignore ? "gi" : "g")), (typeof (str2) == "string") ? str2.replace(/\$/g, "$$$$") : str2);
}

function getStrFirstWord(str) {
    let spaceIndex = str.indexOf(' ');
    return spaceIndex === -1 ? str : str.substr(0, spaceIndex);
}

function getStrRemainder(str) {
    let spaceIndex = str.indexOf(' ');
    return spaceIndex === -1 ? "" : str.slice(spaceIndex+1);
}

/* The string str contains $1, $2 ...
 * and the regexp match array a replaces $1 with a[1], etc.
 * returns the substituted string. 
 */
function dlrReplace(str, a) {
    var i;
    var p;

    // Skip the first match entry
    for (i = 0; i < a.length; i++) {
        str = str.replaceAll("$" + i, a[i]);
        //console.log("Replaced string = " + str);
        //console.log("match[" + i + "] = " + a[i]);
    }

    return str;
}


/* ========== ALIAS ============= */

function aliasCheck(str) 
{
    if (g_nAliasCircular > 5)
    {
        outputText("Alias nesting more than five levels, aborting alias.<br/>", false);
        return 1;
    }

    var cmd = getStrFirstWord(str);
    var a = str.split(" ");
    a[0] = getStrRemainder(str);

    for (var i=0; i < g_aAliasData.length; i++)
    {
        if (g_aAliasName[i].toLowerCase() == cmd.toLowerCase())
        {
            g_nAliasCircular++;
            var t = g_aAliasData[i];
            t = dlrReplace(t, a);
            manycmds(t);
            InputFocus("");
            g_nAliasCircular--;
            return 1;
        }
    }

    return 0;
}


function setAlias(item) {
    var s;

    //console.log("SET ALIAS");

    g_aAliasName = [];
    g_aAliasData = [];

    for (var i = 0; i < item.length; i++) {
        s = item[i].innerText;
        g_aAliasName[i] = getStrFirstWord(s).trim();
        g_aAliasData[i] = getStrRemainder(s).trim();
        //console.log("Array[" + i + "] = " + g_aAliasName[i] + "=" + g_aAliasData[i]);
    }
}



/* Splits str by ; and sends each as its own command
 */
function manycmds(str) {
    var cmds = str.split(';');

    for (var i = 0; i < cmds.length; i++)
    {
        if ((g_nAliasCircular > 5) || (g_nTriggerCircular > 5))
        {
            outputText("Alias nesting more than five levels, aborting alias.<br/>", false);
            return;
        }
    
        sendCommand(cmds[i], false, false, true);
    }
}


function testTrigger(s, regexp, mycmd)
{
    var s;

    if (g_nTriggerCircular > 5)
    {
        outputText("Triggers nesting more than five levels, aborting.<br/>", false);
        return 1;
    }

    var i,j;
    i = Math.round(Date.now() / 1000);
    j = i - g_nTriggerLastTime; // seconds since last checkpoint
    g_nTriggerLastTime = i;
    g_nTriggerCounter -= j;

    if (g_nTriggerCounter < 0)
        g_nTriggerCounter = 0;

    if (g_nTriggerCounter > 5)
    {
        outputText("Triggers firing too fast, aborting.<br/>", false);
        return 1;
    }

    //console.log("TRIGGER TEST:" + String(regexp));
    const matches = s.matchAll(regexp);

    for (const match of matches) {
        //console.log("match = " + match);
        //console.log("match.index = " + match.index);
        g_nTriggerCircular++;
        g_nTriggerCounter++;
        s = dlrReplace(mycmd, match);
        manycmds(s);
        g_nTriggerCircular--;
    }

    return 0;
}


function variableReplace(str)
{
    console.log("==========================");
    for (var i = 0; i < g_aVariableName.length; i++)
    {
        str = str.replaceAll("@" + g_aVariableName[i], g_aVariableData[i]);
        console.log("REPLACE @" + g_aVariableName[i] + "="+ g_aVariableData[i] + "|" + str);
    }

    return str;
}


function setVariable(item) {
    var s;

    console.log("Variable:");

    g_aVariableName = [];
    g_aVariableData = [];

    for (var i = 0; i < item.length; i++) {
        s = item[i].innerText;
        g_aVariableName[i] = getStrFirstWord(s).trim();
        g_aVariableData[i] = getStrRemainder(s).trim();
        console.log("Variable[" + i + "] = " + g_aVariableName[i] + "=" + g_aVariableData[i]);
    }
}


function setTrigger(item)
{
    var s, t, u;
    var j;

    console.log("Trigger:");

    g_aTriggerRegExp = [];
    g_aTriggerCmd = [];

    j = 0;
    for (var i = 0; i < item.length; i++) {
        s = item[i].innerText;
        //console.log(s);

        if ((s.indexOf("/") === -1) || (s.indexOf("/") === s.lastIndexOf("/")))
        {
            outputText("Trigger "+s+": missing slash around the trigger expression /trigger/. Remove trigger and try again.<br/>", false);
            continue;
        }

        t = s.substring(s.indexOf("/") + 1, s.lastIndexOf("/")).trim();
        if (t.length === 0)
        {
            outputText("Trigger "+s+": empty expression between slash: /empty/ commands<br/>", false);
            continue;
        }

        u = s.slice(s.lastIndexOf("/")+1).trim();
        if (u.length === 0)
        {
            outputText("Trigger "+s+": commands is empty: /tigger/ empty<br/>", false);
            continue;
        }

        console.log("TRIGGER:" + t + "=" + u);
        t = variableReplace(t);
        g_aTriggerRegExp[j] = new RegExp(t, "gim");
        g_aTriggerCmd[j] = u;
        j++;
    }
    console.log("Trigger end");
}


/*
 * Add a command string 's' to the history array
 *
 */
function HistoryAdd(s) {
    if (s.length === 0 || !s.trim())
        return;

    var i;

    for (i = 0; i < g_aIgnoreCommands.length; i++)
        if (g_aIgnoreCommands[i].indexOf(s) == 0)
        return;

    i = (g_nCmdPos-1) % g_aCmdHistory.length;
    if (g_aCmdHistory[i] == s)
        return;
        
    g_nCmdPos = g_nCmdPos % g_aCmdHistory.length;
    g_aCmdHistory[g_nCmdPos] = s;
    g_nCmdPos += 1;
}

/*
 * Get the nPos last command
 *   If nPos is one it's the last command, 2 the second last, etc.
 *   If nPos is zero or less it will return the empty string ""
 */
function HistoryGet(nPos) {
    if (nPos <= 0)
        return "";

    nPos = g_nCmdPos - nPos;
    while (nPos < 0)
        nPos += g_aCmdHistory.length;

    nPos = nPos % g_aCmdHistory.length;
    console.log(nPos + " = " + g_aCmdHistory[nPos]);
    return g_aCmdHistory[nPos];
}



/**
    * Event handler for clicking on button "Disconnect"
    */
function onDisconnectClick() {
    webSocket.close();
}


function cmd(str) {
    /*if (modalVisible())
        modalHide();*/

    sendCommand(str, true, true, true);
}


// Recursively traverse element tree and change <a cmd> to work
function TraverseTreeAndFix(myelem) {
    var nodes = myelem.childNodes;
    for (var i = 0; i < nodes.length; i++) {
        if (!nodes[i])
            continue;

        if (nodes[i].nodeName == 'A') {
            nodes[i].href = '#';

            var s = String(nodes[i].attributes.getNamedItem('cmd').value);

            // I would have liked to do the split / join neato but didn't work.
            var n = s.indexOf("#");
            if (n > -1)
                s = s.slice(0, n) + nodes[i].innerText + s.slice(n + 1);

            nodes[i].data = s;
            nodes[i].onclick = function (e) { cmd(this.data); };
        }

        if (nodes[i].childNodes.length > 0)
            TraverseTreeAndFix(nodes[i]);
    }
    return;

}

// Show the popover modal & copy content from paged.
function modalShow() {
    var item, pgd;

    pgd = document.getElementById("acp_text").firstChild;

    // This is terrible. I dont understand why I can't just copy an object :-))
    // So instead for now I have to build a new one. I cant use innerhtml either
    // because the assigned data and onclick events dont come back out from innerhtml
    //
    var item = document.createElement("div");
    item.setAttribute("style", "display: inline");
    item.innerHTML = pgd.innerHTML;
    TraverseTreeAndFix(item); // Transform links

    document.getElementById("modtext").firstChild.replaceWith(item);

    document.getElementById("myModal").style.display = "block";
}

function modalVisible() {
    return document.getElementById("myModal").style.display != "none";
}

function modalHide() {
    document.getElementById("myModal").style.display = "none";
    InputFocus(null);
}

function pagedSet(item) {
    outputItem(item);

    var item2 = document.createElement("div");
    item2.setAttribute("style", "display: inline");
    item2.innerHTML = item.innerHTML;

    TraverseTreeAndFix(item2);;

    //console.log("PAGED CONTENT" + item.innerHTML);
    //document.getElementById("acp_text").innerHTML = item.innerHTML;
    document.getElementById("acp_text").firstChild.replaceWith(item2);
    //modalShow();
}

// str is of the exit format "N E U NW "... 
function setExits(str) {
    var fields, buff;

    buff = "";
    fields = str.split(/ /);

    var i;
    for (i = 0; i < fields.length; i++)
        buff = buff + " <a href='#' cmd='#' onclick='cmd(\x22" + fields[i] + "\x22)'>" + fields[i] + "</a>";

    document.getElementById("ace_text").innerHTML = buff;
}


function updateBars() {
    var e;

    e = document.getElementById("hp-bar");
    e.style.width = Math.round((100 * g_nHp) / g_nHpMax) + "%";

    e = document.getElementById("ep-bar");
    e.style.width = Math.round((100 * g_nEp) / g_nEpMax) + "%";

    e = document.getElementById("mp-bar");
    e.style.width = Math.round((100 * g_nMp) / g_nMpMax) + "%";
}


// str is of the exit format "N E U NW "... 
function setBars(str) {
    var fields, buff;

    fields = str.split(/ /);  // Get the three bars

    if (fields.length != 3) {
        console.log("setBars() format not as expected");
        return;
    }

    buff = fields[0].split(/\//);
    g_nHp = Number(buff[0]);
    g_nHpMax = Number(buff[1]);

    buff = fields[1].split(/\//);
    g_nEp = Number(buff[0]);
    g_nEpMax = Number(buff[1]);

    buff = fields[2].split(/\//);
    g_nMp = Number(buff[0]);
    g_nMpMax = Number(buff[1]);

    updateBars();
}

function setMap(szone, smap) {
    if (szone == null)
        return;

    var x = -1;
    var y = -1;

    if (smap != null) {
        //console.log(smap);
        var fields = smap.split(",");

        if (fields.length >= 2) {
            x = parseInt(fields[0]);
            y = parseInt(fields[1]);
        }
    }

    if (szone != "")
        g_sImage = "img/" + szone + ".jpg";
    else
        g_sImage = "img/logo.gif";

    var mye = document.getElementById("ac_map");
    var xw, yh;
    xw = mye.clientWidth;
    yh = mye.clientHeight;

    if (x == -1) {
        var mye = document.getElementById("mymap");
        mye.style = "background-image: url('" + g_sImage + "'); background-size: contain; width: 100%; height: 100%; background-position: center; background-repeat: no-repeat;";

        mye = document.getElementById("crosshairs");
        mye.hidden = true;    
        return;
    }

    //console.log("map: " + x + "," + y + "   :: window size " + xw + "," + yh);

    // Do the math to put the coordinate in the center of the window
    // x = 0 => right side sticks; -200 creates dark edge on the left, +400 moves 
    // y = 0 => top, y = - 400 moves picture down

    // Center coordinate
    x = x - xw / 2;
    y = y - yh / 2;

    // Coordinate conversion
    x = -x
    y = -y

    var mye = document.getElementById("mymap");
    mye.style = "background-image: url('" + g_sImage + "'); width: " + xw + "px; height: " + yh + "px; background-repeat: no-repeat; background-position: left " + x + "px top " + y + "px;";

    x = xw/2-18;
    y = yh/2-18;

    mye = document.getElementById("crosshairs");
    mye.style = "position: relative; left: " + x + "px; top: " + y + "px;";
    mye.hidden = false;
    //console.log(mye.style);
    /* mye.style = "position:relative; right:"+x+"px; bottom:"+y+"px;";*/
}


function outputReader(item) {
    // Some screen readers work way better with lists (?)
    var list = document.createElement("li");
    list.appendChild(item);

    var chatContent = document.getElementById("converse");
    chatContent.appendChild(list);
    if (nRowCount > 200)  // Max 200 list items
    {
        chatContent.removeChild(chatContent.childNodes[0]);
        nRowCount -= 1;
    }
}

function outputNormal(item) {
    var msgo = document.getElementById("converse");

    if (nRowCount > 200)  // Max 200 child items 
    {
        msgo.removeChild(msgo.childNodes[0]);
        nRowCount -= 1;

        // When it eventually overruns
        if (msgo.childNodes.length > 250)
            while (msgo.childNodes.length > 200)
                msgo.removeChild(msgo.childNodes[0]);
    }
    //msgo.innerHTML += str;
    msgo.appendChild(item);
}

function outputItem(item) {
    nRowCount += 1;

    if (bScreenReader) // Test if more accessible in lists. 
        outputReader(item);
    else
        outputNormal(item);

    // AUTO SCROLL
    document.getElementById("al_text").scrollTop = document.getElementById("al_text").scrollHeight;
}


// bParse parameter. If false then it skips all
function outputText(str, bParse) {
    console.log("WS[" + str + "]");

    if (bParse)
    {
        // I was wondering if using something like <span class='paged'>
        // might be a better marker. But it would leave a non-unique ending
        // of </span> which could appear elsewhere. So I thought for now
        // <paged> is better
        if (str.slice(0, 7) == "<paged>") {
            bPaged = 1;
            sPaged = "";
            str = str.slice(7); // Remove first 7 characters
        }

        if (bPaged) {
            //console.log("PAGED");
            if (str.slice(-8) == "</paged>") {
                bPaged = 0;
                sPaged += str.slice(0, -8); // remove last 8 characters

                var item = document.createElement("div");
                item.setAttribute("style", "display: inline");
                item.innerHTML = sPaged;

                TraverseTreeAndFix(item);

                pagedSet(item);
            }
            else {
                sPaged += str;
            }
            return;
        }
    }

    var item = document.createElement("div");
    item.setAttribute("style", "display: inline");
    item.innerHTML = str;
    TraverseTreeAndFix(item);

    if (bParse)
    {
        if (str.slice(0, 25) == "<div class='prompt' bars=") {
            var mye = item.firstChild.getAttribute("bars");

            if (mye != "")
                setBars(mye);
        }
        else if (str.slice(0, 28) == "<h1 class='room_title' zone=") {
            setExits(item.firstChild.getAttribute("exits"));
            setMap(item.firstChild.getAttribute("zone"), item.firstChild.getAttribute("map"));
        }
        else if (str.slice(0, 8) == "<script>") {
            // For now I can't figure out a better method
            // Preferably I would have loved for the script to
            // simply execute when added to the lists.
            if (str.slice(-9) == "</script>") {
                var myscript = str.slice(0, -9).slice(8);
                //console.log(myscript);
                eval(myscript);
                InputFocus(null);
            }
            return;
        }
        else if (str.slice(0, 6) == "<data ") {
            if (str.slice(-7) != "</data>") {
                console.log("Got incomplete <data> package.");
                return;
            }

            var mytype = item.firstChild.getAttribute("type");
            var myitem = item.getElementsByTagName("li");

            if (mytype == "$alias")
                setAlias(myitem);
            else if (mytype == "$trigger")
                setTrigger(myitem);
            else if (mytype == "$var")
                setVariable(myitem);
            else
            {
                console.log("Unknown data type " + mytype);
            }

            return;
        }
    } // bParse

    outputItem(item);

    // It gets super complicated to write triggers if the HTML is included
    var txt = item.innerText;

    if (bParse)
    {
        for (var i=0; i < g_aTriggerRegExp.length; i++)
            if (testTrigger(txt, g_aTriggerRegExp[i], g_aTriggerCmd[i]))
                return;
    }
}


/**
    * Open a new WebSocket connection using the given parameters
    */
function openWSConnection(protocol, hostname, port, endpoint) {

    var webSocketURL = protocol + "://" + hostname + ":" + port + endpoint;

    outputText("Connecting to: " + webSocketURL + "<br/>", false);

    try {
        webSocket = new WebSocket(webSocketURL);
        webSocket.onopen = function (openEvent) {
            console.log("WebSocket OPEN: " + JSON.stringify(openEvent, null, 4) + "<br/>");
            document.getElementById("btnSend").disabled = false;
            //document.getElementById("message").focus();
            //document.getElementById("message").select();

            // document.getElementById("incomingMsgOutput").innerHTML = "";
            outputText("<br/>Connected to <i><b>Valhalla</b> MUD</i><br/>", false);
            webSocket.send("");

            InputFocus(null);
        };
        webSocket.onclose = function (closeEvent) {
            outputText("WebSocket CLOSE: " + JSON.stringify(closeEvent, null, 4) + "<br/>", false);
            document.getElementById("btnSend").disabled = true;
        };
        webSocket.onerror = function (errorEvent) {
            outputText("WebSocket ERROR: " + JSON.stringify(errorEvent, null, 4) + "<br/>", false);
        };
        webSocket.onmessage = function (messageEvent) {
            var wsMsg = messageEvent.data;
            // console.log("WebSocket MESSAGE: " + wsMsg);
            outputText(wsMsg, true);
            /* Cant trigger on any error string .... if (wsMsg.indexOf("error") > 0) {
                outputText("error: " + wsMsg.error + "<br/>");
            } else {
            }*/
        };
    } catch (exception) {
        outputText("Exception: " + exception + "<br/>", false);
        console.error(exception);
    }
}


/*
 * If str is null the value is left unchanged, otherwise it's set to str
 */
function InputFocus(str) {
    var myfld;
    
    if (isPasswordActive())
        myfld = document.getElementById("pwd");
    else
        myfld = document.getElementById("message");

    if (str != null)
        myfld.value = str;
    myfld.focus();
    myfld.select();
}


function sendCommand(str, bFocus, bHistory, bEcho)
{
    if (webSocket.readyState != WebSocket.OPEN) {
        outputText("webSocket is not open: " + webSocket.readyState, false);
        return false;
    }

    g_nLastSend = Math.round(Date.now() / 1000);

    str = str.trim();

    if (bHistory)
        HistoryAdd(str);

    if (aliasCheck(str))
        return;

    if (bEcho)
        outputText("<div class='echo'>" + str + "</div><br/>", false); // ECHO COMMAND

    webSocket.send(str);

    if (bFocus)
        InputFocus("");

    return true;
}

function isPasswordActive() {
    return document.getElementById("pwd").style.display != "none";
}

/**
    * Send a message to the WebSocket server
    */
function onSendClick() {
    g_nHistoryPos = 0;
    
    var myfld;
    var bPwdActive = isPasswordActive();

    if (bPwdActive)
        myfld = document.getElementById("pwd");
    else
        myfld = document.getElementById("message");

    sendCommand(myfld.value, true, !bPwdActive, !bPwdActive);
}

function shSettings() {
    var str;
    str = "<H1>Select Theme:</h1><select id=theme onchange=switchTheme()><option value=style.css>Default Theme</option><option value=midnight.css>Midnight Theme</option><option value=silver.css>Silver Theme</option></select><br/><br/>";

    var item = document.createElement("div");
    item.setAttribute("style", "display: inline");
    item.innerHTML = str;

    document.getElementById("modtext").firstChild.replaceWith(item);

    document.getElementById("myModal").style.display = "block";
}

function toggleSettings() {
  var d = document.getElementById("banner");
  if (d.style.display == "none") {
    d.style.display = "block";
  } else {
    d.style.display = "none";
  }
}

function switchTheme()
        {
          var th = document.getElementById("theme");
          var newstylesheet = th.options[th.selectedIndex].value;
          var stylesheet = document.getElementById("stylesheet");
             stylesheet.setAttribute('href', newstylesheet);
            }

function onMapClick()
{
    var item = document.createElement("img");
    item.id = "id";
    item.src = g_sImage;
    item.setAttribute("style", "display: inline");
    item.style.maxHeight = "500px";
    item.style.maxWidth  = "auto";

    document.getElementById("modtext").firstChild.replaceWith(item);

    document.getElementById("myModal").style.display = "block";
}


function onMainClick()
{
    var item, pgd;

    pgd = document.getElementById("al_text");

    // This is terrible. I dont understand why I can't just copy an object :-))
    // So instead for now I have to build a new one. I cant use innerhtml either
    // because the assigned data and onclick events dont come back out from innerhtml
    //
    var item = document.createElement("div");
    item.setAttribute("style", "display: inline");
    item.innerHTML = pgd.innerHTML;
    TraverseTreeAndFix(item); // Transform links

    document.getElementById("modtext").firstChild.replaceWith(item);    

    document.getElementById("myModal").style.display = "block";
}


function keepAlive()
{
    var sec = Math.round(Date.now() / 1000);

    if (sec-g_nLastSend < 250)
        return;

    console.log("Keep alive");
    sendCommand("", false, false, false);
}
