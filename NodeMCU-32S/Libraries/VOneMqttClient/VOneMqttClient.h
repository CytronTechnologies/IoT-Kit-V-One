#ifndef VOneMqttClient_h
#define VOneMqttClient_h
#endif

#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <WiFiClientSecure.h>
#include <time.h>  
#include "vonesetting.h"


static void (*_actuatorCallbackEvent)(const char*, const char*);

class VOneMqttClient 
{
private:	  
  static void callback(char* topic, byte* payload, unsigned int length);
  String getDateTime(); 
 
  WiFiClientSecure espClient;
  PubSubClient mqttclient; 
     
public:  
  VOneMqttClient();
  VOneMqttClient(void (*callbackEvent)(const char*, const char*));
	~VOneMqttClient();	
  
  void reconnect();
	void setup();
	bool connected();
	void loop();	
  
  void publishDeviceStatusEvent(const char* deviceId, bool status); 
  void publishDeviceStatusEvent(const char* deviceId, bool status, const char* errorMsg); 
  
  void publishTelemetryData(const char* deviceId, JSONVar& payloadObject); 
  void publishTelemetryData(const char* deviceId, const char* sensorVariableName, const char* data);
  void publishTelemetryData(const char* deviceId, const char* sensorVariableName, int data);
  void publishTelemetryData(const char* deviceId, const char* sensorVariableName, float data);
  void publishTelemetryData(const char* deviceId, const char* sensorVariableName, double data); 

  void registerActuatorCallback(void (*)(const char*, const char*)); 
  void publishActuatorStatusEvent(const char* deviceId, const char* originCommand, bool status);
  void publishActuatorStatusEvent(const char* deviceId, const char* originCommand, const char* errorMsg, bool status); 
  
};	
