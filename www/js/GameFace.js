var loopTimer;
var myWebSock;

var canvas = document.getElementById('gameCanvas');
var ctx = canvas.getContext("2d");
var borderWidth = 4;
var centerX;
var centerY;

// allow for multiple keys down simultaneously.
var keysPressed = {};
const textEncoder = new TextEncoder();


// DOM object events we care about.
document.addEventListener("click", on_click);
document.addEventListener("keydown", on_keydown);
document.addEventListener("keyup", on_keyup);

const littleEndian = ((() => {
    const buffer = new ArrayBuffer(2);
    new DataView(buffer).setInt16(0, 256, true);
    return new Int16Array(buffer)[0] === 256;
})() == 0) ? 0xAA : 0xAB;

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

class WebSock {
    constructor() {
        this.sock = new WebSocket(this.MakeWebSockUrl());
        this.isConnected = false;
        this.sock.binaryType = 'arraybuffer';

        this.sock.onopen = this.OnOpen;
        this.sock.onmessage = this.OnMessage;
        this.sock.onclose = this.OnClose;
        this.sock.onerror = this.OnError;
    }

    OnOpen(event) {
        console.log("Connected");
    }

    OnMessage(event) {
        console.log("WebSock.OnMessage: " + event.data);
    }

    OnClose(event) {
        console.log("Disconnected");
    }

    OnError(event) {
        console.log("Error: " + event.data);
    }

    MakeWebSockUrl() {
        var protocol;
        if (window.location.protocol == "https:")
            protocol = "wss:"
        else if (window.location.protocol == "http:")
            protocol = "ws:"

        var url = protocol + "//" + window.location.hostname + ":" + window.location.port + "/";

        return url;
    }

    Send(what) {
        this.sock.send(what);
    }

    Tick() {
        switch (this.sock.readyState) {
            case 0:
                console.log("CONNECTING...");
                break;

            case 1:
                this.isConnected = true;
                break;

            case 2:
                console.log("CLOSING...");
                this.isConnected = false;
                break;

            case 3:
                console.log("CLOSED.");
                this.isConnected = false;
                break;
        }
    }
}

// This runs at the main loopTimer interval to run client-side periodic events.
// Because javascript is loaded asynchronously, and in random order, this
// function maintains a state machine for the WebSocket connection.
//
// Took me a bit to figure out why Tim did it this way.
function timerLoopTick()
{
    if (typeof myWebSock === 'undefined') {
        myWebSock = new WebSock();
    }
    else {
        myWebSock.Tick();
        if (myWebSock.isConnected) {
            if (typeof OnConnectedTimerTick != 'undefined')
                OnConnectedTimerTick();
        }
    }
}

function on_click(event) {
    console.log("click: " + event.clientX + ", " + event.clientY);
}

function ProcessKeyEvent(key, isDown) {
    console.log("key: " + key);
    var buff = new ArrayBuffer(4);
    var view = new Uint8Array(buff);
    view[0] = littleEndian;
    view[1] = 0x12;
    view[2] = isDown;
    var foo = textEncoder.encode(key);
    view[3] = foo;
    myWebSock.Send(buff);
}

// filter multiple key down's (keyboard repeat)
function on_keydown(event) {
    if (!keysPressed[event.key]) {
        keysPressed[event.key] = true;
        ProcessKeyEvent(event.key, true)
    }
}

function on_keyup(event) {
    if (keysPressed[event.key]) {
        keysPressed[event.key] = false;
        ProcessKeyEvent(event.key, false)
    }
}

function on_resize() {
    ctx.canvas.width = window.innerWidth - borderWidth;
    ctx.canvas.height = window.innerHeight - borderWidth;

    centerX = ctx.canvas.width / 2;
    centerY = ctx.canvas.height / 2;

    console.log("center: " + centerX + "," + centerY);
}

function Init() {
    // Setup resize
    window.onresize = on_resize;
    on_resize();

    loopTimer = new Timer();
    loopTimer.start(timerLoopTick, 50);
}

// Called every timer interval, once the Websocket connection is established.
function OnConnectedTimerTick() {
}

