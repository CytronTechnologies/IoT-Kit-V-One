/*
  ESP32 publish telemetry data to VOne Cloud (Rain_Sensor)
*/

#include "VOneMqttClient.h"

//define device id
const char* RainSensor = "464b7d12-27ca-45c6-858a-8d619dff879b";         //Replace with YOUR deviceID for rain sensor

//Used Pins
const int rainPin = 4;       //Left side Maker Port

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
  pinMode(rainPin, INPUT);
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    voneClient.publishDeviceStatusEvent(RainSensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemetry data
    int raining = !digitalRead(rainPin);
    voneClient.publishTelemetryData(RainSensor, "Raining", raining);

  }
}
