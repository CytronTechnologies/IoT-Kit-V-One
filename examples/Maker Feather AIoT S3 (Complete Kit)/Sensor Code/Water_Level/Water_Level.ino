/*
  ESP32 publish telemetry data to VOne Cloud (Water Level Sensor)
*/

#include "VOneMqttClient.h"

int MinDepthValue = 4095;
int MaxDepthValue = 2200;
int MinDepth = 0;
int MaxDepth = 45;
int depth = 0;

//define device id
const char* WaterLevel = "351eebb8-4b04-486b-a16e-8775cf353a28";        //Replace this with YOUR deviceID for the water level sensor

//Used Pins
const int depthPin = 4;       //Left side Maker Port

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
    int sensorValue = analogRead(34);
    depth = map(sensorValue, MinDepthValue, MaxDepthValue, MinDepth, MaxDepth);
    voneClient.publishTelemetryData(WaterLevel, "Depth", depth);
  }
}
