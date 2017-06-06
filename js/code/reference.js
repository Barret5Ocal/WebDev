
function ColorRect(topLeftX, topLeftY, boxWidth, boxHeight, fillColor) 
{
    CanvasContext.fillStyle = fillColor;
    CanvasContext.fillRect(topLeftX, topLeftY, boxWidth, boxHeight);
}

function ColorCircle(centerX, centerY, radius, fillColor) 
{
    CanvasContext.fillStyle = fillColor;
    CanvasContext.beginPath();
    CanvasContext.arc(centerX, centerY, 10, 0, Math.PI * 2, true);
    CanvasContext.fill();
}

function KeyPressed(evt)
{
    console.log("key pressed " + evt.keyCode);
}

function KeyReleased(evt)
{
    console.log("key up " + evt.keyCode);
}


var Pic = document.createElement("img");
var PicLoaded = false;
Pic.onload = function()
{
    PicLoaded = true; 
}
Pic.src = "../data/player1car.png";

var PicAngle;
function DrawBitmapWithRotation(Bitmap, atX, atY)
{
    CanvasContext.save();
    CanvasContext.translate(atX, atY);
    CanvasContext.rotate(PicAngle);
    CanvasContext.drawImage(Bitmap, -Bitmap.width/2, -Bitmap.height/2);
    CanvasContext.restore(); 
}
