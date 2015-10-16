/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 modified 9 Apr 2012
 by David A. Mellis
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0x98, 0x4F, 0xEE, 0x05, 0x34, 0x14 };
//IPAddress server(173,194,33,104); // Google
IPAddress server(192,168,1,69); // serveur

IPAddress ip(192,168,1,127); // ip

IPAddress dns(192,168,1,1); // ip
IPAddress gateway(192,168,1,1); // ip
IPAddress subnet(255,255,255,0); // ip

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  //system("/etc/init.d/networking restart"); 
  
//system("ifdown eth0");
  //system("ifup eth0");
  //system("ifup eth0");
  //delay(3000);

  system("ifconfig > /dev/ttyGS0");
  
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start the Ethernet connection:
  /*if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }*/
  
    // start the Ethernet connection:
  Ethernet.begin(mac,ip,dns,gateway,subnet); 

  
  
  
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET / HTTP/1.1");
    client.println();
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
   system("ifconfig > /dev/ttyGS0");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  delay(2000);
  //si le client est connecté
  if(client.connected()){
    //Serial.println("Loop : connected == true");
    Serial.println("ClientConnected");
    //si il y a des données a lire ...
    if (client.available()) {
      //Serial.println("Loop : Données disponibles !");
      char c = client.read();
      Serial.print(c);
    }
    
  }
  
  //si deconnexion
  else {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}

