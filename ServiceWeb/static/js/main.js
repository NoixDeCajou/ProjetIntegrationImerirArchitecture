/***********************************
*
*		Gestion du dessin
*	
*
*
/***********************************/

var jsonCity = '{"areas":[{"name":"Quartier Nord","map":{"weight":{"w":1,"h":1},"vertices":[{"name":"m","x":0.5,"y":0.5},{"name":"b","x":0.5,"y":1}],"streets":[{"name":"mb","path":["m","b"],"oneway":false}],"bridges":[{"from":"b","to":{"area":"Quartier Sud","vertex":"h"},"weight":2}]}},{"name":"Quartier Sud","map":{"weight":{"w":1,"h":1},"vertices":[{"name":"a","x":1,"y":1},{"name":"m","x":0,"y":1},{"name":"h","x":0.5,"y":0}],"streets":[{"name":"ah","path":["a","h"],"oneway":false},{"name":"mh","path":["m","h"],"oneway":false}],"bridges":[{"from":"h","to":{"area":"Quartier Nord","vertex":"b"},"weight":2}]}}]}';
jsonCity = data = $.parseJSON(jsonCity);
console.log(jsonCity);
var firstArea = jsonCity.areas[0];
var areaName = firstArea.name;
$("#areaName").append(areaName);

var paperGlobal;

var widthCanvas;
var heightCanvas;

var appelX = null;
var appelY = null;

var marker = null;
var cheminAppelPoint = null;


var listVerticesCircle = [];


window.onresize=function(){    
	var canvas = init();
	canvas.innerHTML = "";
	draw(canvas);
	if (appelX != null || appelY != null) {
		getPointProche(appelX,appelY);
	};
}

window.onload = function(){
	initWebService();
	var canvas = init();
	draw(canvas);
	//initWebSocket();
};

function init(){
	canvas = document.getElementById("myCanvas");
	canvas.width = document.body.clientWidth;
	canvas.height = document.body.clientHeight; 

	widthCanvas = canvas.width;
	heightCanvas = canvas.height;

	return canvas;
}


function draw(canvas){
	paperGlobal = Raphael(canvas, canvas.width, canvas.height);

	$.each(firstArea.map.vertices, function(i,item){
		var circle = paperGlobal.circle(widthCanvas*item.x, heightCanvas*item.y, 30);
		listVerticesCircle[item.name]=item;
		//var text = paper.text(canvas.width*item.x, canvas.height*item.y, item.name);
		//text.attr({"font-weight": "bold" , "font-size" : 20});
		circle.attr({stroke:'#888888',"stroke-width":5,"fill": "#f00",cursor: 'pointer'});
		circle.click(function(e) {
			e.stopPropagation();
			appelSurPoint(this,item.name);
		});
		circle.hover(hoverIn, hoverOut);
	});


	$.each(firstArea.map.streets, function(i,item){
		var pointA = listVerticesCircle[item.path[0]];
		var pointB = listVerticesCircle[item.path[1]];
		console.log(pointA);
		var c = paperGlobal.path("M "+widthCanvas*pointA.x+","+heightCanvas*pointA.y+" L"+widthCanvas*pointB.x+","+heightCanvas*pointB.y);
		c.attr({stroke:'#888888',"stroke-width":30});
		c.toBack();
	});

	//console.log(listVerticesCircle);

	/*****ANIMATIONS*****/
	// Hover in 
	function hoverIn() {
		this.animate({
			r: 40
		}, 500,"bounce");
	}

	// Hover out 
	function hoverOut() {
		this.animate({
			r: 30
		}, 500,"bounce");
	}

	// Hover in 
	function hoverInPath() {
		this.animate({
			stroke : "#f00"
		}, 500,"bounce");
	}

	// Hover out 
	function hoverOutPath() {
		this.animate({
			stroke : "#000000"
		}, 500,"bounce");
	}

}

