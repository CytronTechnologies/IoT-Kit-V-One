/*
ESP32 publish telemetry data to VOne Cloud (Push_Button)
*/

#include "VOneMqttClient.h"

//define device id
const char* DigitalInput = "8c08337d-5244-46eb-a84e-e6ce2f798c1a";//digital input

//Used Pins
const int buttonPin = 23;

//input sensor

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

void setup() {  
  
  setup_wifi();   
  voneClient.setup();
  
  //sensor
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    voneClient.publishDeviceStatusEvent(DigitalInput,true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemetry data
    int digitalInputValue = digitalRead(buttonPin);
    if(digitalInputValue == HIGH)
    {
      digitalInputValue = 0;
    }
    else
    {
       digitalInputValue = 1;
    }    
     voneClient.publishTelemetryData(DigitalInput,"Button1", digitalInputValue);
       
  }
}
