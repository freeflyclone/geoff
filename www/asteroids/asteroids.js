const FPS = 60;                     // frames per second
const FRICTION = 0.7;               // friction coefficient of space (0 = no friction, 1 = lots of friction)
const GAME_LIVES = 3;               // starting number of lives
const LASER_DIST = 0.6;             // max distance laser can travel as fraction of screen width
const LASER_EXPLODE_DUR = 0.1;      // duration of the lasers' explosion in seconds
const LASER_MAX = 10;               // maximum number of lasers on screen at once
const LASER_SPD = 500;              // speed of lasers in pixels per second
const ROID_JAG = 0.15;              // jaggedness of the asteroids (0 = none, 1 = lots)
const ROID_NUM = 1;                 // starting number of asteroids
const ROID_PTS_LGE = 20;            // points scored for large asteroid
const ROID_PTS_MED = 50;            // points scored for medium asteroid
const ROID_PTS_SML = 100;           // points scored for small asteroid
const ROID_SIZE = 100;              // starting size of asteroids in pixels
const ROID_SPD = 50;                // max starting speed of asteroids in pixels per second
const ROID_VERT = 20;               // average number of vertices on each asteroid
const SAVE_KEY_SCORE = "highscore"; // key of high score local storage
const SHIP_BLINK_DUR = 0.2;         // duration in seconds of a single blink during ship's invisibility
const SHIP_EXPLODE_DUR = 0.5;       // duration of the ship's explosion in seconds
const SHIP_INV_DUR = 3;             // duration of the ship's invisibility in seconds
const SHIP_SIZE = 20;               // ship height in pixels
const SHIP_THRUST = 10;             // acceleration of the ship in pixels per second per second
const SHIP_TURN_SPEED = 360;        // turn speed in degrees per second
const SHOW_BOUNDING = false;        // show or hide collision bounding
const TEXT_FADE_TIME = 2.5;         // text fade time in seconds
const TEXT_SIZE = 40;               // text font height in pixels
const FP_4_12 = 4096.0;              // convert to fixed point 4.12, specifically for angle scaling

const GamePhase = {
    InLobby: 0,
    Playing: 1,
    GameOver: 2,
}

var level, lives, roids, phase, score, scoreHigh, ship, text, textAlpha;

var universeRocks = [];
var universeShips = [];
var universeBullets = [];
var universeStars = [];

var universeWidth;
var universeHeight;
var contextWidth;
var contextHeight;
var contextOffsetX;
var contextOffsetY;

var shipsFlags = 0;

var currentPhase = GamePhase.InLobby;
scoreHigh = 0;

function newShip() {
    return {
        x: canv.width / 2,
        y: canv.height / 2,
        a: 90 / 180 * Math.PI, // convert to radians
        r: SHIP_SIZE / 2,
        blinkNum: Math.ceil(SHIP_INV_DUR / SHIP_BLINK_DUR),
        blinkTime: Math.ceil(SHIP_BLINK_DUR * FPS),
        canShoot: true,
        dead: false,
        explodeTime: 0,
        lasers: [],
        bullets: [],
        rot: 0,
        thrusting: false,
        exploding: false,
        visible: false,
        thrust: {
            x: 0,
            y: 0
        },

        Move: function(x, y, angle)
        {
            if (this.explodeTime == 0) {
                this.x = x;
                this.y = y;
                this.a = angle;
            }
        }
    }
}

function drawSpace() {
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, canv.width, canv.height);

    var numStars = universeStars.length;

    for (i = 0; i < numStars; i++) {
        drawBullet(universeStars[i].x - contextOffsetX, universeStars[i].y - contextOffsetY, 3, "white");
    }
}

function drawShip(x, y, a, color = "white") {
    ctx.strokeStyle = color;
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo( // nose of the ship
        x + 4 / 3 * ship.r * Math.cos(a),
        y - 4 / 3 * ship.r * Math.sin(a)
    );
    ctx.lineTo( // rear left
        x - ship.r * (2 / 3 * Math.cos(a) + Math.sin(a)),
        y + ship.r * (2 / 3 * Math.sin(a) - Math.cos(a))
    );
    ctx.lineTo( // rear center
        x,
        y
    );
    ctx.lineTo( // rear right
        x - ship.r * (2 / 3 * Math.cos(a) - Math.sin(a)),
        y + ship.r * (2 / 3 * Math.sin(a) + Math.cos(a))
    );
    ctx.closePath();
    ctx.stroke();
}