$('#myCanvas').click(function(e) {
	if (cheminAppelPoint != null || marker != null ) {
		cheminAppelPoint.remove();
		marker.remove();
	};
	var posX = $(this).position().left,posY = $(this).position().top;
	posX = (e.pageX - posX);
	posY = (e.pageY - posY);

	appelX = posX/widthCanvas;
	appelY = posY/heightCanvas;

	getPointProche(appelX,appelY,this);
});

function appelSurPoint(caller,name){
	//caller.attr("fill" , "#5CB85C");
	if (cheminAppelPoint != null || marker != null ) {
		cheminAppelPoint.remove();
		marker.remove();
	};
	console.log(caller);
	doSendCabRequest(areaName,name);
	//alert("Un Taxi a été appelé sur le point "+name);
	$("#lastNotification").html("Un Taxi a été appelé sur le point "+name);
}

function calculeDistance(x1, y1, x2, y2) {
    return Math.sqrt(Math.pow((y2 - y1),2) + Math.pow((x2 - x1),2));
}

function getPointProche(posX,posY){
	var distanceLaPlusProche = 9999;
	var pointLePlusProche;
	canvas = document.getElementById("myCanvas");
	$.each(firstArea.map.vertices,function(i,item){
		var distance = calculeDistance(widthCanvas*posX,heightCanvas*posY,widthCanvas*item.x,heightCanvas*item.y);
		if (distance < distanceLaPlusProche) {
			distanceLaPlusProche = distance;
			pointLePlusProche = item;
		};
	});
	console.log(pointLePlusProche);
	console.log(distanceLaPlusProche);

	cheminAppelPoint = paperGlobal.path("M "+widthCanvas*posX+","+heightCanvas*posY+" L"+widthCanvas*pointLePlusProche.x+","+heightCanvas*pointLePlusProche.y);
	marker = paperGlobal.image("/static/img/marker.png", widthCanvas*posX-40, heightCanvas*posY-40, 80, 80);
	cheminAppelPoint.attr({"stroke-dasharray" :"-","stroke-width":3});

	doSendCabRequest(areaName,pointLePlusProche.name);
	$("#lastNotification").html("Un Taxi a été appelé sur le point "+pointLePlusProche.name);
	//alert("Un Taxi a été appelé sur le point "+pointLePlusProche.name);

}


/***********************************
*
*		Gestion du webservice
*	
*
*
/***********************************/
function initWebService(){
	$.getJSON( "/monitor", function( data ) {
		console.log(data);
	})
	.done(function( json ) {
		initWebSocket();
	})
	.fail(function( jqxhr, textStatus, error ) {
		var err = textStatus + ", " + error;
		alert( "Request Failed: " + err );
	});
}





/***********************************
*
*		Gestion de la websocket
*	
*
*
/***********************************/

function initWebSocket()
{
	var webSocketAddress = "ws://localhost:8000/"
	doConnect(webSocketAddress);
}

function doConnect(webSocketAddress)
{
	websocket = new WebSocket(webSocketAddress);
	websocket.onopen = function(evt) { onOpen(evt) };
	websocket.onclose = function(evt) { onClose(evt) };
	websocket.onmessage = function(evt) { onMessage(evt) };
	websocket.onerror = function(evt) { onError(evt) };
}

function onOpen(evt)
{
	console.log("connected\n" + evt);

}

function onClose(evt)
{
	console.log("disconnected\n" + evt);
}

function onMessage(evt)
{
	console.log("response: " + evt.data + '\n');
	//faire traitement json
}

function onError(evt)
{
	console.log('error: ' + evt.data + '\n');

	websocket.close();

}

function doSendCabRequest(areaName,vertexName)
{
	var message = '{"cabRequest": [{"area": "'+areaName+'","location": {"area": "'+areaName+'","locationType": "vertex","location": "'+vertexName+'"}}]}';
	websocket.send(message);
}

function doDisconnect() {
	websocket.close();
}
