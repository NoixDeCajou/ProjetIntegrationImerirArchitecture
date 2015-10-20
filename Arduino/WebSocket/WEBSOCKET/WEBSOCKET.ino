#include "Arduino.h"
#include <Ethernet.h>
#include <SPI.h>
#include <WebSocketClient.h>

byte mac[] = {  0x98, 0x4F, 0xEE, 0x05, 0x34, 0x14 };
char server[] = "192.168.1.69";
WebSocketClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("EXAMPLE: setup()");
  Ethernet.begin(mac);
  client.connect(server,8000);
  client.onOpen(onOpen);
  client.onMessage(onMessage);
  client.onError(onError);
  client.send("Hello World!");
}

void loop() {
  client.monitor();
}

void onOpen(WebSocketClient client) {
  Serial.println("EXAMPLE: onOpen()");
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
