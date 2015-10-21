/***********************************
*
*		Gestion du dessin
*	
*
/***********************************/
var id; //id du moniteur

var jsonCity; //ville au format json
var firstArea; //objet json de la zone du moniteur
var areaName; //nom de la zone du moniteur

var paperGlobal; //canvas Raphael.js

var widthCanvas; //largeur du canvas
var heightCanvas; //hauteur du canvas

var appelX = null;
var appelY = null;

var marker = null; //Objet svg du marker pieton
var cheminAppelPoint = null; //Objet svg du chemin en pointillé vers le point le plus proche

var taxi = null; //Objet svg du tavi
var taxiX = null; // x taxi
var taxiY = null; // y taxi

var lastNotification =null; //Dernier vertice demandé

var listVerticesCircle = []; //Liste des vertices


window.onresize=function(){  //Au redimentionnement de la page
	var canvas = init(); //Recuperation de la nouvelle largeur / hauteur
	canvas.innerHTML = ""; //Efface le canvas
	draw(canvas); //redessine le canvas
	if (appelX != null || appelY != null) { //si un marker est placé , on le redessine
		getPointProche(appelX,appelY);
	}
	if (taxi != null) { //si un taxi est placé , on le redessine
		taxi.remove();
		//taxi = null;  
		taxi = paperGlobal.image("/static/img/Taxi-50.png", widthCanvas*taxiX-25, heightCanvas*taxiY-25, 50, 50);
	}
}

window.onload = function(){ //Au chargement de la page , on initialise la connexion http 
	initWebService();
};

function init(){

	//Initialisation du canvas Raphael.js
	canvas = document.getElementById("myCanvas");
	canvas.width = document.body.clientWidth;
	canvas.height = document.body.clientHeight; 

	widthCanvas = canvas.width;
	heightCanvas = canvas.height;

	return canvas;
}


function draw(canvas){
	//Defini la zone de dessin
	paperGlobal = Raphael(canvas, canvas.width, canvas.height);

	//dessine les vertices
	$.each(firstArea.map.vertices, function(i,item){
		var circle = paperGlobal.circle(widthCanvas*item.x, heightCanvas*item.y, 30);
		listVerticesCircle[item.name]=item;
		//var text = paperGlobal.text(canvas.width*item.x, canvas.height*item.y-5, item.name);
		//text.attr({"font-family": 'Comic Sans MS", cursive, sans-serif' , "font-size" : 30,cursor: 'pointer'});
		circle.attr({stroke:'#888888',"stroke-width":5,"fill": "#f00",cursor: 'pointer'});

		//Sur un clique sur un vertice , on demande un taxi
		circle.click(function(e) {
			//bloque le onclick du canvas
			e.stopPropagation();
			appelSurPoint(this,item.name);
		});

		//Animation sur survol de la souris
		circle.hover(hoverIn, hoverOut);
	});

	//Dessine les routes
	$.each(firstArea.map.streets, function(i,item){
		var pointA = listVerticesCircle[item.path[0]];
		var pointB = listVerticesCircle[item.path[1]];
		console.log(pointA);
		var peinture = paperGlobal.path("M "+widthCanvas*pointA.x+","+heightCanvas*pointA.y+" L"+widthCanvas*pointB.x+","+heightCanvas*pointB.y);
		var rand = Math.floor(Math.random() * 2) + 1;
		//peinture.attr({"stroke-dasharray" :"-","stroke-width":3,stroke:'#FFFFFF',});
		if (rand == 1) {
			peinture.attr({"stroke-dasharray" :"-","stroke-width":3,stroke:'#FFFFFF',});
		}else{
			peinture.attr({"stroke-width":3,stroke:'#FFFFFF',});
		}
		peinture.toBack(); //envoie vers l'arriere pour laisser les vertices devant
		var c = paperGlobal.path("M "+widthCanvas*pointA.x+","+heightCanvas*pointA.y+" L"+widthCanvas*pointB.x+","+heightCanvas*pointB.y);
		c.attr({stroke:'#888888',"stroke-width":30});
		c.toBack();//envoie vers l'arriere
	});

	console.log(listVerticesCircle);

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

}

//Au clique sur le canvas, on dessine un marker et on appel un taxi sur le point le plus proche
$('#myCanvas').click(function(e) {
	if (cheminAppelPoint != null || marker != null ) { //si un marker est placé , on le redessine
		cheminAppelPoint.remove();
		marker.remove();
	};

	//recuperation des coordonées relative au canvas
	var posX = $(this).position().left,posY = $(this).position().top;
	posX = (e.pageX - posX);
	posY = (e.pageY - posY);

	//Transformation des coordonées en echelle 0-1
	appelX = posX/widthCanvas;
	appelY = posY/heightCanvas;

	//Demande du nom du vertice le plus proche
	var namePointLePlusProche = getPointProche(appelX,appelY,this);

	//Demande taxi au point le plus proche
	doSendCabRequest(areaName,namePointLePlusProche);

});

//Appel un taxi au vertice demander
function appelSurPoint(caller,name){
	//caller.attr("fill" , "#5CB85C");
	if (cheminAppelPoint != null && marker != null ) {
		cheminAppelPoint.remove();
		marker.remove();
	};
	console.log(caller);
	doSendCabRequest(areaName,name);
	//alert("Un Taxi a été appelé sur le point "+name);

	$("#lastNotification").html("Un Taxi a été appelé sur le point <strong>"+name+"</strong>");
	lastNotification = name;
}