function drawShipFully() {
    var blinkOn = ship.visible; //ship.blinkNum % 2 == 0;
    var exploding = ship.exploding; //ship.explodeTime > 0;

    var posX = ship.x - contextOffsetX;
    var posY = ship.y - contextOffsetY;

    // draw the triangular ship
    if (!exploding) {
        if (blinkOn && !ship.dead) {
            drawShip(posX, posY, ship.a)
        }

        if (ship.thrusting && !ship.dead) {
            // draw the thruster
            ctx.strokeStyle = "white";
            ctx.lineWidth = 1;
            ctx.beginPath();
            ctx.moveTo(
                posX, posY
            );
            ctx.lineTo( // rear left
                posX - ship.r * (2 / 3 * Math.cos(ship.a) + 0.5 * Math.sin(ship.a)),
                posY + ship.r * (2 / 3 * Math.sin(ship.a) - 0.5 * Math.cos(ship.a))
            );
            ctx.lineTo( // rear centre (behind the ship)
                posX - ship.r * 5 / 3 * Math.cos(ship.a),
                posY + ship.r * 5 / 3 * Math.sin(ship.a)
            );
            ctx.lineTo( // rear right
                posX - ship.r * (2 / 3 * Math.cos(ship.a) - 0.5 * Math.sin(ship.a)),
                posY + ship.r * (2 / 3 * Math.sin(ship.a) + 0.5 * Math.cos(ship.a))
            );
            ctx.closePath();
            ctx.stroke();
        }
    } else if (!ship.dead) {
        // draw the explosion (concentric circles of different colours)
        ctx.fillStyle = "darkred";
        ctx.beginPath();
        ctx.arc(posX, posY, ship.r * 1.7, 0, Math.PI * 2, false);
        ctx.fill();
        ctx.fillStyle = "red";
        ctx.beginPath();
        ctx.arc(posX, posY, ship.r * 1.4, 0, Math.PI * 2, false);
        ctx.fill();
        ctx.fillStyle = "orange";
        ctx.beginPath();
        ctx.arc(posX, posY, ship.r * 1.1, 0, Math.PI * 2, false);
        ctx.fill();
        ctx.fillStyle = "yellow";
        ctx.beginPath();
        ctx.arc(posX, posY, ship.r * 0.8, 0, Math.PI * 2, false);
        ctx.fill();
        ctx.fillStyle = "white";
        ctx.beginPath();
        ctx.arc(posX, posY, ship.r * 0.5, 0, Math.PI * 2, false);
        ctx.fill();
    }

    if (SHOW_BOUNDING) {
        ctx.strokeStyle = "lime";
        ctx.beginPath();
        ctx.arc(posX, posY, ship.r, 0, Math.PI * 2, false);
        ctx.stroke();
    }
}

function drawBullet(x, y, radius = 2, color = "cyan") {
    ctx.fillStyle = color;
    ctx.beginPath();
    ctx.arc(x, y, radius, 0, Math.PI * 2, false);
    ctx.fill();
}

function drawRock(x, y, radius = 2, color = "cyan") {
    ctx.strokeStyle = color;
    ctx.beginPath();
    ctx.arc(x, y, radius, 0, Math.PI * 2, false);
    ctx.stroke();
}

function drawBullets() {
    var numberOfBullets = Object.keys(ship.bullets).length;
    if (typeof ship.bullets == 'undefined') {
        console.log("ship.bullets undefined");
        return;
    }

    for (i = 0; i < numberOfBullets; i++) {
        drawBullet(ship.bullets[i].x, ship.bullets[i].y);
    }
}

function drawOtherShips() {
    var numberOfOtherShips = Object.keys(universeShips).length;
    if (typeof universeShips == 'undefined') {
        console.log("otherShips undefined");
        return;
    }

    for (i = 0; i < numberOfOtherShips; i++) {
        drawShip(universeShips[i].x - contextOffsetX, universeShips[i].y - contextOffsetY, universeShips[i].angle, "yellow");
    }
}

function drawOtherBullets() {
    var numberOfOtherBullets = Object.keys(universeBullets).length;
    if (typeof universeBullets == 'undefined') {
        console.log("otherBullets undefined");
        return;
    }

    if (numberOfOtherBullets == 0)
        return;

    for (i = 0; i < numberOfOtherBullets; i++) {
        drawBullet(universeBullets[i].x - contextOffsetX, universeBullets[i].y - contextOffsetY, universeBullets[i].radius, universeBullets[i].color);
    }
}

function drawRocks() {
    var numberOfRocks = Object.keys(universeRocks).length;
    if (typeof universeRocks == 'undefined') {
        console.log("universeRocks undefined");
        return;
    }

    if (numberOfRocks == 0)
        return;

    for (i = 0; i < numberOfRocks; i++) {
        drawRock(universeRocks[i].x - contextOffsetX, universeRocks[i].y - contextOffsetY, universeRocks[i].r, "white");
    }
}

