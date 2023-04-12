const ConnectionState = {
    Disconnected: 0,
    Connecting: 1,
    Connected: 2
}
var serverConnectionState = ConnectionState.Disconnected;

var timerLoop;
var websock;

// allow for multiple keys down simultaneously.
var keysPressed = {};

// DOM object events we care about.
document.addEventListener("click", on_click);
document.addEventListener("keydown", on_keydown);
document.addEventListener("keyup", on_keyup);

class Timer {
    contructor() {
        this.isRunning = false;
        this.interval = 0;
        var functionToCall;
    }

    adjust(intervalmS) {
        this.interval = intervalmS;
        this.restart();
    }

    start(callbackFunction, intervalmS) {
        this.functionToCall = callbackFunction;
        this.adjust(intervalmS);
    }

    restart() {
        if (this.isRunning)
            this.stop();

        this.timerID = setInterval(this.functionToCall, this.interval);
        this.isRunning = true;
    }

    stop() {
        if (this.isRunning)
            clearInterval(this.timerID);
        this.isRunning = false;
    }
}

function make_websocket_url() {
    var protocol;
    if (window.location.protocol == "https:")
        protocol = "wss:"
    else if (window.location.protocol == "http:")
        protocol = "ws:"

    var url = protocol + "//" + window.location.hostname + ":" + window.location.port + "/";

    return url;
}

function websock_onopen(event) {
    serverConnectionState = ConnectionState.Connected;
}

function websock_onmessage(event) {
    console.log("websock_onmessage: " + event.data);
}

function websock_onclose(event) {
    serverConnectionState = ConnectionState.Disconnected;
}

function websock_onerror(event) {

}

function WebsocketConnect()
{
    console.log("Connection initiated");

    serverConnectionState = ConnectionState.Connecting;
    websock = new WebSocket(make_websocket_url());

    websock.binaryType = 'arraybuffer';

    websock.onopen = websock_onopen;
    websock.onmessage = websock_onmessage;
    websock.onclose = websock_onclose;
    websock.onerror = websock_onerror;
}

function WebsocketConnected()
{
    websock.send("This is a test.");
}

// This runs at the main timerLoop interval to run client-side periodic events.
// Because javascript is loaded asynchronously, and in random order, this
// function maintains a state machine for the WebSocket connection.
//
// Took me a bit to figure out why Tim did it this way.
function timerLoopTick()
{
    switch (serverConnectionState)
    {
        case ConnectionState.Disconnected:
            WebsocketConnect();
            break;

        case ConnectionState.Connecting:
            console.log("Awaiting connect completion.");
            break;

        case ConnectionState.Connected:
            WebsocketConnected();
            break;
    }
}

function on_click(event) {
    console.log("click: " + event.clientX + ", " + event.clientY);
}

function on_keydown(event) {
    keysPressed[event.key] = true;
}

function on_keyup(event) {
    keysPressed[event.key] = false;
}

function Init() {
    timerLoop = new Timer();
    timerLoop.start(timerLoopTick, 50);
}
