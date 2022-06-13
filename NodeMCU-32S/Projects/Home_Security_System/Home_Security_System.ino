/*
  ESP32 publish telemetry data to VOne Cloud (Home Security System)
*/

#include "VOneMqttClient.h"

//define device id
const char* DigitalInput = "8c08337d-5244-46eb-a84e-e6ce2f798c1a";  //Insert
const char* LEDLight = "1cd90ad1-0128-4bcc-b365-a6e29023e896";      //led light
const char* PIRsensor = "123914bb-debc-4490-b673-470b4bc73c44";     //pir sensor

//Used Pins
const int buttonPin = 23;
const int ledPin = 33;
const int motionSensor = 22;
bool ledflag = 0;

//input sensor
// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean PIRvalue = false;

// Checks if motion was detected, sets LED HIGH and starts a timer
void IRAM_ATTR detectsMovement() {
  PIRvalue = true;
  lastTrigger = millis();
}

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

  if (String(actuatorDeviceId) == LEDLight)
  {
    //{"LEDLight":false}
    String key = "";
    bool commandValue = "";
    for (int i = 0; i < keys.length(); i++) {
      key = (const char* )keys[i];
      commandValue = (bool)commandObjct[keys[i]];
      Serial.print("Key : ");
      Serial.println(key.c_str());
      Serial.print("value : ");
      Serial.println(commandValue);
    }

    if (commandValue == true) {
      Serial.println("LED ON");
      digitalWrite(ledPin, true);
    }
    else {
      Serial.println("LED OFF");
      digitalWrite(ledPin, false);
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

  //sensor
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(motionSensor, INPUT);
  digitalWrite(ledPin, LOW);

  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "PIRsensor Fail";
    voneClient.publishDeviceStatusEvent(DigitalInput, true);
    voneClient.publishDeviceStatusEvent(PIRsensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemetry data
    int digitalInputValue = digitalRead(buttonPin);
    int PIRvalue = digitalRead(motionSensor);
    if (digitalInputValue == LOW)
    {
      digitalInputValue = 1;
      digitalWrite(ledPin, HIGH);
    }
    else
    {
      digitalInputValue = 0;
    }

    voneClient.publishTelemetryData(DigitalInput, "Button1", digitalInputValue);

    if (PIRvalue == HIGH)
    {
      digitalWrite(ledPin, HIGH);
    }
    delay(1000);

    voneClient.publishTelemetryData(PIRsensor, "Motion", PIRvalue);

  }
}
