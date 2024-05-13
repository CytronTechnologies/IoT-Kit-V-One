/*
  ESP32 publish telemetry data to VOne Cloud (Smart Bedroom)
*/

#include "VOneMqttClient.h"
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 1
#define DELAYVAL 500

//define device id
const char* Relay = "804eb6ba-09c9-43c5-a58e-37a81d619eb0";      //Replace this with YOUR deviceID for the relay
const char* LEDLight1 = "1cd90ad1-0128-4bcc-b365-a6e29023e896";  //Replace this with YOUR deviceID for the first LED
const char* LEDLight2 = "0f127883-bc5f-424e-b565-f22353424e5c";  //Replace this with YOUR deviceID for the second LED
const char* LEDLight3 = "fca553e1-095f-4340-be62-77fbc579897e";  //Replace this with YOUR deviceID for the third LED

//Used Pins
const int relayPin = 14;       //Pin 14
const int ledPin1 = 21;        //Pin 21
const int ledPin2 = 47;        //Pin 47
const int ledPin3 = 48;        //Pin 48
const int buttonPin = 3;       //Onboard button
const int neoPin = 46;         //onboard Neopixel

int buttonState = 0;
int lastButtonState = 0;
int colorIndex = 0;
int colorValues[4][4] = { { 0, 0, 0 }, { 0, 0, 120 }, { 120, 0, 0 }, { 0, 120, 0 } };

Adafruit_NeoPixel pixels(NUMPIXELS, neoPin, NEO_GRB + NEO_KHZ800);

//Create an insance of VOneMqttClient
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
  }

  if (String(actuatorDeviceId) == LEDLight1) {
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
      Serial.println("LED ON");
      digitalWrite(ledPin1, true);
    } else {
      Serial.println("LED OFF");
      digitalWrite(ledPin1, false);
    }
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, true);
  }

  if (String(actuatorDeviceId) == LEDLight2) {
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
      Serial.println("LED ON");
      digitalWrite(ledPin2, true);
    } else {
      Serial.println("LED OFF");
      digitalWrite(ledPin2, false);
    }
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, true);
  }
  if (String(actuatorDeviceId) == LEDLight3) {
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
      Serial.println("LED ON");
      digitalWrite(ledPin3, true);
    } else {
      Serial.println("LED OFF");
      digitalWrite(ledPin3, false);
    }
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, true);
  }
}

void setup() {
  setup_wifi();
  voneClient.setup();
  voneClient.registerActuatorCallback(triggerActuator_callback);

  pixels.begin();
  pixels.show();

  //actuator
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);
    if (buttonState != lastButtonState) {
      if (buttonState == HIGH) {
        colorIndex++;
        if (colorIndex == 4) {
          colorIndex = 0;
        }
        pixels.setPixelColor(0, pixels.Color(colorValues[colorIndex][0], colorValues[colorIndex][1], colorValues[colorIndex][2], colorValues[colorIndex][3]));
        pixels.show();
      }
    }
    lastButtonState = buttonState;

  if (!voneClient.connected()) {
    voneClient.reconnect();
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;
  }
}
