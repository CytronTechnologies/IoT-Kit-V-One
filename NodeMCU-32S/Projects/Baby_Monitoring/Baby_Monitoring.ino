/*
  ESP32 publish telemetry data to VOne Cloud (Baby Monitoring)
*/

#include "VOneMqttClient.h"

//define device id
const char* PIRsensor = "123914bb-debc-4490-b673-470b4bc73c44";  //Replace with YOUR deviceID for the PIR sensor

//Used Pins
const int motionSensor = 22;

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
  pinMode(motionSensor, INPUT);
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "PIRsensor Fail";
    voneClient.publishDeviceStatusEvent(PIRsensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    int movementTrigger = digitalRead(motionSensor);

    voneClient.publishTelemetryData(PIRsensor, "Motion", movementTrigger);

  }
}
