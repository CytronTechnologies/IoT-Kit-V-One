/*
  ESP32 publish telemetry data to VOne Cloud
*/

#include "VOneMqttClient.h"

int MinDepthValue = 4095;
int MaxDepthValue = 2600;
int MinDepth = 0;
int MaxDepth = 45;
int depth = 0;

//define device id
const char* WaterLevel = "19dad3ae-2b27-4929-b766-d41e6ce8699e"; //Water Level sensor

//Used Pins
const int depthPin = 35;

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
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "Water Level Sensor Fail";
    voneClient.publishDeviceStatusEvent(WaterLevel, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemtry data
    int sensorValue = depthPin;
    depth = map(sensorValue, MinDepthValue, MaxDepthValue, MinDepth, MaxDepth);
    voneClient.publishTelemetryData(WaterLevel, "Depth", depth);
  }
}
