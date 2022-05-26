/*
  ESP32 publish telemetry data to VOne Cloud (Infrared sensor)
*/

#include "VOneMqttClient.h"

//define device id
const char* InfraredSensor = "bf28a317-0245-4ad2-b26e-443f43cbc4a2";  //infrared sensor

//Used Pins
const int InfraredPin = 36;

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
  pinMode(InfraredPin, INPUT_PULLUP);
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    voneClient.publishDeviceStatusEvent(InfraredSensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemetry data
    int InfraredVal = !digitalRead(36);
    voneClient.publishTelemetryData(InfraredSensor, "Infrared", InfraredVal);

  }
}
