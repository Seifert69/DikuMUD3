var webSocket = null;
var bScreenReader = true;
var nRowCount = 0;
var bPaged = 0;
var sPaged = "";

var g_nHp = 1;
var g_nHpMax = 1;
var g_nEp = 1;
var g_nEpMax = 1;
var g_nMp = 1;
var g_nMpMax = 1;

var g_aCmdHistory = new Array(50); // History of 50 commands
var g_nCmdPos     = 0;
var g_nHistoryPos = 0; 

// Dont store these commands in the history array
var g_aIgnoreCommands = ['north', 'east', 'south', 'west', 'up', 'down', 'northeast', 'northwest', 'southeast', 'southwest', '!', 'look'];

/*
 * Add a command string 's' to the history array
 *
 */
function HistoryAdd(s) {
    if (s.length === 0 || !s.trim())
        return;

    var i;

    for (i=0; i < g_aIgnoreCommands.length; i++)
        if (g_aIgnoreCommands[i].indexOf(s) == 0)
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


function cmd(str)
{
    /*if (modalVisible())
        modalHide();*/

    sendCommand(str, true);
}


// Recursively traverse element tree and change <a cmd> to work
function TraverseTreeAndFix(myelem)
{
    var nodes =myelem.childNodes;
    for (var i = 0; i < nodes.length; i++)
    {
        if(!nodes[i])
            continue;
        
        if (nodes[i].nodeName == 'A')
        {
            nodes[i].href = '#';

            var s = String(nodes[i].attributes.getNamedItem('cmd').value);

            // I would have liked to do the split / join neato but didn't work.
            var n = s.indexOf("#");
            if (n > -1)
                s = s.slice(0,n) + nodes[i].innerText + s.slice(n+1);

            nodes[i].data = s;
            nodes[i].onclick = function(e) { cmd(this.data); };
        }
            
        if(nodes[i].childNodes.length > 0)
            TraverseTreeAndFix(nodes[i]);
    }
    return;

}

// Show the popover modal & copy content from paged.
function modalShow(){
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

function modalVisible(){
    return document.getElementById("myModal").style.display != "none";
}

function modalHide(){
    document.getElementById("myModal").style.display = "none";
    InputFocus(null);
}

function pagedSet(item){
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
function setExits(str)
{
    var fields, buff;

    buff = "";
    fields = str.split(/ /);

    var i;
    for (i=0; i < fields.length; i++)
        buff = buff + " <a href='#' cmd='#' onclick='cmd(\x22"+fields[i]+"\x22)'>"+fields[i]+"</a>";

    document.getElementById("ace_text").innerHTML = buff;
}


function updateBars()
{
    var e;

    e = document.getElementById("hp-bar");
    e.style.width = Math.round((100 * g_nHp) / g_nHpMax) + "%";

    e = document.getElementById("ep-bar");
    e.style.width = Math.round((100 * g_nEp) / g_nEpMax) + "%";

    e = document.getElementById("mp-bar");
    e.style.width = Math.round((100 * g_nMp) / g_nMpMax) + "%";
}


// str is of the exit format "N E U NW "... 
function setBars(str)
{
    var fields, buff;

    fields = str.split(/ /);  // Get the three bars

    if (fields.length != 3)
    {
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

function setMap(szone, smap)
{
    if (szone == null)
        return;

    var x = -1;
    var y = -1;

    if (smap != null)
    {
        console.log(smap);
        var fields = smap.split(",");

        if (fields.length >= 2)
        {
            x = parseInt(fields[0]);
            y = parseInt(fields[1]);
        }
    }

    var s;
    if (szone != "") 
        s = "img/"+szone+".jpg";
    else
        s = "img/logo.gif";

    var mye = document.getElementById("ac_map");
    var xw, yh;
    xw = mye.clientWidth;
    yh = mye.clientHeight;    

    if (x == -1)
    {
        var mye = document.getElementById("mymap");
        mye.style = "background-image: url('"+s+"'); background-size: contain; width: 100%; height: 100%; background-position: center; background-repeat: no-repeat;";
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
    mye.style = "background-image: url('"+s+"'); width: "+xw+"px; height: "+yh+"px; background-repeat: no-repeat; background-position: left "+x+"px top "+y+"px;";
    //console.log(mye.style);
    /* mye.style = "position:relative; right:"+x+"px; bottom:"+y+"px;";*/
}


function outputReader(item)
{
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

function outputNormal(item)
{
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

function outputItem(item)
{
    nRowCount += 1;

    if (bScreenReader) // Test if more accessible in lists. 
        outputReader(item);
    else
        outputNormal(item);

    // AUTO SCROLL
    document.getElementById("al_text").scrollTop = document.getElementById("al_text").scrollHeight;
}


function outputText(str)
{
    console.log("WS[" + str + "]");

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
        else
        {
            sPaged += str;
        }
        return;
    }

    var item = document.createElement("div");
    item.setAttribute("style", "display: inline");
    item.innerHTML = str; 
    TraverseTreeAndFix(item);

    if (str.slice(0, 25) == "<div class='prompt' bars=")
    {
        var mye = item.firstChild.getAttribute("bars");

        if (mye != "")
            setBars(mye);
    }
    else if (str.slice(0, 28) == "<h1 class='room_title' zone=")
    {
        setExits(item.firstChild.getAttribute("exits"));
        setMap(item.firstChild.getAttribute("zone"), item.firstChild.getAttribute("map"));
    }
    else if (str.slice(0, 8) == "<script>") 
    {
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

    outputItem(item);
}


/**
    * Open a new WebSocket connection using the given parameters
    */
function openWSConnection(protocol, hostname, port, endpoint) {

    var webSocketURL = protocol + "://" + hostname + ":" + port + endpoint;

    outputText("Connecting to: " + webSocketURL + "<br/>");

    try {
        webSocket = new WebSocket(webSocketURL);
        webSocket.onopen = function (openEvent) {
            console.log("WebSocket OPEN: " + JSON.stringify(openEvent, null, 4) + "<br/>");
            document.getElementById("btnSend").disabled = false;
            //document.getElementById("message").focus();
            //document.getElementById("message").select();

            // document.getElementById("incomingMsgOutput").innerHTML = "";
            outputText("<br/>Connected to <i><b>Valhalla</b> MUD</i><br/>");
            webSocket.send("");

            InputFocus(null);
        };
        webSocket.onclose = function (closeEvent) {
            outputText("WebSocket CLOSE: " + JSON.stringify(closeEvent, null, 4) + "<br/>");
            document.getElementById("btnSend").disabled = true;
        };
        webSocket.onerror = function (errorEvent) {
            outputText("WebSocket ERROR: " + JSON.stringify(errorEvent, null, 4) + "<br/>");
        };
        webSocket.onmessage = function (messageEvent) {
            var wsMsg = messageEvent.data;
            // console.log("WebSocket MESSAGE: " + wsMsg);
            outputText(wsMsg);
            /* Cant trigger on any error string .... if (wsMsg.indexOf("error") > 0) {
                outputText("error: " + wsMsg.error + "<br/>");
            } else {
            }*/
        };
    } catch (exception) {
        outputText("Exception: " + exception + "<br/>");
        console.error(exception);
    }
}


/*
 * If str is null the value is left unchanged, otherwise it's set to str
 */
function InputFocus(str) {
    var myfld = document.getElementById("message");
    if (str != null)
        myfld.value = str;
    myfld.focus();
    myfld.select();
}


function sendCommand(str, bEcho)
{
    if (bEcho)
        HistoryAdd(str);

    if (webSocket.readyState != WebSocket.OPEN) {
        outputText("webSocket is not open: " + webSocket.readyState);
        return false;
    }

    webSocket.send(str);

    if (bEcho)
        outputText(str + "<br/>"); // ECHO COMMAND

    InputFocus("");
    return true;
}

/**
    * Send a message to the WebSocket server
    */
function onSendClick() {
    g_nHistoryPos = 0;
    var myfld = document.getElementById("message");
    sendCommand(myfld.value, myfld.getAttribute('type') != "password");
}
