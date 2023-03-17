/*
  ESP32 publish telemetry data to VOne Cloud (Smart Kitchen)
*/

#include "VOneMqttClient.h"
#include <ESP32Servo.h>
#include "DHT.h"
float gasValue;

//define device id
const char* Relay = "804eb6ba-09c9-43c5-a58e-37a81d619eb0";        //Replace this with YOUR deviceID for the relay
const char* ServoMotor = "7b9ad8f7-dc0f-4979-9e0d-87a6cab8ff83";   //Replace this with YOUR deviceID for the servo
const char* MQ2sensor = "ed7f6ce9-3bd2-4f6d-8dae-6d060f392c0f";    //Replace this with YOUR deviceID for the MQ2 sensor
const char* DHT11Sensor = "29099b40-50a8-457a-865f-5c120863cb15";  //Replace this with YOUR deviceID for the DHT11 sensor

//Used Pins
const int relayPin = 14;        //Pin 14
const int servoPin = 21;        //Pin 21
const int MQ2pin = A2;          //Middle Maker Port
const int dht11Pin = 4;         //Left side Maker Port
const int buzzerPin = 12;       //Onboard buzzer

//input sensor
#define DHTTYPE DHT11
DHT dht(dht11Pin, DHTTYPE);

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

void triggerActuator_callback(const char* actuatorDeviceId, const char* actuatorCommand) {
  //actuatorCommand format {"servo":90}
  Serial.print("Main received callback : ");
  Serial.print(actuatorDeviceId);
  Serial.print(" : ");
  Serial.println(actuatorCommand);

  String errorMsg = "";

  JSONVar commandObjct = JSON.parse(actuatorCommand);
  JSONVar keys = commandObjct.keys();

  if (String(actuatorDeviceId) == ServoMotor) {
    //{"servo":90}
    String key = "";
    JSONVar commandValue = "";
    for (int i = 0; i < keys.length(); i++) {
      key = (const char*)keys[i];
      commandValue = commandObjct[keys[i]];
    }
    Serial.print("Key : ");
    Serial.println(key.c_str());
    Serial.print("value : ");
    Serial.println(commandValue);

    int angle = (int)commandValue;
    Myservo.write(angle);
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, errorMsg.c_str(), true);  //publish actuator status
  } else {
    Serial.print(" No actuator found : ");
    Serial.println(actuatorDeviceId);
    errorMsg = "No actuator found";
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, errorMsg.c_str(), false);  //publish actuator status
  }

  if (String(actuatorDeviceId) == Relay) {
    //{"LEDLight":false}
    String key = "";
    bool commandValue = "";
    for (int i = 0; i < keys.length(); i++) {
      key = (const char*)keys[i];
      commandValue = (bool)commandObjct[keys[i]];
      Serial.print("Key : ");
      Serial.println(key.c_str());
      Serial.print("value : ");
      Serial.println(commandValue);
    }

    if (commandValue == true) {
      Serial.println("Relay ON");
      digitalWrite(relayPin, true);
    } else {
      Serial.println("Relay OFF");
      digitalWrite(relayPin, false);
    }

    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, true);
    //Sample publish actuator fail status
    //errorMsg = "LED unable to light up.";
    //voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, errorMsg.c_str(), false);
  }
}

void setup() {

  setup_wifi();
  voneClient.setup();
  voneClient.registerActuatorCallback(triggerActuator_callback);
  Serial.println("Gas sensor warming up!");
  delay(20000);  // allow the MQ-2 to warm up

  //sensor
  dht.begin();

  //actuator
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Myservo.attach(servoPin);
  Myservo.write(0);
}

void loop() {
  if (gasValue > 2000) {
    tone(buzzerPin, 1000);
  } else {
    noTone(buzzerPin);
  }

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "Sensor Fail";
    voneClient.publishDeviceStatusEvent(MQ2sensor, true);
    voneClient.publishDeviceStatusEvent(DHT11Sensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemtry data
    gasValue = analogRead(MQ2pin);
    voneClient.publishTelemetryData(MQ2sensor, "Gas detector", gasValue);

    //Publish telemetry data 2
    float h = dht.readHumidity();
    int t = dht.readTemperature();

    JSONVar payloadObject;
    payloadObject["Humidity"] = h;
    payloadObject["Temperature"] = t;
    voneClient.publishTelemetryData(DHT11Sensor, payloadObject);
  }
}
