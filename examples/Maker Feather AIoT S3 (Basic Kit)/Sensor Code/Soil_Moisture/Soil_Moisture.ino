/*
  ESP32 publish telemetry data to VOne Cloud and subcribe to execute controller (Soil Moisture)
*/

#include "VOneMqttClient.h"

int MinMoistureValue = 4095;
int MaxMoistureValue = 1500;
int MinMoisture = 0;
int MaxMoisture = 100;
int Moisture = 0;

//define device id
const char* MoistureSensor = "ca4b26fd-9fa8-4e8b-aa7c-7a2a2d376057";          //Replace this with YOUR deviceID for the moisture sensor

//Used Pins
const int moisturePin = A2;      //Middle Maker Port

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
    String errorMsg = "Moisture Sensor Fail";
    voneClient.publishDeviceStatusEvent(MoistureSensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemtry data
    int sensorValue = analogRead(moisturePin);
    Moisture = map(sensorValue, MinMoistureValue, MaxMoistureValue, MinMoisture, MaxMoisture);
    voneClient.publishTelemetryData(MoistureSensor, "Soil moisture", Moisture);
  }
}
