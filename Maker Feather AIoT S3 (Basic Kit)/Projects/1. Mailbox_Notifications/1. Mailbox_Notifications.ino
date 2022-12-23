/*
  ESP32 publish telemetry data to VOne Cloud (Mailbox Notifications)
*/

#include "VOneMqttClient.h"

//define device id
const char* InfraredSensor = "756b1939-44ad-4872-8b2a-12dbd45bd9ff";  //Replace this with YOUR deviceID for the infrared sensor

//Used Pins
const int buttonPin = 3;         //Onboard button
const int InfraredPin = 42;      //Right side Maker Port
const int ledPin = 2;            //Onboard LED

int count1 = 0;

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
  pinMode(ledPin, OUTPUT);
  pinMode(InfraredPin, INPUT_PULLUP);
  digitalWrite(ledPin, LOW);
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "PIRsensor Fail";
    voneClient.publishDeviceStatusEvent(InfraredSensor, true);

  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemetry data
    int InfraredValue = !digitalRead(InfraredPin);
    int digitalInputValue = digitalRead(buttonPin);

    if (InfraredValue == HIGH)
    {
      delay (1000);
      count1++;
    }

    if (digitalInputValue == LOW)
    {
      if (count1)
      {
        count1 = 0;
      }
    }

    if (count1)
    {
      digitalWrite(ledPin, HIGH);
    }
    else
    {
      digitalWrite(ledPin, LOW);
    }

    voneClient.publishTelemetryData(InfraredSensor, "Obstacle", count1);
  }
}
