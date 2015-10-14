
window.onresize=function(){    
    var canvas = init();
    draw(canvas);
}

window.onload = function(){
    var canvas = init();
    draw(canvas);
};

function init(){
    canvas = document.getElementById("myCanvas");
    canvas.width = document.body.clientWidth; //document.width is obsolete
    canvas.height = document.body.clientHeight; //document.height is obsolete
    canvasW = canvas.width;
    canvasH = canvas.height;
    return canvas;
}


function draw(canvas){
    var c = canvas;
    canvasW = c.width;
    canvasH = c.height;
    var ctx = c.getContext("2d");
    ctx.moveTo(0,0);
    ctx.lineTo(canvasW*0.5,canvasH*0.25);
    ctx.stroke();
}