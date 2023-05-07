var gameFace;

var loopTimer;
var webSock;

var appVersion = 3;
var serverAppVersion = 0;
var mapWidth = 0;
var mapHeight = 0;

var canv = document.getElementById('gameCanvas');
var ctx = canv.getContext("2d");
var borderWidth = 4;
var sessionID;
var tickCount;

// allow for multiple keys down simultaneously.
var keysPressed = {};

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

function on_resize() {
    ctx.canvas.width = window.innerWidth - borderWidth;
    ctx.canvas.height = window.innerHeight - borderWidth;

    var buffer = new ArrayBuffer(12);
    var view = new DataView(buffer);

    view.setUint8(0, (littleEndian == 0) ? 0xAA : 0xAB);
    view.setUint8(1, 0x06);
    view.setUint32(2, sessionID);
    view.setUint16(6, ctx.canvas.width);
    view.setUint16(8, ctx.canvas.height);

    if (typeof webSock != 'undefined')
        webSock.Send(buffer);
}

function ProcessKeyEvent(keyCode, isDown) {
    var buff = new ArrayBuffer(4);
    var view = new Uint8Array(buff);

    view[0] = littleEndian;
    view[1] = 0x04;
    view[2] = isDown;
    view[3] = keyCode;

    webSock.Send(buff);
}

function on_click(event) {
    var buffer = new ArrayBuffer(12);
    var view = new DataView(buffer);

    view.setUint8(0, (littleEndian == 0) ? 0xAA : 0xAB);
    view.setUint8(1, 0x02);
    view.setUint32(2, sessionID);
    view.setUint16(6, event.clientX);
    view.setUint16(8, event.clientY);

    webSock.Send(buffer);
}

function on_keydown(event) {
    keysPressed[event.keyCode] = true;
    if (!event.repeat)
        ProcessKeyEvent(event.keyCode, true);
}

function on_keyup(event) {
    keysPressed[event.keyCode] = false;
    ProcessKeyEvent(event.keyCode, false);
}

function RegisterClient() {
    console.log("RegisterClient()");
    var buffer = new ArrayBuffer(8);
    var view = new DataView(buffer);

    view.setUint8(0, (littleEndian == 0) ? 0xAA : 0xAB);
    view.setUint8(1, 0x00);
    view.setUint16(2, appVersion);
    view.setUint16(4, canv.width);
    view.setUint16(6, canv.height);

    webSock.Send(buffer);
}

function HandleMessageEvent(data) {
    var view = new DataView(data);
    if (view.getUint8(0) == 0xBB) {
        var serverCommand = view.getUint8(1);
        if (serverCommand == 0x01) {
            if (view.byteLength >= 12) {
                sessionID = view.getUint32(2);
                serverAppVersion = view.getUint16(6);
                mapWidth = view.getUint16(8);
                mapHeight = view.getUint16(10);
                this.isConnected = true;
            }
        }
        else if (serverCommand == 0x05) {
            sessionID = view.getUint32(2);
            isDown = view.getUint8(6);
            key = view.getUint8(7);
            //console.log("SessionID: " + sessionID + ", Key: " + key + ", isDown: " + isDown);
        }
        else if (serverCommand == 0x07) {
            sessionID = view.getUint32(2);
            tickCount = view.getUint32(6);

            ship.Move(view.getInt16(10), view.getInt16(12), view.getInt16(14) / 4096.0);

            if (view.byteLength == 16) {
                update();
                return;
            }

            numBullets = view.getUint16(16);

            if (numBullets > 0) {
                if (typeof bullets == 'undefined') {
                    console.log("ship.bullets is undefined");
                    return;
                }

                bullets = [];

                for (i = 0; i < numBullets; i++) {
                    x = view.getUint16(18 + i * 4);
                    y = view.getUint16(20 + i * 4);
                    bullets.push({ x, y });
                }

            }

            // some JavaScript file needs to define a single update() function
            // which is invoked after the sever has updated all game state
            if (typeof update != undefined) {
                update();
            }
        }
        else
            console.log("HandleMessageEvent, serverCommand: " + serverCommand);
    }
}

function GameFaceInit() {
    // Setup resize
    window.onresize = on_resize;
    on_resize();

    if (typeof webSock === 'undefined') {
        webSock = new WebSock();
    }

    // DOM object events we care about.
    document.addEventListener("click", on_click);
    document.addEventListener("keydown", on_keydown);
    document.addEventListener("keyup", on_keyup);
}
