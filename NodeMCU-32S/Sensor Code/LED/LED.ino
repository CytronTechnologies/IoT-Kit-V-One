/*
ESP32 publish telemetry data to VOne Cloud and subcribe to execute controller (LED)
*/

#include "VOneMqttClient.h"

//define device id
const char* LEDLight = "4d785918-9050-4e91-8567-b4b96533d671"; //Replace this with YOUR deviceID for the LED

//Used Pins
const int ledPin = 23; //GPIO13. D7

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
      
    if(String(actuatorDeviceId) == LEDLight)
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
           Serial.println("LED ON");                
           digitalWrite(ledPin, true);
        }
        else {
          Serial.println("LED OFF");    
          digitalWrite(ledPin, false);
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
  pinMode(ledPin, OUTPUT);  
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
