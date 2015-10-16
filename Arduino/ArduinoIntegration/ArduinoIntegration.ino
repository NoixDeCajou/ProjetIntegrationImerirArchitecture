#include "Arduino.h"
#include <Ethernet.h>
#include <SPI.h>
#include <WebSocketClient.h>
#include <aJSON.h>

byte mac[] = {  0x98, 0x4F, 0xEE, 0x05, 0x34, 0x14 };

IPAddress server(192,168,1,69); // serveur
char WSserver[] = "192.168.1.69";

IPAddress ip(192,168,1,127); // 
IPAddress dns(192,168,1,1); // 
IPAddress gateway(192,168,1,1); // 
IPAddress subnet(255,255,255,0); // 

EthernetClient client;
WebSocketClient WSclient;

String requete = "";
boolean httpRequestDone = false;

char inString[32]; // string for incoming serial data
int stringPos = 0; // string index counter
boolean startRead = false; // is reading?


/*****Definition Fonction******/
int parseJson(char *jsonString) ;
void initWebSocket(int);

/******************************/


void setup() {
  system("ifconfig > /dev/ttyGS0"); //affiche la configuration reseau
  
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect.
  }
  Serial.println("Serial : OK...");
  
  Ethernet.begin(mac,ip,dns,gateway,subnet); 
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("Ethernet : OK...");
  
  Serial.println(Ethernet.localIP());
  
   // if you get a connection, report back via serial:
  if (client.connect(server, 5000)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /cab HTTP/1.1");
    client.println();
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

}

void loop() {
  String jsonHttpGet = readPage();
  Serial.println(jsonHttpGet);
  
  int size = jsonHttpGet.length()+1;
  char charJsonHttpGet[size];
  jsonHttpGet.toCharArray(charJsonHttpGet,size);
  char* jsonString = charJsonHttpGet;
  int value = parseJson(jsonString);

    if (value) {
        Serial.println("Successfully Parsed: ");
        Serial.println(value);
        initWebSocket(value);
        //WSclient.monitor();
        
    } else {
        Serial.println("There was some problem in parsing the JSON");
    }
    
    WSclient.monitor();
  
    //delay(2000);
  //si le client est connecté
  /*if(client.connected() && httpRequestDone == false){
    //Serial.println("Loop : connected == true");
    //Serial.println("ClientConnected");
    //si il y a des données a lire ...
    if (client.available()) {
      //Serial.println("Loop : Données disponibles !");
      char c = client.read();
      requete += c;
      //Serial.print(c);
    }
        Serial.print(requete);
        //httpRequestDone = true;

  }
  
  //si deconnexion
  else {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }*/
  
}

String readPage(){
  //read the page, and capture & return everything between '<' and '>'

  stringPos = 0;
  memset( &inString, 0, 32 ); //clear inString memory

  while(true){

    if (client.available()) {
      char c = client.read();

      if (c == '{' ) { //'<' is our begining character
        startRead = true; //Ready to start reading the part 
          inString[stringPos] = '{';
          stringPos ++;
      }else if(startRead){

        if(c != '}'){ //'>' is our ending character
          inString[stringPos] = c;
          stringPos ++;
        }else{
          inString[stringPos] = '}';
          stringPos ++;
          //got what we need here! We can disconnect now
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




int parseJson(char *jsonString) 
{
    int value;

    aJsonObject* root = aJson.parse(jsonString);
    
    Serial.println("TEST ROOT");

    if (root != NULL) {
        Serial.println("Parsed successfully 1 " );
        aJsonObject* query = aJson.getObjectItem(root, "portWebSocket"); 
                    
        if (query != NULL) {
            Serial.println("Parsed successfully 5 " );
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



void initWebSocket(int port){
  WSclient.connect(WSserver,port);
  WSclient.onOpen(onOpen);
  WSclient.onMessage(onMessage);
  WSclient.onError(onError);
  //WSclient.send("Hello World!");
}

void onOpen(WebSocketClient client) {
  Serial.println("EXAMPLE: onOpen()");
  client.send("Hello World! nigga");
}

void onMessage(WebSocketClient client, char* message) {
  Serial.println("EXAMPLE: onMessage()");
  client.send("monmessage");
  Serial.print("Received: "); Serial.println(message);
}

void onError(WebSocketClient client, char* message) {
  Serial.println("EXAMPLE: onError()");
  Serial.print("ERROR: "); Serial.println(message);
}
