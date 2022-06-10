/*
  ESP32 publish telemetry data to VOne Cloud (Voting System)
*/

#include "VOneMqttClient.h"

//define device id
const char* DigitalInput = "8c08337d-5244-46eb-a84e-e6ce2f798c1a";  //Replace with the deviceID of YOUR first button
const char* DigitalInput2 = "4eb5fe11-35ad-423b-9be9-658c606daffa"; //Replace with the deviceID of YOUR second button

//Used Pins
const int buttonPin = 22;
const int buttonPin2 = 23;
const int ledPin = 32;

int count1 = 0;
int count2 = 0;

//input sensor

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
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "PIRsensor Fail";
    voneClient.publishDeviceStatusEvent(DigitalInput, true);
    voneClient.publishDeviceStatusEvent(DigitalInput2, true);

  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemetry data
    int digitalInputValue = digitalRead(buttonPin);
    int digitalInputValue2 = digitalRead(buttonPin2);

    if (digitalInputValue == HIGH)
    {
      digitalWrite(ledPin, HIGH);
      delay (1000);
      count1++;
      digitalWrite(ledPin, LOW);
    }
    else
    {
      digitalWrite(ledPin, LOW);
    }

    voneClient.publishTelemetryData(DigitalInput, "Button1", count1);

    if (digitalInputValue2 == HIGH)
    {
      digitalWrite(ledPin, HIGH);
      delay (1000);
      count2++;
      digitalWrite(ledPin, LOW);
    }
    else
    {
      digitalWrite(ledPin, LOW);
    }

    voneClient.publishTelemetryData(DigitalInput2, "Button2", count2);
  }
}
