#include "Arduino.h"
#include <Ethernet.h>
#include <SPI.h>
#include <WebSocketClient.h>
#include <aJSON.h>
#include <LiquidCrystal.h>

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

//Adresse mac de l'ethernet du galileo 10
byte mac[] = {  0x98, 0x4F, 0xEE, 0x05, 0x34, 0x14 };

//LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int lcd_key     = 0;
int adc_key_in  = 0;


//Adresse du serveur
IPAddress server(192,168,1,69); 
char WSserver[] = "192.168.1.69";

//Parametres de l'adresse du galileo
IPAddress ip(192,168,1,127);
IPAddress dns(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

//Declaration du client ethernet et websocket
EthernetClient client;
WebSocketClient WSclient;

String requete = "";
boolean httpRequestDone = false;

//Declaration de variables utiles pour isoler le json de la reponse http 
char inString[32];
int stringPos = 0;
boolean startRead = false;

//Declaration de variables globales
int id; //id du cabdevice donné par le serveur en http
boolean isFree = true; // Pour determiner l'etat du taxi : occupé/libre
int requestNumber = 0; // Nombre de requete en file d'attente
String nextRequestVertice = ""; // Nom du vertice de la requete en cour
String areaRequest = ""; // Nom de la zone sur laquelle se trouve le taxi;
String currentPosition=""; //Position actuel du taxi (nom du vertice);
int idRequest; //Id de la requete en cours
double distance = 0; //Distance parcourue depuis le debut

String destinationArea = ""; //Nom de la zone à atteindre
String destinationVertice = ""; //Nom du vertice à atteindre


/*****Definition Fonction******/
//Retourne le port de la websocket ou l'id du cabrequest
int getHttpInfo(char *jsonString, char*);
//Initialise la websocket avec le port passé en parametre
void initWebSocket(int);
//Initialise les bouttons du LCD
int read_LCD_buttons();
//Retourne le nombre de requetes en file d'attente
int getRequestnumber(char *jsonString);
//Retourne l'id de la prochaine requete
int getIdRequest(char *jsonString);
//Retourne le nom du vertice de la prochaine requete
char* getNextRequest(char *jsonString);
//Retourne la position du taxi (nom de vertice)
char* getCurrentPosition(char *jsonString);
//Retourne le nom de la zone de la prochaine requete
char* getAreaRequest(char *jsonString);
//Retourne la distance parcourue depuis le debut
double getDistance(char *jsonString);
/******************************/


void setup() {
	lcd.begin(16, 2);  //Initialise le LCD 16Colones 2lignes
	lcd.setCursor(0,0); //Positione le curseur au debut
	lcd.print("> Initialisation ...");
	//system("ifconfig > /dev/ttyGS0"); //affiche la configuration reseau pour debug

	Serial.begin(9600);
	while (!Serial) {
	; // attend la connexion du port serie
	}
	Serial.println("Serial : OK...");

	Ethernet.begin(mac,ip,dns,gateway,subnet); //initialisation de l'ethernet
	// Donne une petite seconde à l'ethernet pour initialisation
	delay(1000);
	Serial.println("Ethernet : OK...");

	Serial.println(Ethernet.localIP());//retourne l'ip du galileo

	// Si la connexion ethernet est validé
	if (client.connect(server, 5000)) {
		Serial.println("connected");

	// on formule une requete http:
		client.println("GET /cab HTTP/1.1");
		client.println();
	} 
	else { //echec de la connexion en ethernet
		Serial.println("connection failed");
	}


	String jsonHttpGet = readPage();//lecture et extration du json de la reponse http
	Serial.println(jsonHttpGet);

	//Conversion String vers char array pour faire correspondre avec les parametres de la fonction de parse Json de la librairie Ajson
	int size = jsonHttpGet.length()+1;
	char charJsonHttpGet[size];
	jsonHttpGet.toCharArray(charJsonHttpGet,size);
	char* jsonString = charJsonHttpGet;
	int value = getHttpInfo(jsonString,"portWebSocket");
	id = getHttpInfo(jsonString,"id");

	if (value) {
		Serial.println("Successfully Parsed: ");
		Serial.println(value);

		initWebSocket();
	//WSclient.monitor();

	} else {
		Serial.println("There was some problem in parsing the JSON");
	}
	lcd.clear();
}

void loop() {
//lcd.clear();

	WSclient.monitor(); //on regarde s'il y a de l'activité sur la Websocket

	lcd.setCursor(0,0);
	if(isFree == true){
		lcd.print("                ");
		lcd.setCursor(0,0);
		lcd.print("FREE");
		lcd.setCursor(0,1);
		lcd.print("                ");

		lcd.setCursor(14,0);    
		if(requestNumber == 0){
			lcd.print("0");
		}else{
			lcd.print(requestNumber);
			lcd.setCursor(0,1);
			lcd.print("                ");
			lcd.setCursor(0,1);
			lcd.print(areaRequest+" "+nextRequestVertice);
			//Serial.print("ID REQUEST :");Serial.println(idRequest);
			lcd_key = read_LCD_buttons();  // lecture des bouttons

			switch (lcd_key)
			{
				case btnLEFT:
				{
					String messageToServer = "{\"id\" : ";messageToServer+= id;
					messageToServer+= " , \"idCabRequest\" : "; messageToServer+= idRequest;
					messageToServer+= " , \"accepted\" : false }";
					Serial.println(messageToServer);

					int size = messageToServer.length()+1;
					char charMessageToServer[size];
					messageToServer.toCharArray(charMessageToServer,size);
					char* messageToServerFinal = charMessageToServer;

					WSclient.send(messageToServerFinal);
					break;
				}
				case btnSELECT:
				{
					destinationArea = areaRequest;
					destinationVertice = nextRequestVertice;

					isFree = false;

					String messageToServer = "{\"id\" : ";messageToServer+= id;
					messageToServer+= " , \"idCabRequest\" : "; messageToServer+= idRequest;
					messageToServer+= " , \"accepted\" : true }";
					Serial.println(messageToServer);

					int size = messageToServer.length()+1;
					char charMessageToServer[size];
					messageToServer.toCharArray(charMessageToServer,size);
					char* messageToServerFinal = charMessageToServer;

					WSclient.send(messageToServerFinal);

					break;
				}
				case btnRIGHT:
				{
					isFree = true;
					break;
				}
				case btnNONE:
				{
					break;
				}
			}
		} 
	}else{//occupé
		if( currentPosition == destinationVertice ){
			isFree = true;
		}else{
			lcd.print("BUSY");   
			lcd.setCursor(14,0);  
			lcd.print(requestNumber);
			lcd.setCursor(0,1);
			lcd.print("                ");
			lcd.setCursor(0,1);
			lcd.print(currentPosition);
		}
	}

	lcd.setCursor(5,0);
	lcd.print("d");
	lcd.setCursor(6,0);
	lcd.print(distance);

}

String readPage(){
//lis la reponse http et retourne tout ce qui se trouve entre '{' et '}'
	stringPos = 0;
	memset( &inString, 0, 32 ); //remet à 0

	while(true){

		if (client.available()) {
			char c = client.read();

			if (c == '{' ) { //'{' carractère de debut
				startRead = true;
				inString[stringPos] = '{';
				stringPos ++;
			}else if(startRead){

				if(c != '}'){ //'}' caractère de fin
					inString[stringPos] = c;
					stringPos ++;
				}else{
					inString[stringPos] = '}';
					stringPos ++;
					//On a tout , on se deconnect
					startRead = false;
					client.stop();
					client.flush();
					Serial.println("disconnecting.");
					return inString;

				}
			}
		}
	}
}




int getHttpInfo(char *jsonString, char *demande) 
{
	int value;

	aJsonObject* root = aJson.parse(jsonString);

	Serial.println("TEST ROOT");

	if (root != NULL) {
		aJsonObject* query = aJson.getObjectItem(root, demande); 

		if (query != NULL) {
			value = query->valueint;
		}
	}else{
		Serial.println("root null");
	}

	if (value) {
		return value;
	} else {
		return NULL;
	}
}

int getRequestnumber(char *jsonString){
	int value;

	aJsonObject* root = aJson.parse(jsonString);

	Serial.println("TEST ROOT");

	if (root != NULL) {
		aJsonObject* nbCabRequest = aJson.getObjectItem(root, "nbCabRequest"); 

		if (nbCabRequest != NULL) {
			value = nbCabRequest->valueint;
		}
	}else{
		Serial.println("root null");
	}

	if (value) {
		return value;
	} else {
		return NULL;
	}  
}


char* getNextRequest(char *jsonString){
	char* value;

	aJsonObject* root = aJson.parse(jsonString);

	if (root != NULL) {
		aJsonObject* cabRequests = aJson.getObjectItem(root, "cabRequests"); 

		if (cabRequests != NULL) {
			aJsonObject* firstCabRequests = aJson.getArrayItem(cabRequests, 0); 

			if (firstCabRequests != NULL) {
				aJsonObject* location = aJson.getObjectItem(firstCabRequests, "location"); 

				if (location != NULL) {
					aJsonObject* subLocation = aJson.getObjectItem(location, "location"); 

					if (subLocation != NULL) {
						value = subLocation->valuestring;
					}
				}
			}
		}
	}

	if (value) {
		return value;
	} else {
		return NULL;
	}  
}

char* getAreaRequest(char *jsonString){
	char* value;

	aJsonObject* root = aJson.parse(jsonString);

	if (root != NULL) {
		aJsonObject* cabRequests = aJson.getObjectItem(root, "cabRequests"); 

		if (cabRequests != NULL) {
			aJsonObject* firstCabRequests = aJson.getArrayItem(cabRequests, 0); 

			if (firstCabRequests != NULL) {
				aJsonObject* location = aJson.getObjectItem(firstCabRequests, "location"); 

				if (location != NULL) {
					aJsonObject* area = aJson.getObjectItem(location, "area"); 

					if (area != NULL) {
						value = area->valuestring;
					}
				}
			}
		}
	}

	if (value) {
		return value;
	} else {
		return NULL;
	}  
}


int getIdRequest(char *jsonString){
	int value;

	aJsonObject* root = aJson.parse(jsonString);

	if (root != NULL) {
		aJsonObject* cabRequests = aJson.getObjectItem(root, "cabRequests"); 

		if (cabRequests != NULL && (aJson.getArraySize(cabRequests) != 0)) {
			aJsonObject* firstCabRequests = aJson.getArrayItem(cabRequests, 0); 

			if (firstCabRequests != NULL) {
				aJsonObject* idCabRequest = aJson.getObjectItem(firstCabRequests, "idCabRequest"); 

				if (idCabRequest != NULL) {
					value = idCabRequest->valueint;
				}
			}
		}
	}

	if (value) {
		return value;
	} else {
		return NULL;
	}  
}


char* getCurrentPosition(char *jsonString){
	char* value;

	aJsonObject* root = aJson.parse(jsonString);

	if (root != NULL) {
		aJsonObject* cabInfo = aJson.getObjectItem(root, "cabInfo"); 

		if (cabInfo != NULL) {
			aJsonObject* locNow = aJson.getObjectItem(cabInfo, "loc_now"); 

			if (locNow != NULL) {
				aJsonObject* locationType = aJson.getObjectItem(locNow, "locationType"); 

				if (locationType != NULL) {
					String localisationTypeString = locationType->valuestring;
					if(localisationTypeString == "vertex"){
						aJsonObject* location = aJson.getObjectItem(locNow, "location");

						if (location != NULL) {
							value = location->valuestring;
						}
					}else if(localisationTypeString == "street"){
						aJsonObject* location = aJson.getObjectItem(locNow, "location");
						if (location != NULL) {
							aJsonObject* from = aJson.getObjectItem(location, "from");
							if (from != NULL) {
								value = from->valuestring;
							}                     
						}
					}
				}
			}
		}

	}

	if (value) {
		return value;
	} else {
		return NULL;
	}    
}

double getDistance(char *jsonString){
	double value;

	aJsonObject* root = aJson.parse(jsonString);

	if (root != NULL) {
		aJsonObject* cabInfo = aJson.getObjectItem(root, "cabInfo"); 

		if (cabInfo != NULL) {
			aJsonObject* odometer = aJson.getObjectItem(cabInfo, "odometer"); 

			if (odometer != NULL) {
				value = odometer->valuefloat;
			}
		}

	}

	if (value) {
		return value;
	} else {
		return NULL;
	}    
}


// Fonction d'example du site du constructeur
int read_LCD_buttons()
{
adc_key_in = analogRead(0);      // read the value from the sensor 
// my buttons when read are centered at these valies: 0, 144, 329, 504, 741
// we add approx 50 to those values and check to see if we are close
if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
// For V1.1 us this threshold
if (adc_key_in < 50)   return btnRIGHT;  
if (adc_key_in < 250)  return btnUP; 
if (adc_key_in < 450)  return btnDOWN; 
if (adc_key_in < 650)  return btnLEFT; 
if (adc_key_in < 850)  return btnSELECT;  

return btnNONE;  // when all others fail, return this...
}



void initWebSocket(){
	WSclient.connect(WSserver,8000);
	WSclient.onOpen(onOpen);
	WSclient.onMessage(onMessage);
	WSclient.onError(onError);
	Serial.println("WebSocket OK...");
}

void onOpen(WebSocketClient client) {
	Serial.println("EXAMPLE: onOpen()");
}

void onMessage(WebSocketClient client, char* message) {
	//A la reception du message de la websocket , on recupere tout ce dont on a besoin
	idRequest = getIdRequest(message);
	requestNumber = getRequestnumber(message);
	nextRequestVertice = getNextRequest(message);
	areaRequest = getAreaRequest(message);
	currentPosition = getCurrentPosition(message);
	distance = getDistance(message);
	Serial.print("CURRENT POSITION = ");Serial.println(currentPosition);
	Serial.print("NombreRequest = ");Serial.println(requestNumber);
	Serial.print("NextRequest = ");Serial.println(getNextRequest(message));
}

void onError(WebSocketClient client, char* message) {
	Serial.println("EXAMPLE: onError()");
	Serial.print("ERROR: "); Serial.println(message);
}
