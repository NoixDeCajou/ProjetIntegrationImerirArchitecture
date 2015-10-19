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

byte mac[] = {  0x98, 0x4F, 0xEE, 0x05, 0x34, 0x14 };

//LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int lcd_key     = 0;
int adc_key_in  = 0;


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
int read_LCD_buttons();

/******************************/


void setup() {
   lcd.begin(16, 2);              // start the library
   lcd.setCursor(0,0);
   lcd.print("> Initialisation ..."); // print a simple message
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
        
        initWebSocket();
        //WSclient.monitor();
        
    } else {
        Serial.println("There was some problem in parsing the JSON");
    }

}

void loop() {
 //WSclient.monitor();
 lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
 lcd.print(millis()/1000);      // display seconds elapsed since power-up


 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = read_LCD_buttons();  // read the buttons

 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     lcd.print("RIGHT ");
     Serial.print("RIGHT ");
     break;
     }
   case btnLEFT:
     {
     lcd.print("LEFT   ");
     Serial.print("LEFT ");
     break;
     }
   case btnUP:
     {
     lcd.print("UP    ");
     Serial.print("UP ");
     break;
     }
   case btnDOWN:
     {
     lcd.print("DOWN  ");
     Serial.print("DOWN ");
     break;
     }
   case btnSELECT:
     {
     lcd.print("SELECT");
     Serial.print("SELECT ");
     break;
     }
     case btnNONE:
     {
     lcd.print("NONE  ");
     Serial.print("NONE ");
     break;
     }
 }
 WSclient.monitor();
}

String readPage(){
  //read the page, and capture & return everything between '{' and '}'

  stringPos = 0;
  memset( &inString, 0, 32 ); //clear inString memory

  while(true){

    if (client.available()) {
      char c = client.read();

      if (c == '{' ) { //'{' is our begining character
        startRead = true; //Ready to start reading the part 
          inString[stringPos] = '{';
          stringPos ++;
      }else if(startRead){

        if(c != '}'){ //'}' is our ending character
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


// read the buttons
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

 // For V1.0 comment the other threshold and use the one below:
/*
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
*/


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
  client.send("Hello World! nigga");
}

void onMessage(WebSocketClient client, char* message) {
  Serial.println("EXAMPLE: onMessage()");
  Serial.print("Received: "); Serial.println(message);
}

void onError(WebSocketClient client, char* message) {
  Serial.println("EXAMPLE: onError()");
  Serial.print("ERROR: "); Serial.println(message);
}
