document.addEventListener("click", on_click);
canvas = document.getElementById('gameCanvas');
ctx = canvas.getContext("2d");
borderWidth = 4;

var bases = [];
var beings = [];
var zombies = [];
var player;

var spritesHuman;
var spritesZombie;

let initialised = false;

const numberOfZombies = 100;
const numberOfBases = 100;

const mapWidth = 8192;
const mapHeight = 4096;

var mapOffsetX = 0;
var mapOffsetY = 0;

const radarDisplayWidth = 200;
var radarSize; 

class SpriteSheet
{
	constructor(sw, sh, nx, ny, source)
	{
		this.Initialised = false;
		this.spriteWidth = sw;
		this.spriteHeight = sh;
		this.sheetSizeX = nx;
		this.sheetSizeY = ny;
		this.sheetImage = new Image();
		this.sheetImage.onload = this.event_onload(this);
		this.sheetImage.src = source;
	}

	event_onload(sourceObject)
	{
		sourceObject.Initialised = true;
	}

	draw(x, y, sx, sy)
	{
		if (this.Initialised == true)
			ctx.drawImage(this.sheetImage, 2 + (sx * (this.spriteWidth + 1)), 2 + (sy * (this.spriteHeight + 1)), this.spriteWidth - 2, this.spriteHeight - 2, (x - mapOffsetX) - (this.spriteWidth >> 1), (y - mapOffsetY) - (this.spriteHeight >> 1), this.spriteWidth, this.spriteHeight);
	}
}

class Size
{
	constructor(w, h)
	{
		this.width = w;
		this.height = h;
    }
}

class Location
{
	constructor(x, y)
	{
		this.xPos = x;
		this.yPos = y;
	}

	getScaledLocation(s)
	{
		var l = new Location((this.xPos * s.width) / mapWidth, (this.yPos * s.height) / mapHeight);
		return l;
	}

	setLocation(x, y)
	{
		this.xPos = x;
		this.yPos = y;
	}

	moveBy(x, y)
	{
		this.xPos += x;
		this.yPos += y;
	}

	snapToPosition(x, y, distance)
	{
		if (Math.abs(this.xPos - x) < distance)
			this.xPos = x;
		if (Math.abs(this.yPos - y) < distance)
			this.yPos = y;
	}

	distanceFrom(otherLocation)
	{
		var xD = Math.pow(this.xPos - otherLocation.xPos, 2);
		var yD = Math.pow(this.yPos - otherLocation.yPos, 2);
		return Math.sqrt(xD + yD);
    }
}

class MovementInstance
{
	constructor()
	{
		this.xAdj = 0;
		this.yAdj = 0;
		this.hasMoved = false;
	}

	applyMovement(position)
	{
		this.hasMoved = false;
		if (this.xAdj != 0 || this.yAdj != 0)
		{
			position.moveBy(this.xAdj, this.yAdj);
			this.hasMoved = true;
		}
    }
}

class LocationMovement
{
	constructor(p, x, y)
	{
		this.position = p;
		this.destination = new Location(x, y);
	}

	updateMovement(movementSpeed)
	{
		var adjustment = new MovementInstance();
		
		if (this.position.xPos < this.destination.xPos)
			adjustment.xAdj = movementSpeed;
		if (this.position.xPos > this.destination.xPos)
			adjustment.xAdj = -movementSpeed;

		if (this.position.yPos < this.destination.yPos)
			adjustment.yAdj = movementSpeed;
		if (this.position.yPos > this.destination.yPos)
			adjustment.yAdj = -movementSpeed;

		adjustment.applyMovement(this.position);
		return adjustment;
    }
}

class Being
{
	constructor(x, y, speed, spriteSheet)
	{
		this.position = new Location(x, y);
		this.movement = new LocationMovement(this.position, x, y);

		this.isLeft = 0;
		this.animframe = 0;

		this.IsMoving = false;
		this.MovementSpeed = speed;
		this.spriteSheet = spriteSheet;
	}

	draw()
	{
		this.spriteSheet.draw(this.position.xPos, this.position.yPos, this.animframe, this.isLeft);
	}

	updateMovement()
	{

		// If within snapLock distance, snap to position
		this.position.snapToPosition(this.movement.destination.xPos, this.movement.destination.yPos, this.MovementSpeed);

		// Move if required and set isMoving
		var adjustment = this.movement.updateMovement(this.MovementSpeed);
		this.IsMoving = adjustment.hasMoved;

		// Update animation frame (if moved)
		if (this.IsMoving)
		{
			if (adjustment.xAdj > 0)
				this.isLeft = 0;
			if (adjustment.xAdj < 0)
				this.isLeft = 1;

			if (this.isLeft == 0)
				this.animframe = (this.animframe + 1) % 6;
			else {
				this.animframe -= 1;
				if (this.animframe < 0)
					this.animframe = 5;
			}
		}
		return this.IsMoving;
	}
}

