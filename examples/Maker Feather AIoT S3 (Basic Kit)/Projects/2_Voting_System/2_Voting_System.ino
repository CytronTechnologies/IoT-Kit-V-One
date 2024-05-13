/*
  ESP32 publish telemetry data to VOne Cloud (Voting System)
*/

#include "VOneMqttClient.h"

//define device id
const char* DigitalInput = "89ad6204-e772-40ba-8b01-6360cc3c1c46";  //Replace with the deviceID of YOUR first push button
const char* DigitalInput2 = "97019af5-1403-4859-9057-39c5746caa73"; //Replace with the deviceID of YOUR second push button

//Used Pins
const int buttonPin = 14;        //Pin 14
const int buttonPin2 = 21;       //Pin 21
const int buzzer = 12;           //Onboard buzzer
const int ledPin = 2;            //Onboard LED

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
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
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

    if (digitalInputValue == LOW)
    {
      tone(buzzer, 1000);
      digitalWrite(ledPin, HIGH);
      delay (1000);
      count1++;
      noTone(buzzer);
      digitalWrite(ledPin, LOW);
    }
    else
    {
      digitalWrite(ledPin, LOW);
    }

    voneClient.publishTelemetryData(DigitalInput, "Button1", count1);

    if (digitalInputValue2 == LOW)
    {
      tone(buzzer, 1000);
      digitalWrite(ledPin, HIGH);
      delay (1000);
      count2++;
      noTone(buzzer);
      digitalWrite(ledPin, LOW);
    }
    else
    {
      digitalWrite(ledPin, LOW);
    }

    voneClient.publishTelemetryData(DigitalInput2, "Button2", count2);
  }
}
