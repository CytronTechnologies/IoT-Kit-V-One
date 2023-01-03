/*
  ESP32 publish telemetry data to VOne Cloud (Pet Feeder)
*/

#include "VOneMqttClient.h"
#include <ESP32Servo.h>

//define device id
const char* ServoMotor = "d33bdf13-4595-4f04-b561-0b743238f25e";      //Replace with YOUR deviceID for the servo
const char* InfraredSensor = "756b1939-44ad-4872-8b2a-12dbd45bd9ff";  //Replace with YOUR deviceID for the infrared sensor

//Used Pins
const int servoPin = 4;          //Pin A4
const int InfraredPin = 42;      //Right side Maker Port

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

  //sensor
  pinMode(InfraredPin, INPUT);
  //actuator
  Myservo.attach(servoPin);
  Myservo.write(0);
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