class Human extends Being
{

	constructor(x, y)
	{
		super(x, y, 3, spritesHuman);
		var isInBase = false;
	}
}

class Zombie extends Being
{

	constructor(x, y)
	{
		super(x, y, 2, spritesZombie);
	}
}

class Base
{
	constructor(x, y, r)
	{
		this.position = new Location(x, y);
		this.radius = r;
		this.arcspan = 2 * Math.PI;
		this.occupied = false;
	}

	draw()
	{
		var x = this.position.xPos - mapOffsetX;
		var y = this.position.yPos - mapOffsetY;

		if (this.occupied)
		{
			ctx.beginPath();
			ctx.arc(x, y, this.radius, 0, 2 * Math.PI);
			ctx.fillStyle = '#cf0000';
			ctx.fill();
		}

		ctx.beginPath();
		ctx.lineWidth = 2;
		ctx.arc(x, y, this.radius, 0, 2 * Math.PI);
		ctx.strokeStyle = '#0000ff';
		ctx.stroke();

		ctx.beginPath();
		ctx.lineWidth = (this.occupied) ? 6 : 2;
		ctx.arc(x, y, this.radius, 0, this.arcspan);
		ctx.strokeStyle = '#ffffff';
		ctx.stroke();
	}

	decay()
	{
		var arcspan = this.arcspan - 0.005;
		if (arcspan < 0)
			arcspan = 0;
		this.arcspan = arcspan;
		return (arcspan <= 0) ? false : true;
	}

	updateOccupiedState(playerToCheck)
	{
		var distFromPlayer = this.position.distanceFrom(playerToCheck.position);
		this.occupied = (distFromPlayer < this.radius) ? true : false;
    }

}

function timed_movementupdate()
{
	// Update base decay
	updateBasesDecay();

	// If player is in base AND zombie is moving, check it's not within < 50, if so, stop
	if (player.isInBase)
	{
		// Go through all zombies 
		for (let i = 0; i < zombies.length; i++)
		{
			// Within distance? If so, stop it
			if (player.position.distanceFrom(zombies[i].position) < 100)
			{
				zombies[i].movement.destination.xPos = zombies[i].position.xPos;
				zombies[i].movement.destination.yPos = zombies[i].position.yPos;
			}
		}
	}

	// Update movement of all beings
	var BeingHasMoved = false;
	for (let i = 0; i < beings.length; i++)
		BeingHasMoved |= beings[i].updateMovement();

	// Have beings moved about?
	if (BeingHasMoved == true)
	{

		// Has player moved?
		if (player.IsMoving == true)
			updateBasesOccupiedState();
	}

	// Redraw
	draw();
}

function updateBasesOccupiedState()
{
	player.isInBase = false;
	for (let i = 0; i < bases.length; i++)
	{
		bases[i].updateOccupiedState(player);
		player.isInBase |= bases[i].occupied;
	}
}

function updateBasesDecay()
{
	var baseRemoved = false;
	for (let i = bases.length - 1; i >= 0; i--)
	{
		bases[i].decay();
		if (bases[i].arcspan <= 0)
		{
			bases.splice(i, 1);
			baseRemoved = true;
		}
	}
	if (baseRemoved)
		updateBasesOccupiedState();
}

function timed_zombiedestinationandcreatebaseupdate()
{
	// Go through all zombies, 
	for (let i = 0; i < zombies.length; i++)
	{

		// If not moving, (or random) check for new destination
		if (zombies[i].IsMoving == false || (Math.random() > 0.9))
		{
			if (Math.random() > 0.5)
				zombies[i].movement.destination.setLocation(player.position.xPos + ((Math.random() * 100) - 50), player.position.yPos + ((Math.random() * 100) - 50));
			else
				if (Math.random() > 0.9)
					zombies[i].movement.destination.setLocation(Math.random() * mapWidth, Math.random() * mapHeight);
		}
	}

	// check for new bases
	if (bases.length < numberOfBases)
	{
		if (Math.random() > 0.6)
		{
			let base = new Base(Math.random() * mapWidth, Math.random() * mapHeight, 50);
			bases.push(base);
		}
	}
}