function drawRadar() {
    var radarScaler = 32;

    // size of radar window
    var radarWidth = universeWidth / radarScaler;
    var radarHeight = universeHeight / radarScaler;

    // size of player window, in radar window units
    var radarWW = contextWidth / radarScaler;
    var radarWH = contextHeight / radarScaler;

    var radarOX = contextOffsetX / radarScaler;
    var radarOY = contextOffsetY / radarScaler;

    // screen coordinates of radar top/left
    var radarStartX = contextWidth - radarWidth;
    var radarStartY = 0;

    // screen coordinates of radar center
    var radarCenterX = radarStartX + radarWidth / 2;
    var radarCenterY = radarStartY + radarHeight / 2;

    ctx.strokeStyle = "blue";
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.rect(contextWidth - radarWidth, 0, radarWidth, radarHeight);
    ctx.closePath();
    ctx.stroke();

    ctx.strokeStyle = "cyan";
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.rect(radarStartX + radarOX, radarStartY + radarOY, radarWW, radarWH);
    ctx.closePath();
    ctx.stroke();

    // draw the universeRocks in the radarView
    for (i = 0; i < universeRocks.length; i++) {
        var rockX = universeRocks[i].x;
        var rockY = universeRocks[i].y;

        rockX /= radarScaler;
        rockY /= radarScaler;

        rockX += radarStartX;
        rockY += radarStartY;

        drawBullet(rockX, rockY, 2, "green");
    }

    // draw player's ship
    var shipX = ship.x;
    var shipY = ship.y;

    shipX /= radarScaler;
    shipY /= radarScaler;

    shipX += radarStartX;
    shipY += radarStartY;

    drawBullet(shipX, shipY, 2, "white");

    // draw other player's ships
    for (i = 0; i < universeShips.length; i++) {
        otherShipX = universeShips[i].x;
        otherShipY = universeShips[i].y;

        otherShipX /= radarScaler;
        otherShipY /= radarScaler;

        otherShipX += radarStartX;
        otherShipY += radarStartY;

        drawBullet(otherShipX, otherShipY, 2, "yellow");
    }
}

function drawGameInfo() {
    var exploding = ship.exploding;

    // draw the game text
    if (textAlpha >= 0) {
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        ctx.fillStyle = "rgba(255, 255, 255, " + textAlpha + ")";
        ctx.font = "small-caps " + TEXT_SIZE + "px dejavu sans mono";
        ctx.fillText(text, canv.width / 2, canv.height * 0.75);
        textAlpha -= (1.0 / TEXT_FADE_TIME / FPS);
    }

    // draw the lives
    var lifeColour;
    for (var i = 0; i < lives; i++) {
        lifeColour = exploding && i == lives - 1 ? "red" : "white";
        drawShip(SHIP_SIZE + i * SHIP_SIZE * 1.2, SHIP_SIZE, 0.5 * Math.PI, lifeColour);
    }

    // draw the score
    ctx.textAlign = "right";
    ctx.textBaseline = "middle";
    ctx.fillStyle = "white";
    ctx.font = TEXT_SIZE + "px arial";
    ctx.fillText(score, canv.width - SHIP_SIZE / 2, SHIP_SIZE);

    // draw the high score
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillStyle = "white";
    ctx.font = (TEXT_SIZE * 0.75) + "px arial";
    ctx.fillText("High Score: " + scoreHigh, canv.width / 2, SHIP_SIZE);
}


function update() {
    switch (phase) {
        case GamePhase.InLobby:
            console.log("InLobby");
            return;
            break;

        case GamePhase.Playing:
            if (currentPhase == GamePhase.InLobby) {
                currentPhase = phase;
                console.log("Transition from InLobby to Playing");
                var lobby_div = document.getElementById("lobby");
                lobby_div.className = "zoom_out";
            }
            else if (currentPhase == GamePhase.GameOver) {
                currentPhase = phase;
                console.log("Transition from GameOver to Playing");
                document.getElementById("lounge").className = "zoom_out";
                return;
            }
            console.log("Playing");
            break;

        case GamePhase.GameOver:
            console.log("GameOver");
            if (currentPhase == GamePhase.Playing) {
                console.log("Transition from Playing to GameOver");
                document.getElementById("lobby").style = "visiblility: hidden";
                document.getElementById("lounge").style = "visibility = visible";
                document.getElementById("lounge").className = "zoom_start";
                currentPhase = phase;
            }
            break;
    }

    drawSpace();
    drawShipFully();   
    drawBullets();
    drawOtherShips();
    drawOtherBullets();
    drawRocks();
    drawRadar();

    drawGameInfo();
}

// this game now depends on a steady timer tick Websock message 
// with complete game state for this client.
function AsteroidsInit() {
    ship = newShip();
}

