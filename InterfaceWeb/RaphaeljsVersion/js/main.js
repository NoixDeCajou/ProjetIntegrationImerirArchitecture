var jsonCity = '{"areas":[{"name":"Quartier Nord","map":{"weight":{"w":1,"h":1},"vertices":[{"name":"m","x":0.5,"y":0.5},{"name":"b","x":0.5,"y":1}],"streets":[{"name":"mb","path":["m","b"],"oneway":false}],"bridges":[{"from":"b","to":{"area":"Quartier Sud","vertex":"h"},"weight":2}]}},{"name":"Quartier Sud","map":{"weight":{"w":1,"h":1},"vertices":[{"name":"a","x":1,"y":1},{"name":"m","x":0,"y":1},{"name":"h","x":0.5,"y":0}],"streets":[{"name":"ah","path":["a","h"],"oneway":false},{"name":"mh","path":["m","h"],"oneway":false}],"bridges":[{"from":"h","to":{"area":"Quartier Nord","vertex":"b"},"weight":2}]}}]}';
jsonCity = data = $.parseJSON(jsonCity);
console.log(jsonCity);
var firstArea = jsonCity.areas[0];
var areaName = firstArea.name;
console.log(areaName);
$("#areaName").append(areaName);


window.onresize=function(){    
    var canvas = init();
    canvas.innerHTML = "";
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
// Creates canvas 320 × 200 at 10, 50
var paper = Raphael(canvas, canvas.width, canvas.height);

var w = canvas.width;
var h = canvas.height;



var c = paper.path("M 0,0 L "+canvas.width*0.5+" "+canvas.height*0.25);
//var c = paper.path("M10 10L90 90");
// Sets the stroke attribute of the circle to white
c.attr({stroke:'#000000',"stroke-width":5});
// c.glow({color: '#f00',width: 5});

// Creates circle at x = 50, y = 40, with radius 10
var circle = paper.circle(canvas.width*0.5, canvas.height*0.25, 10);
// Sets the fill attribute of the circle to red (#f00)
circle.attr("fill", "#f00");

/*****ANIMATIONS*****/
// Hover in function
function hoverIn() {
  this.animate({
    r: 20
  }, 500,"bounce");
}

// Hover out function
function hoverOut() {
  this.animate({
    r: 10
  }, 500,"bounce");
}

// Hover in function
function hoverInPath() {
  this.animate({
    stroke : "#f00"
  }, 500,"bounce");
}

// Hover out function
function hoverOutPath() {
  this.animate({
    stroke : "#000000"
  }, 500,"bounce");
}

circle.hover(hoverIn, hoverOut);
c.hover(hoverInPath, hoverOutPath);

}