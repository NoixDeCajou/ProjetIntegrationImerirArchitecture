/*
 Sample sketch to demonstrate Multi Level JSON parsing in Arduino
 
 This sketch parses the complexly nested JSON
 
 Libraries needed:

 - aJson library for JSON parsing - https://github.com/interactive-matter/aJson
 
 Circuit:

    You don't need any circuit, but need the Arduino board

 Author: 

    Sudar - <http://sudarmuthu.com> <http://hardwarefun.com>
    Refer to http://hardwarefun.com/tutorials/parsing-json-in-arduino

 License:

    BeerWare ;)
 
 */
#include <aJSON.h>

// function definitions
char* parseJson(char *jsonString) ;




// Json string to parse
//char jsonString[] = "{\"error\":\"test\", \"portWebSocket\":8000, \"id\":0}";
//String monString = "{\"query\":{\"count\":1,\"created\":\"2012-08-04T14:46:03Z\",\"lang\":\"en-US\",\"results\":{\"item\":{\"title\":\"false\"}}}}";

void setup() {
  String monString = "{\"query\": {\"count\": 1,\"created\": \"2012-08-04T14: 46: 03Z\",\"lang\": \"en-US\", \"results\": {\"item\": [{\"title\": \"test1\"},{\"title\": \"test2\"}]}}}";
  //String monString = "{\"error\":\"test\", \"portWebSocket\":8000, \"id\":0}";
int size = monString.length()+1;
char test[size];
monString.toCharArray(test,size);
  char* jsonString = test;
  
  
  delay(2000);
    Serial.begin(9600);
    Serial.println(jsonString);
    Serial.println("Starting to parse");

/******************/

Serial.println(test);
/******************/


    char* value = parseJson(jsonString);

    if (value) {
        Serial.println("Successfully Parsed: ");
        Serial.println(value);
    } else {
        Serial.println("There was some problem in parsing the JSON");
    }
}

/**
 * Parse the JSON String. Uses aJson library
 * 
 * Refer to http://hardwarefun.com/tutorials/parsing-json-in-arduino
 */
char* parseJson(char *jsonString) 
{
    char* value;

    aJsonObject* root = aJson.parse(jsonString);
    
    Serial.println("TEST ROOT");

if (root != NULL) {
        //Serial.println("Parsed successfully 1 " );
        aJsonObject* query = aJson.getObjectItem(root, "query"); 

        if (query != NULL) {
            //Serial.println("Parsed successfully 2 " );
            aJsonObject* results = aJson.getObjectItem(query, "results"); 

            if (results != NULL) {
                //Serial.println("Parsed successfully 3 " );
                aJsonObject* item = aJson.getObjectItem(results, "item"); 

                if (item != NULL) {
                    //Serial.println("Parsed successfully 4 " );
                    aJsonObject* item0 = aJson.getArrayItem(item, 0); 
                    
                if (item0 != NULL) {
                    //Serial.println("Parsed successfully 4 " );
                    aJsonObject* title = aJson.getObjectItem(item0, "title"); 
                    
                    if (title != NULL) {
                        //Serial.println("Parsed successfully 5 " );
                        value = title->valuestring;
                    }else{
      Serial.println("title NULL TAMER");
      }
                }else{
      Serial.println("item NULL TAMER");
      }
            }else{
      Serial.println("results NULL TAMER");
      }
        }else{
      Serial.println("query NULL TAMER");
      }
    }else{
      Serial.println("ROOT NULL TAMER");
      }

    if (value) {
        return value;
    } else {
        return NULL;
    }
}

void loop() {
   // Nothing to do 
   ;
}