function OnSessionRegistered(data) {
    view = new DataView(data);
    console.log("OnSessionRegistered");
    var offset = 2;

    if (view.byteLength >= 8) {
        sessionID = view.getUint32(offset);
        offset += 4;

        serverAppVersion = view.getUint16(offset);
        offset += 2;
        this.isConnected = true;
    }

    if (offset >= DataView.byteLength)
        return;

    var numStars = view.getUint16(offset);

    if (offset >= DataView.byteLength)
        return;

    for (i = 0; i < numStars; i++) {
        x = view.getUint16(offset);
        offset += 2;

        y = view.getUint16(offset);
        offset += 2;

        universeStars.push({ x, y });
    }

    console.log("numStars pushed: " + universeStars.length);
}

function OnKeyMessage(data) {
    view = new DataView(data);
    console.log("KeyMessage");

    sessionID = view.getUint32(2);
    isDown = view.getUint8(6);
    key = view.getUint8(7);
    console.log("SessionID: " + sessionID + ", Key: " + key + ", isDown: " + isDown);
}

function OnPlayerTickMessage(data) {
    view = new DataView(data);

    offset = 2;
    sessionID = view.getUint32(offset);
    offset += 4;

    tickCount = view.getUint32(offset);
    offset += 4;

    contextWidth = view.getUint16(offset);
    offset += 2;

    contextHeight = view.getUint16(offset);
    offset += 2;

    contextOffsetX = view.getUint16(offset);
    offset += 2;

    contextOffsetY = view.getUint16(offset);
    offset += 2;

    score = view.getUint32(offset);
    offset += 4;

    phase = view.getUint16(offset);
    offset += 2;

    shipX = view.getUint16(offset);
    offset += 2;

    shipY = view.getUint16(offset);
    offset += 2;

    shipAngle = view.getUint16(offset) / FP_4_12;
    offset += 2;

    ship.Move(shipX, shipY, shipAngle);

    if (offset == view.byteLength) {
        ship.bullets = [];
        update();
        return;
    }

    shipsFlags = view.getUint8(offset);
    offset += 1;

    ship.thrusting = (shipsFlags & 0x0001) ? true : false;
    ship.exploding = (shipsFlags & 0x0002) ? true : false;
    ship.visible = (shipsFlags & 0x0004) ? true : false;
    ship.dead = (shipsFlags & 0x0008) ? true : false;

    lives = view.getUint8(offset);
    offset += 1;

    numBullets = view.getUint16(offset);
    offset += 2;

    if (numBullets > 0) {
        if (typeof ship.bullets == 'undefined') {
            console.log("ship.bullets is undefined");
            return;
        }

        ship.bullets = [];

        for (i = 0; i < numBullets; i++) {
            x = view.getUint16(offset) - contextOffsetX;
            offset += 2;

            y = view.getUint16(offset) - contextOffsetY;
            offset += 2;

            ship.bullets.push({ x, y });
        }
    }
    else {
        ship.bullets = [];
    }

    if (typeof update != undefined) {
        update();
    }
}

function OnUniverseTickMessage(data) {
    if (data.byteLength < 14)
        return;

    view = new DataView(data);

    universeRocks = [];
    universeShips = [];
    universeBullets = [];

    // skip 0xBB,UniverseTickMessage bytes
    offset = 2;

    sessionID = view.getUint32(offset);
    offset += 4;

    tickCount = view.getUint32(offset);
    offset += 4;

    universeWidth = view.getUint16(offset);
    offset += 2;

    universeHeight = view.getUint16(offset);
    offset += 2;

    // there are no rocks, ships, or bullets
    if (offset == data.byteLength) {
        return;
    }

    numRocks = view.getUint16(offset);
    offset += 2;

    if (numRocks) {
        for (i = 0; i < numRocks; i++) {
            x = view.getUint16(offset);
            offset += 2;

            y = view.getUint16(offset);
            offset += 2;

            r = view.getUint16(offset);
            offset += 2;

            universeRocks.push({ x, y, r });
        }
    }

    // there are no ships or bullets;
    if (offset == data.byteLength) {
        return;
    }

    numShips = view.getUint16(offset);
    offset += 2;

    // if there are no universeShips, there can be no universeBullets
    if (numShips == 0) {
        return;
    }

    for (i = 0; i < numShips; i++) {
        x = view.getUint16(offset);
        offset += 2;

        y = view.getUint16(offset);
        offset += 2;

        angle = view.getUint16(offset) / FP_4_12;
        offset += 2;

        universeShips.push({ x, y, angle });
    }

    if (offset == view.byteLength) {
        return;
    }

    numBullets = view.getUint16(offset);
    offset += 2;

    if (numBullets == 0) {
        return;
    }

    for (i = 0; i < numBullets; i++) {
        x = view.getInt16(offset);
        offset += 2;

        y = view.getInt16(offset);
        offset += 2;

        radius = 2;
        color = "red";
        universeBullets.push({ x, y, radius, color });
    }
}
