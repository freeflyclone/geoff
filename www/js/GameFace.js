var loopTimer;
var webSock;

var appVersion = 3;
var sessionID;
var playerID = 0;
var serverAppVersion = 0;
var mapWidth = 0;
var mapHeight = 0;
var mapOffsetX = 0;
var mapOffsetY = 0;

var canvas = document.getElementById('gameCanvas');
var ctx = canvas.getContext("2d");
var borderWidth = 4;
var centerX;
var centerY;

// allow for multiple keys down simultaneously.
var keysPressed = {};

// map non-ASCII keys to single byte values for comms
var keyMap = {
    "ArrowLeft": 128,
    "ArrowUp" : 129,
    "ArrowRight": 130,
    "ArrowDown": 131,
};

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

class WebSock {
    constructor() {
        this.sock = new WebSocket(this.MakeWebSockUrl());
        this.isConnected = false;
        this.isRegistered = false;
        this.sock.binaryType = 'arraybuffer';

        this.sock.onopen = this.OnOpen;
        this.sock.onmessage = this.OnMessage;
        this.sock.onclose = this.OnClose;
        this.sock.onerror = this.OnError;
    }

    OnOpen(event) {
        if (typeof RegisterClient != 'undefined') {
            RegisterClient();
        }
    }

    OnMessage(event) {
        if (typeof HandleMessageEvent != 'undefined') {
            HandleMessageEvent(event.data);
        }
        else {
            console.log("OnMessage but HandleMessageEvent is not defined");
        }
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
}

function on_click(event) {
    var buffer = new ArrayBuffer(12);
    var view = new DataView(buffer);

    view.setUint8(0, (littleEndian == 0) ? 0xAA : 0xAB);
    view.setUint8(1, 0x12);
    view.setUint32(2, sessionID);
    view.setUint16(6, playerID);
    view.setUint16(8, mapOffsetX + event.clientX);
    view.setUint16(10, mapOffsetY + event.clientY);

    webSock.Send(buffer);
}

function ProcessKeyEvent(key, isDown) {
    var buff = new ArrayBuffer(4);
    var view = new Uint8Array(buff);

    view[0] = littleEndian;
    view[1] = 0x14;
    view[2] = isDown;
    view[3] = (typeof keyMap[key] === 'undefined') ? textEncoder.encode(key) : keyMap[key];

    webSock.Send(buff);
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
}

function Init() {
    // Setup resize
    window.onresize = on_resize;
    on_resize();

    if (typeof webSock === 'undefined') {
        webSock = new WebSock();
    }
}

// Called every timer interval, once the Websocket connection is established.
function OnConnectedTimerTick() {
}

function RegisterClient() {
    var buffer = new ArrayBuffer(4);
    var view = new DataView(buffer);

    view.setUint8(0, (littleEndian == 0) ? 0xAA : 0xAB);
    view.setUint8(1, 0x08);
    view.setUint16(2, appVersion);

    webSock.Send(buffer);
}

function HandleMessageEvent(data) {
    view = new DataView(data);
    if (view.getUint8(0) == 0xBB)
    {
        var serverCommand = view.getUint8(1);
        if (serverCommand == 0x09)
        {
            if (view.byteLength >= 12)
            {
                sessionID = view.getUint32(2);
                serverAppVersion = view.getUint16(6);
                mapWidth = view.getUint16(8);
                mapHeight = view.getUint16(10);
                this.isConnected = true;
            }
        }
        console.log("HandleMessageEvent, serverCommand: " + serverCommand);
    }
}

