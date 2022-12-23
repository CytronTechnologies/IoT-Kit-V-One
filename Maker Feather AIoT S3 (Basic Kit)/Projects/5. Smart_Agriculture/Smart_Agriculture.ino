/*
  ESP32 publish telemetry data to VOne Cloud (Smart Agriculture)
*/

#include "VOneMqttClient.h"
#include <ESP32Servo.h>

int MinMoistureValue = 4095;
int MaxMoistureValue = 2060;
int MinMoisture = 0;
int MaxMoisture = 100;
int Moisture = 0;

int MinDepthValue = 4095;
int MaxDepthValue = 2170;
int MinDepth = 0;
int MaxDepth = 100;
int depth = 0;

//define device id
const char* ServoMotor = "d33bdf13-4595-4f04-b561-0b743238f25e";      //Replace this with YOUR deviceID for the servo
const char* MoistureSensor = "abcf1c7f-b293-40dc-9fb0-add99e4e90f8";  //Replace this with YOUR deviceID for the moisture sensor
const char* WaterLevel = "345a038c-0fdc-4cd3-a60e-0060d83c32da";      //Replace this with YOUR deviceID for the water level sensor

//Used Pins
const int servoPin = 21;         //Pin 21
const int moisturePin = A3;      //Middle Maker Port
const int depthPin = 4;          //Left side Maker Port
const int buzzer = 12;           //Onboard buzzer

//Output
Servo Myservo;

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

void triggerActuator_callback(const char* actuatorDeviceId, const char* actuatorCommand)
{
  //actuatorCommand format {"servo":90}
  Serial.print("Main received callback : ");
  Serial.print(actuatorDeviceId);
  Serial.print(" : ");
  Serial.println(actuatorCommand);

  String errorMsg = "";

  JSONVar commandObjct = JSON.parse(actuatorCommand);
  JSONVar keys = commandObjct.keys();

  if (String(actuatorDeviceId) == ServoMotor)
  {
    //{"servo":90}
    String key = "";
    JSONVar commandValue = "";
    for (int i = 0; i < keys.length(); i++) {
      key = (const char* )keys[i];
      commandValue = commandObjct[keys[i]];

    }
    Serial.print("Key : ");
    Serial.println(key.c_str());
    Serial.print("value : ");
    Serial.println(commandValue);

    int angle = (int)commandValue;
    Myservo.write(angle);
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, errorMsg.c_str(), true);//publish actuator status
  }
  else
  {
    Serial.print(" No actuator found : ");
    Serial.println(actuatorDeviceId);
    errorMsg = "No actuator found";
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, errorMsg.c_str(), false);//publish actuator status
  }
}

void setup() {

  pinMode(buzzer, OUTPUT);

  setup_wifi();
  voneClient.setup();
  voneClient.registerActuatorCallback(triggerActuator_callback);

  //actuator
  Myservo.attach(servoPin);
  Myservo.write(0);
}

void loop() {



  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "Sensor Fail";
    voneClient.publishDeviceStatusEvent(MoistureSensor, true);
    voneClient.publishDeviceStatusEvent(WaterLevel, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemtry data
    int sensorValue = analogRead(moisturePin);
    Moisture = map(sensorValue, MinMoistureValue, MaxMoistureValue, MinMoisture, MaxMoisture);
    voneClient.publishTelemetryData(MoistureSensor, "Soil moisture", Moisture);

    //Publish telemtry data2
    int sensorValue2 = analogRead(depthPin);
    depth = map(sensorValue2, MinDepthValue, MaxDepthValue, MinDepth, MaxDepth);
    voneClient.publishTelemetryData(WaterLevel, "Depth", depth);
  }

  if (depth > 95) {
    tone(buzzer, 1000);
    delay(1000);
  }
  else
  {
    noTone(buzzer);
  }
}
