/*
ESP32 publish telemetry data to VOne Cloud and subcribe to relay
*/

#include "VOneMqttClient.h"

//define device id
const char* Relay = "30f2294d-e7f1-4808-ba80-fbaecbb7885c";         //Replace with YOUR deviceID for relay

//Used Pins
const int relayPin = 14;       //Pin 14

//Output

//Create an instance of VOneMqttClient
VOneMqttClient voneClient;

//last message time
unsigned long lastMsgTime = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void triggerActuator_callback(const char* actuatorDeviceId, const char* actuatorCommand)
{ 
    //actuatorCommand format {"servo":90}
    Serial.print("Main received callback : ");
    Serial.print(actuatorDeviceId);
    Serial.print(" : ");
    Serial.println(actuatorCommand);
    
    String errorMsg = "";  

    JSONVar commandObjct = JSON.parse(actuatorCommand);
    JSONVar keys = commandObjct.keys();
      
    if(String(actuatorDeviceId) == Relay)
    {
       //{"LEDLight":false}
        String key = "";
        bool commandValue = "";
        for (int i = 0; i < keys.length(); i++) {
           key = (const char* )keys[i];
           commandValue = (bool)commandObjct[keys[i]];
          Serial.print("Key : ");
          Serial.println(key.c_str());
          Serial.print("value : ");
          Serial.println(commandValue);           
        }
        
        if(commandValue == true){ 
           Serial.println("Relay ON");                
           digitalWrite(relayPin, true);
        }
        else {
          Serial.println("Relay OFF");    
          digitalWrite(relayPin, false);
        }   

        voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, true);
        //Sample publish actuator fail status   
        //errorMsg = "LED unable to light up.";
        //voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, errorMsg.c_str(), false);
    }
}

void setup() {  
  
  setup_wifi();   
  voneClient.setup();
  voneClient.registerActuatorCallback(triggerActuator_callback);
  
  //actuator 
  pinMode(relayPin, OUTPUT);  
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;
       
  }
}
