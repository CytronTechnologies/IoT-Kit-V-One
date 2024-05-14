/*
  ESP32 publish telemetry data to VOne Cloud (Visitor Counter)
*/

#include "VOneMqttClient.h"
#include <ESP32Servo.h>

//define device id
const char* InfraredSensor = "6d81b821-59b4-4e88-8096-47bc40eb8f4d";  //Replace with the deviceID of YOUR infrared sensor
const char* ServoMotor = "1fc35421-fde1-4589-b5d0-b020db545d09";      //Replace with the deviceID of YOUR servo motor

//Used Pins
const int InfraredPin1 = 21;
const int InfraredPin2 = 35;
const int ledPin1 = 32;
const int ledPin2 = 33;
const int servoPin = 27;

unsigned long lastMillis = 0;
volatile int count1 = 0;
int limit = 10;

//input sensor

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
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(InfraredPin1, INPUT_PULLUP);
  pinMode(InfraredPin2, INPUT_PULLUP);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);

  //actuator
  Myservo.attach(servoPin);
  Myservo.write(0);
}

void loop() {
  if (count1 >= limit) {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, LOW);
  }
  else
  {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, HIGH);
  }

    if ((millis() - lastMillis) > INTERVAL2) {
    lastMillis = millis();
    if (digitalRead(InfraredPin2) == 1) {
      count1++;
    }
    if (digitalRead(InfraredPin1) == 1) {
      if (count1 > 0) {
        count1--;
      }
    }
  }

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "Infrared sensor Fail";
    voneClient.publishDeviceStatusEvent(InfraredSensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;
    voneClient.publishTelemetryData(InfraredSensor, "Obstacle", count1);
  }
}
