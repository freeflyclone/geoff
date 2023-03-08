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

const numberOfZombies = 10;
const numberOfBases = 5;

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
			ctx.drawImage(this.sheetImage, 2 + (sx * (this.spriteWidth + 1)), 2 + (sy * (this.spriteHeight + 1)), this.spriteWidth - 2, this.spriteHeight - 2, x - (this.spriteWidth >> 1), y - (this.spriteHeight >> 1), this.spriteWidth, this.spriteHeight);
	}
}

class Location
{
	constructor(x, y)
	{
		this.xPos = x;
		this.yPos = y;
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
		if (this.occupied)
		{
			ctx.beginPath();
			ctx.arc(this.position.xPos, this.position.yPos, this.radius, 0, 2 * Math.PI);
			ctx.fillStyle = '#cf0000';
			ctx.fill();
		}

		ctx.beginPath();
		ctx.lineWidth = 2;
		ctx.arc(this.position.xPos, this.position.yPos, this.radius, 0, 2 * Math.PI);
		ctx.strokeStyle = '#0000ff';
		ctx.stroke();

		ctx.beginPath();
		ctx.lineWidth = (this.occupied) ? 6 : 2;
		ctx.arc(this.position.xPos, this.position.yPos, this.radius, 0, this.arcspan);
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
		// Go through all zombies, 
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

		// If not moving, check for new destination
		if (zombies[i].IsMoving == false)
		{
			if (Math.random() > 0.5)
				zombies[i].movement.destination.setLocation(player.position.xPos + ((Math.random() * 100) - 50), player.position.yPos + ((Math.random() * 100) - 50));
			else
				if (Math.random() > 0.9)
					zombies[i].movement.destination.setLocation(Math.random() * canvas.width, Math.random() * canvas.height);
		}
	}

	// check for new bases
	if (bases.length < numberOfBases)
	{
		if (Math.random() > 0.6)
		{
			let base = new Base(Math.random() * canvas.width, Math.random() * canvas.height, 50);
			bases.push(base);
		}
	}
}

function init()
{
	// Create sprite sheets
	spritesHuman = new SpriteSheet(40, 54, 6, 2, 'SSheet_Man.gif');
	spritesZombie = new SpriteSheet(40, 54, 6, 2, 'SSheet_Zombie.gif');

	// Setup resize
	window.onresize = on_resize;
	on_resize();

	// Create zombies and add to beings list
	for (let i = 0; i < numberOfZombies; i++)
	{
		let zombie = new Zombie(Math.random() * canvas.width, Math.random() * canvas.height);
		zombies.push(zombie);
		beings.push(zombie);
	}

	// Create 1 base
	let base = new Base(Math.random() * canvas.width, Math.random() * canvas.height, 50);
	bases.push(base);
	

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

	// Clear background
	ctx.clearRect(0, 0, canvas.width, canvas.height);

	// Render all bases
	for (let i = 0; i < bases.length; i++)
		bases[i].draw();

	// Draw all beings
	for (let i = 0; i < beings.length; i++)
		beings[i].draw();
}

function on_click(event)
{
	player.movement.destination.setLocation(event.clientX, event.clientY);
}