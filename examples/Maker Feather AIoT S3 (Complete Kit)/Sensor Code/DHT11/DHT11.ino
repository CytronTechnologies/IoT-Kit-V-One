/*
ESP32 publish telemetry data to VOne Cloud (DHT11 sensor)
*/

#include "VOneMqttClient.h"
#include "DHT.h"


//define device id
const char* DHT11sensor = "29099b40-50a8-457a-865f-5c120863cb15";         //Replace with the deviceID of YOUR DHT11 sensor

//Used Pins
const int dht11Pin = 4;        //Left side Maker Port

//input sensor
#define DHTTYPE DHT11
DHT dht(dht11Pin, DHTTYPE);

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
    voneClient.publishDeviceStatusEvent(DHT11sensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;
   
    //Publish telemetry data
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    JSONVar payloadObject;    
    payloadObject["Humidity"] = h;
    payloadObject["Temperature"] = t;
    voneClient.publishTelemetryData(DHT11sensor, payloadObject);

    //Sample sensor fail message
    //String errorMsg = "DHTSensor Fail";
    //voneClient.publishDeviceStatusEvent(DHT11Sensor, false, errorMsg.c_str());
  }
}
