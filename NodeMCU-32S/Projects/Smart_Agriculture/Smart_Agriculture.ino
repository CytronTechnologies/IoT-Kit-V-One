/*
  ESP32 publish telemetry data to VOne Cloud and subcribe to execute controller (servo)
*/

#include "VOneMqttClient.h"
#include <Servo.h>

int MinMoistureValue = 4095;
int MaxMoistureValue = 1500;
int MinMoisture = 0;
int MaxMoisture = 100;
int Moisture = 0;

int MinDepthValue = 4095;
int MaxDepthValue = 2170;
int MinDepth = 0;
int MaxDepth = 100;
int depth = 0;

//define device id
const char* ServoMotor = "e5c08526-7a1c-43ad-86ea-cfdc021836ba"; //servo motor
const char* MoistureSensor = "ca4b26fd-9fa8-4e8b-aa7c-7a2a2d376057"; //moisture sensor
const char* WaterLevel = "19dad3ae-2b27-4929-b766-d41e6ce8699e"; //Water Level sensor

//Used Pins
const int servoPin = 13;
const int moisturePin = 34;
const int depthPin = 35;

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
    voneClient.publishTelemetryData(MoistureSensor, "moisture", Moisture);

    //Publish telemtry data2
    int sensorValue2 = analogRead(depthPin);
    depth = map(sensorValue2, MinDepthValue, MaxDepthValue, MinDepth, MaxDepth);
    voneClient.publishTelemetryData(WaterLevel, "Depth", depth);
  }
}
