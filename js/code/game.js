// TODO(barret): try to recreate long solo project in web x

/*
-create a grid <- DO THIS FIRST 
-create units on the grid
-do pathfinding 
-lots of stuff
*/


var Canvas, CanvasContext;

var MouseX;
var MouseY;

var v2 = 
    {
	x : 0,
	y : 0
    }; 

var unit = 
    {
	position : v2
    };

var Unit = unit; 
Unit.position.x = 1;
Unit.position.y = 1; 

function UpdateMousePos(evt) 
{
    var Rect = Canvas.getBoundingClientRect();
    var Root = document.documentElement;
    MouseX = evt.clientX - Rect.left - Root.scrollLeft;
    MouseY = evt.clientY - Rect.top - Root.scrollTop;
    //console.log(MouseX + " " + MouseY)
}

function Click(evt)
{

}

function ClearScreen()
{
    CanvasContext.fillStyle = 'black';
    CanvasContext.fillRect(0, 0, Canvas.width, Canvas.height);
}

const NodeRadius = 90; 
const GridRows = 6;
const GridColumns = 8;

function Lerp(a, b, t)
{
    return a + t * (b - 1);
}

// TODO(barret): figure out movement
var t = 0;
var end =  Unit.position.x + 1
var current = Unit.position.x;
function DrawUnit(Unit)
{
    var X = Unit.position.x;
    var Y = Unit.position.y; 

    if(current <= end)
    {
	var start = X;	
	X = Lerp(start, end, t);
	current = X; 	
	t += 0.01; 
    }
    else  
    {
	Unit.position.x = 2; 
    }

    CanvasContext.fillStyle = 'red';
    CanvasContext.beginPath();
    CanvasContext.arc(X * (Canvas.width / GridColumns) + (NodeRadius / 2) + 5,
		      Y * (Canvas.height / GridRows) + (NodeRadius / 2) + 5, 
		      NodeRadius / 2, 0, Math.PI * 2, true);
    CanvasContext.fill();
}

function DrawGrid()
{
    CanvasContext.fillStyle = 'blue';

    for(Row = 0; Row < GridRows; Row++)
    {
	for(Column = 0; Column < GridColumns; Column++)
	{
	    CanvasContext.fillRect(Column * (Canvas.width / GridColumns) + 5,
				   Row * (Canvas.height / GridRows) + 5,
				   NodeRadius, 
				   NodeRadius);

	}
    }
}

function UpdateAll()
{
    console.log();
    ClearScreen();
    DrawGrid();
    DrawUnit(Unit);

    CanvasContext.fillStyle = 'yellow';
    CanvasContext.fillText(MouseX + "," + MouseY, MouseX, MouseY - 15);
}

function Onload() 
{
    var v = 0;
    Canvas = document.getElementById('GameCanvas');
    CanvasContext = Canvas.getContext('2d');
    setInterval(UpdateAll, 1000 / 60);

    Canvas.addEventListener('mousemove', UpdateMousePos);
    Canvas.addEventListener('click', Click, false);

}
window.onload = Onload;