//Simple fonction de calcule de distance entre deux points 2D
function calculeDistance(x1, y1, x2, y2) {
    return Math.sqrt(Math.pow((y2 - y1),2) + Math.pow((x2 - x1),2));
}

//Retourne le nom du Vertice le plus proche
//Parcours la liste des vertices afin de calculer la distance , retient le Vetice le plus proche
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

	//dessine le marker
	cheminAppelPoint = paperGlobal.path("M "+widthCanvas*posX+","+heightCanvas*posY+" L"+widthCanvas*pointLePlusProche.x+","+heightCanvas*pointLePlusProche.y);
	marker = paperGlobal.image("/static/img/marker.png", widthCanvas*posX-25, heightCanvas*posY-25, 50, 50);
	cheminAppelPoint.attr({"stroke-dasharray" :".","stroke-width":3});

	//doSendCabRequest(areaName,pointLePlusProche.name);
	$("#lastNotification").html("Un Taxi a été appelé sur le point <strong>"+pointLePlusProche.name+"</strong>");
	lastNotification = pointLePlusProche.name;

	return pointLePlusProche.name;

	//alert("Un Taxi a été appelé sur le point "+pointLePlusProche.name);

}


//Si le nombre max de moniteur est attein
function displayMonitorError(){
	$(".hide").removeClass("hide");
}


/***********************************
*
*		Gestion du webservice
*	
*
*
/***********************************/
function initWebService(){
	var webSocketPort; // port de la websocket
	//appel ajax pour recuperer du json en http
	$.getJSON( "/monitor", function( data ) {

	})
	.done(function( json ) { //succes
		if(json.error == false){

			id = json.id;
			webSocketPort = json.portWebSocket;
			jsonCity = json.map;

			initWebSocket(webSocketPort); // on lance la websocket

			console.log(jsonCity);
			firstArea = jsonCity.areas[id-1]; //attribution de la zone
			areaName = firstArea.name;
			$("#areaName").append(areaName);


			//Lancement du dessin
			var canvas = init();
			draw(canvas);

		}else{
			//alert(json.explication)
			displayMonitorError();
		}
	}) 
	.fail(function( jqxhr, textStatus, error ) { //echec
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

function initWebSocket(webSocketPort)
{
	//On recupere l'ip du serveur
	var ip = location.host;
	ip = ip.split(":");
	ip = ip[0];
	//alert(ip);
	//Connexion a la websocket
	var webSocketAddress = "ws://"+ip+":"+webSocketPort+"/"
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
	websocket.close();
	console.log("disconnected\n" + evt);
}

function onMessage(evt)
{
	//Recuperation de la chaine json broadcasté
	console.log(evt.data);
	var info = jQuery.parseJSON(evt.data);
	//Si c'est bien un message broadcasté on lance le traitement
	if(info.hasOwnProperty('cabInfo')){
		var area = info.cabInfo.loc_now.area; //recuperation de la zone actuel du taxi
		//si la zone du taxi correspond à la zone traité par le monitor, on peut afficher le taxi
		if (area == areaName) {
			var locationType = info.cabInfo.loc_now.locationType; //recuperation du type de localisation vertice /street
			//si le taxi est sur un vertice , on le dessine
			if(locationType == "vertex"){
				var location = info.cabInfo.loc_now.location;
				//si le taxi n'existe pas sur le moniteur, on le dessine à l'endroit designé par le serveur
				if (taxi == null) {
					taxi = paperGlobal.image("/static/img/Taxi-50.png", widthCanvas*listVerticesCircle[location].x-25, heightCanvas*listVerticesCircle[location].y-25, 50, 50);
				}else{//sinon, le taxi exite , on le deplace à l'endroit designé par le serveur par une animation
					var xDestination = widthCanvas*listVerticesCircle[location].x-25;
					var yDestination = heightCanvas*listVerticesCircle[location].y-25;
					//L'animation ne supporte pas les floats
					xDestination=Math.round(xDestination);
					yDestination=Math.round(yDestination);
					//Animation easein easeout vers le x et y demandé (1seconde d'animation)
					taxi.animate({x: xDestination , y: yDestination}, 1000, "<>");
				}
				//dans tout les cas, on recupere les coordonées du taxi pour le redessiner au redimentionnement
				taxiX = listVerticesCircle[location].x;
				taxiY = listVerticesCircle[location].y;

				//Si le taxi arrive au vertice demandé, on efface le marker
				if (lastNotification != null) {
					if ( (taxiX == listVerticesCircle[lastNotification].x) && (taxiY == listVerticesCircle[lastNotification].y) ) {
						if (cheminAppelPoint != null || marker != null ) {
							cheminAppelPoint.remove();
							marker.remove();
							$("#lastNotification").html("");
						}
					}
				}

			}else{ //street
				//pas geré
			}
		}else{//Pas dans la zone on supprime le taxi
			if(taxi != null){
				taxi.remove();
				taxi = null;
			}
		}
	}
}

function onError(evt)
{
	console.log('error: ' + evt.data + '\n');

	websocket.close();

}

function doSendCabRequest(areaName,vertexName)
{
	var message = '{"id": '+id
				+' , "cabRequest": {"area": "'+areaName
				+'","location": {"area": "'+areaName
				+'","locationType": "vertex","location": "'+vertexName
				+'"}}}';
	console.log("Message envoyé : " + message);
	websocket.send(message);
}
