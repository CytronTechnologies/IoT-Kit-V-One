/*
  ESP32 publish telemetry data to VOne Cloud (Visitor Counter)
*/

#include "VOneMqttClient.h"
#include <ESP32Servo.h>
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 1
#define DELAYVAL 500

//define device id
const char* InfraredSensor = "756b1939-44ad-4872-8b2a-12dbd45bd9ff";  //Replace with the deviceID of YOUR infrared sensor
const char* ServoMotor = "d33bdf13-4595-4f04-b561-0b743238f25e";      //Replace with the deviceID of YOUR servo motor

//Used Pins
const int InfraredPin1 = 4;        //Left side Maker Port
const int InfraredPin2 = 42;       //Right side Maker Port
const int ledPin = 2;              //Onboard LED
const int buzzer = 12;             //Onboard buzzer
const int servoPin = 21;           //pin 21
const int neoPin = 46;             //onboard Neopixel

unsigned long lastMillis = 0;
volatile int count1 = 0;
int limit = 10;
int count_prev = 0;

Adafruit_NeoPixel pixels(NUMPIXELS, neoPin, NEO_GRB + NEO_KHZ800);

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

  pixels.begin();

  //sensor
  pinMode(ledPin, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(InfraredPin1, INPUT_PULLUP);
  pinMode(InfraredPin2, INPUT_PULLUP);
  digitalWrite(ledPin, LOW);

  //actuator
  Myservo.attach(servoPin);
  Myservo.write(0);
}

void loop() {
  if (count1 >= limit) {
    digitalWrite(ledPin, HIGH);
    pixels.clear(); // Set all pixel colors to 'off'
  }
  else
  {
    digitalWrite(ledPin, LOW);
    
    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
      pixels.setPixelColor(i, pixels.Color(0, 100, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL);
      pixels.setPixelColor(i, pixels.Color(100, 0, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL);
      pixels.setPixelColor(i, pixels.Color(0, 0, 100));
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL); // Pause before next pass through loop
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL); // Pause before next pass through loop
    }
  }

  if ((millis() - lastMillis) > INTERVAL2) {
    lastMillis = millis();
    if (digitalRead(InfraredPin2) == 1)
    {
      count1++;
    }

    if (digitalRead(InfraredPin1) == 1) {
      if (count1 > 0) {
        count1--;
      }
    }
  }

  if (count1 != count_prev)
  {
    tone(buzzer, 500);
    delay(500);
    noTone(buzzer);
    count_prev = count1;
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