function init()
{
	// Set rader size
	radarSize = new Size(radarDisplayWidth, (radarDisplayWidth * mapHeight) / mapWidth);

	// Create sprite sheets
	spritesHuman = new SpriteSheet(40, 54, 6, 2, 'images/SSheet_Man.gif');
	spritesZombie = new SpriteSheet(40, 54, 6, 2, 'images/SSheet_Zombie.gif');

	// Setup resize
	window.onresize = on_resize;
	on_resize();

	// Create zombies and add to beings list
	for (let i = 0; i < numberOfZombies; i++)
	{
		let zombie = new Zombie(Math.random() * mapWidth, Math.random() * mapHeight);
		zombies.push(zombie);
		beings.push(zombie);
	}

	// Create half the max no. of bases
	for (let i = 0; i < numberOfBases >> 1; i++)
	{
		let base = new Base(Math.random() * mapWidth, Math.random() * mapHeight, 50);
		bases.push(base);
	}
	
	// Create player and add to beings list
	this.player = new Human(bases[0].position.xPos, bases[0].position.yPos);
	beings.push(player);
	
	// Set update timers
	TimerIDmovementUpdate = setInterval(timed_movementupdate, 20);
	TimerIDzombieDestinationUpdate = setInterval(timed_zombiedestinationandcreatebaseupdate, 1000);

	// Pre state updates
	updateBasesOccupiedState();

	// Initialised OK
	initialised = true;
	viewToCenter(player.position.xPos, player.position.yPos);
	draw();
}

function on_resize()
{
	ctx.canvas.width = window.innerWidth - borderWidth;
	ctx.canvas.height = window.innerHeight - borderWidth;
	draw();
}

function draw()
{
	// Check for initialised
	if (!initialised)
		return;

	// Adjust view offset
	var offsetByX = 0;
	var offsetByY = 0;

	var offBy = player.position.xPos - mapOffsetX;
	var mapScrollBorder = canvas.width / 3;

	if (offBy < mapScrollBorder)
		offsetByX = offBy - mapScrollBorder;
	if (offBy > (canvas.width - mapScrollBorder))
		offsetByX = offBy - (canvas.width - mapScrollBorder);

	offBy = player.position.yPos - mapOffsetY;
	mapScrollBorder = canvas.height / 3;

	if (offBy < mapScrollBorder)
		offsetByY = offBy - mapScrollBorder;
	if (offBy > (canvas.height - mapScrollBorder))
		offsetByY = offBy - (canvas.height - mapScrollBorder);

	if (offsetByX != 0 || offsetByY != 0)
		viewMoveOffset(offsetByX, offsetByY);

	// Clear background
	ctx.clearRect(0, 0, canvas.width, canvas.height);

	// Draw map extent border
	ctx.beginPath();
	ctx.lineWidth = 16;
	ctx.rect(-mapOffsetX, -mapOffsetY, mapWidth, mapHeight);
	ctx.strokeStyle = '#0000ff';
	ctx.stroke();

	// Render all bases
	for (let i = 0; i < bases.length; i++)
		bases[i].draw();

	// Draw all beings
	for (let i = 0; i < beings.length; i++)
		beings[i].draw();

	// draw radar
	var radarX = (canvas.width - 20) - radarSize.width;
	var radarY = 20;

	ctx.clearRect(radarX, radarY, radarSize.width, radarSize.height);

	ctx.beginPath();
	ctx.fillStyle = '#ff0000';
	for (let i = 0; i < bases.length; i++) {
		var l = bases[i].position.getScaledLocation(radarSize);
		ctx.fillRect(radarX + l.xPos - 1, radarY + l.yPos - 1, 3, 3);
	}
	ctx.stroke();

	ctx.beginPath();
	ctx.fillStyle = '#00ff00';
	for (let i = 0; i < zombies.length; i++)
	{
		var l = zombies[i].position.getScaledLocation(radarSize);
		ctx.fillRect(radarX + l.xPos, radarY + l.yPos, 1, 1);
	}
	ctx.stroke();

	ctx.beginPath();
	ctx.lineWidth = 1;
	var l = player.position.getScaledLocation(radarSize);
	ctx.fillStyle = '#ffffff';
	ctx.fillRect(radarX + l.xPos - 2, radarY + l.yPos - 2, 5, 5);
	ctx.stroke();

	ctx.beginPath();
	ctx.lineWidth = 1;
	ctx.rect (radarX, radarY, radarSize.width, radarSize.height);
	ctx.strokeStyle = '#0000ff';
	ctx.stroke();

}

function on_click(event)
{
	player.movement.destination.setLocation(mapOffsetX + event.clientX, mapOffsetY + event.clientY);
}

function viewToCenter(x, y)
{
	viewSetOffset(x - (canvas.width >> 1), y - (canvas.height >> 1));
}

function viewMoveOffset(x, y)
{
	viewSetOffset(mapOffsetX + x, mapOffsetY + y);
}

function viewSetOffset(x, y)
{
	if (x < 0)
		x = 0;
	if (x > (mapWidth - canvas.width))
		x = mapWidth - canvas.width;

	if (y < 0)
		y = 0;
	if (y > (mapHeight - canvas.height))
		y = mapHeight - canvas.height;

	mapOffsetX = x;
	mapOffsetY = y;
}
