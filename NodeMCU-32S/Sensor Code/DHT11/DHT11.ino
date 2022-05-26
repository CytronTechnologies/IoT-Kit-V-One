/*
ESP32 publish telemetry data to VOne Cloud (DHT22 sensor)
*/

#include "VOneMqttClient.h"
#include "DHT.h"


//define device id
const char* DHT22Sensor = "3f831071-fefc-412c-bed6-15d8bbdb32bc"; //dht22

//Used Pins
const int dht22Pin = 2; //GPIO2. D4

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
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "DHTSensor Fail";
    voneClient.publishDeviceStatusEvent(DHT22Sensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;
   
    //Publish telemetry data
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    JSONVar payloadObject;    
    payloadObject["humidity"] = h;
    payloadObject["temperature"] = t;
    voneClient.publishTelemetryData(DHT22Sensor, payloadObject);

    //Sample sensor fail message
    //String errorMsg = "DHTSensor Fail";
    //voneClient.publishDeviceStatusEvent(DHT22Sensor, false, errorMsg.c_str());
  }
}
