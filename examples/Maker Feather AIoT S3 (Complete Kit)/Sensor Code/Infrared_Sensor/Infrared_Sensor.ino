/*
  ESP32 publish telemetry data to VOne Cloud (Infrared sensor)
*/

#include "VOneMqttClient.h"

//define device id
const char* InfraredSensor = "756b1939-44ad-4872-8b2a-12dbd45bd9ff";  //Replace with the deviceID of YOUR infrared sensor

//Used Pins
const int InfraredPin = 6;

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
    int InfraredVal = !digitalRead(InfraredPin);
    voneClient.publishTelemetryData(InfraredSensor, "Obstacle", InfraredVal);

  }
}
