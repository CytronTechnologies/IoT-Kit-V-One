/*
  ESP32 publish telemetry data to VOne Cloud
*/

#include "VOneMqttClient.h"

float gasValue;

//define device id
const char* MQ2sensor = "5927205f-4421-46e8-a828-0a03f443799d"; //MQ2 sensor

//Used Pins
const int MQ2pin = 34;

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
  Serial.println("Gas sensor warming up!");
  delay(20000); // allow the MQ-2 to warm up
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "MQ2 Sensor Fail";
    voneClient.publishDeviceStatusEvent(MQ2sensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemtry data
    gasValue = analogRead(MQ2pin);
    voneClient.publishTelemetryData(MQ2sensor, "Gas detector", gasValue);
  }
}
