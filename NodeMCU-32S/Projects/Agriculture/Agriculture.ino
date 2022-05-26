/*
  ESP32 publish telemetry data to VOne Cloud (DHT22 sensor)
*/

#include "VOneMqttClient.h"
#include "DHT.h"

int MinMoistureValue = 4095;
int MaxMoistureValue = 2100;
int MinMoisture = 0;
int MaxMoisture = 100;
int Moisture = 0;

//define device id
const char* DHT22Sensor = "3f831071-fefc-412c-bed6-15d8bbdb32bc"; //dht22
const char* RainSensor = "464b7d12-27ca-45c6-858a-8d619dff879b";//rain sensor
const char* MoistureSensor = "ca4b26fd-9fa8-4e8b-aa7c-7a2a2d376057"; //moisture sensor

//Used Pins
const int dht22Pin = 2;
const int rainPin = 26;
const int moisturePin = 34;

//input sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
DHT dht(dht22Pin, DHTTYPE);

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
  dht.begin();
  pinMode(rainPin, INPUT);
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "DHTSensor Fail";
    voneClient.publishDeviceStatusEvent(DHT22Sensor, true);
    voneClient.publishDeviceStatusEvent(RainSensor, true);
    voneClient.publishDeviceStatusEvent(MoistureSensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemetry data 1
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    JSONVar payloadObject;
    payloadObject["humidity"] = h;
    payloadObject["temperature"] = t;
    voneClient.publishTelemetryData(DHT22Sensor, payloadObject);

    //Sample sensor fail message
    //String errorMsg = "DHTSensor Fail";
    //voneClient.publishDeviceStatusEvent(DHT22Sensor, false, errorMsg.c_str());

    //Publish telemetry data 2
    int raining = !digitalRead(rainPin);
    voneClient.publishTelemetryData(RainSensor, "Raining", raining);

    //Publish telemetry data 3
    int sensorValue = analogRead(moisturePin);
    Moisture = map(sensorValue, MinMoistureValue, MaxMoistureValue, MinMoisture, MaxMoisture);
    voneClient.publishTelemetryData(MoistureSensor, "moisture", Moisture);
  }
}
