/*
  ESP32 publish telemetry data to VOne Cloud (Voting System)
*/

#include "VOneMqttClient.h"

//define device id
const char* DigitalInput = "c75d02a6-4992-400d-83b4-1799cebf8007";//digital input
const char* DigitalInput2 = "18dbd736-322d-473b-925d-3d88de9b4b38";//digital input 2

//Used Pins
const int buttonPin = 22;
const int buttonPin2 = 19;
const int ledPin = 23;

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

    voneClient.publishTelemetryData(DigitalInput, "digitalInput1", count1);

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

    voneClient.publishTelemetryData(DigitalInput2, "digitalInput2", count2);
  }
}
