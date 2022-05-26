#include "VOneMqttClient.h"

VOneMqttClient::VOneMqttClient() 
{	 
  Serial.begin(115200);
  Serial.println("VOneMqttClient constructor");
  mqttclient.setClient(espClient); 
  mqttclient.setServer(MQTT_SERVER, MQTT_PORT);    
}

VOneMqttClient::VOneMqttClient(void (*callbackEvent)(const char*, const char*)) : 
    mqttclient(MQTT_SERVER, MQTT_PORT, callback, espClient)
{
  Serial.begin(115200);
  Serial.println("VOneMqttClient constructor"); 
  _actuatorCallbackEvent = callbackEvent;
}

VOneMqttClient::~VOneMqttClient() {
	
}

void VOneMqttClient::setup() {
	Serial.println("VOneMqttClient Setup"); 
	//configTime(MY_TZ, MY_NTP_SERVER);	
  
  espClient.setInsecure(); 
  
  if (espClient.connect(MQTT_SERVER, MQTT_PORT) == 0)
    Serial.println("WiFiClientSecure connection failed");
  else
    Serial.println("WiFiClientSecure connected");   

  // Configuration of NTP 
  const char* NTPServer = "pool.ntp.org";
  const long  gmtOffset_sec = 28800;
  const int   daylightOffset_sec = 0;
  configTime(gmtOffset_sec, daylightOffset_sec, NTPServer); 

}

void VOneMqttClient::registerActuatorCallback(void (*callbackEvent)(const char*, const char*))
{
  _actuatorCallbackEvent = callbackEvent;
}

void VOneMqttClient::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String result = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    result += (char)payload[i];
  }

  Serial.println();
  Serial.println(result.c_str());
  //Decode the data
  JSONVar actuatorObject = JSON.parse(result.c_str());
   if (JSON.typeof(actuatorObject) == "undefined") {
    Serial.println("Parsing actuatorObject failed!");
   }
   
   if (actuatorObject.hasOwnProperty("message"))
   {      
      JSONVar msgObject = actuatorObject["message"];
      
      if (JSON.typeof(msgObject) == "undefined") 
        Serial.println("Parsing msgObject failed!");
    
      if (msgObject.hasOwnProperty("action"))
      {
          String action = (const char*)msgObject["action"];
          Serial.print("Action : ");
          Serial.println(action);

          if(action == "actuator_command")
          { 
            String command = "";
            String deviceId = "";
             if (msgObject.hasOwnProperty("payload"))
             {
                JSONVar payloadObject = msgObject["payload"];
                deviceId = (const char*) payloadObject["deviceId"];
                command = JSON.stringify(payloadObject["command"]);

                JSONVar commandObjct = payloadObject["command"];
                JSONVar keys = commandObjct.keys();

                String key = "";
                const char* commandValue = "";
                for (int i = 0; i < keys.length(); i++) {
                   key = (const char* )keys[i];
                   commandValue = (const char*)commandObjct[keys[i]];
                }

                Serial.print("deviceId : ");
                Serial.println(deviceId);
                Serial.print("command : ");
                Serial.println(command);
       
                _actuatorCallbackEvent(deviceId.c_str(), command.c_str());
              
             }
          }

          //Sensor deploy.
          if(action == "deploy")
          { 
                  
          }

           //Actuator deploy
          if(action == "actuator_deploy")
          { 
            
          }
       }
   } 
}

String VOneMqttClient::getDateTime() {
	  //read the current time
    //time_t now;                         
    //tm tm; 
    //time(&now);                       
    //localtime_r(&now, &tm);  

    struct tm tm;
    if(!getLocalTime(&tm)){
      Serial.println("Failed to obtain time");
      String currentTime1 = "2022-02-21 16:29:15.615565+08:00";
      return currentTime1;
    }
    //Serial.println(&tm, "%A, %B %d %Y %H:%M:%S");
  
    int monthDay = tm.tm_mday;   
    int currentMonth = tm.tm_mon +1;  
    int currentYear = tm.tm_year +1900;  
    
    //yyyy-MM-dd HH:mm:ss.fffffffK  2022-02-21 16:29:15.615565+08:00
    String currentTime = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay) + " " + String(tm.tm_hour) + ":" + String(tm.tm_min) + ":" + String(tm.tm_sec) + ".000000+08:00";
    //Serial.print("Time now = ");
    //Serial.println(currentTime.c_str());
    return currentTime;
}

void VOneMqttClient::publishDeviceStatusEvent(const char* deviceId, bool status) {
	String eventTemperature = "event/";
    eventTemperature += GATEWAYID;
            
    JSONVar eventObject;
    eventObject["device_id"] = deviceId;
    eventObject["type"] = "device_status";
    eventObject["status"] = status;
    eventObject["payload"] = "{}";
    eventObject["message"] = "{}";
    eventObject["timestamp"] = getDateTime();
    String eventJsonString = JSON.stringify(eventObject);     
    bool result = mqttclient.publish(eventTemperature.c_str(), eventJsonString.c_str());

    Serial.println("Event publish : "); 
    Serial.print(eventTemperature.c_str());
    Serial.print(":");
    Serial.println(eventJsonString); 	
}

void VOneMqttClient::publishDeviceStatusEvent(const char* deviceId, bool status, const char* errorMsg) {
  String eventTemperature = "event/";
    eventTemperature += GATEWAYID;
            
    JSONVar eventObject;
    eventObject["device_id"] = deviceId;
    eventObject["type"] = "device_status";
    eventObject["status"] = status;
    eventObject["payload"] = "{}";

    JSONVar error;
    error["err_type"] = "";
    error["message"] = errorMsg;
    error["node"] = "";
    error["node_type"] = "";
    
    JSONVar errorList;
    errorList[0] = error;
    
    JSONVar errorObject;
    errorObject["error"] = errorList;
    eventObject["message"] = errorObject;
    eventObject["timestamp"] = getDateTime();
    String eventJsonString = JSON.stringify(eventObject);     
    bool result = mqttclient.publish(eventTemperature.c_str(), eventJsonString.c_str());

    Serial.println("Event publish : "); 
    Serial.print(eventTemperature.c_str());
    Serial.print(":");
    Serial.println(eventJsonString);  
}

void VOneMqttClient::publishActuatorStatusEvent(const char* deviceId, const char* originCommand, const char* errorMsg, bool status) {
	String eventTemperature = "event/";
    eventTemperature += GATEWAYID;

    JSONVar commandObjct = JSON.parse(originCommand);
    JSONVar payloadObject;    
    payloadObject["command"] = commandObjct;
             
    JSONVar eventObject;
    eventObject["device_id"] = deviceId;
    eventObject["type"] = "actuator_status";
    eventObject["status"] = status;
    eventObject["payload"] = payloadObject;

    JSONVar error;
    error["err_type"] = "";
    error["message"] = errorMsg;
    error["node"] = "";
    error["node_type"] = "";
    
    JSONVar errorList;
    errorList[0] = error;
    
    JSONVar errorObject;
    errorObject["error"] = errorList;
    eventObject["message"] = errorObject;
  
    eventObject["timestamp"] = getDateTime();
    String eventJsonString = JSON.stringify(eventObject);     
    bool result = mqttclient.publish(eventTemperature.c_str(), eventJsonString.c_str());

    Serial.println("Actuator Event publish : "); 
    Serial.print(eventTemperature.c_str());   
    Serial.print(":");
    Serial.println(eventJsonString); 
}

void VOneMqttClient::publishActuatorStatusEvent(const char* deviceId, const char* originCommand, bool status) {
  String eventTemperature = "event/";
    eventTemperature += GATEWAYID;

    JSONVar commandObjct = JSON.parse(originCommand);
    JSONVar payloadObject;    
    payloadObject["command"] = commandObjct;
             
    JSONVar eventObject;
    eventObject["device_id"] = deviceId;
    eventObject["type"] = "actuator_status";
    eventObject["status"] = status;
    eventObject["payload"] = payloadObject;   
    eventObject["message"] = "{}";
  
    eventObject["timestamp"] = getDateTime();
    String eventJsonString = JSON.stringify(eventObject);     
    bool result = mqttclient.publish(eventTemperature.c_str(), eventJsonString.c_str());

    Serial.println("Actuator Event publish : "); 
    Serial.print(eventTemperature.c_str());   
    Serial.print(":");
    Serial.println(eventJsonString); 
}

void VOneMqttClient::reconnect() {
  // Loop until we're reconnected
  while (!mqttclient.connected()) {
    Serial.print("Attempting MQTT connection...");
       
    String clientId = GATEWAYID;
   
    // Attempt to connect
    if (mqttclient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
       
      mqttclient.setCallback(callback);
      mqttclient.setBufferSize(4096*2);//put large value.  
      
      // and resubscribe
      String subscribeTopic = "rpc/";
      subscribeTopic += GATEWAYID;
      mqttclient.subscribe(subscribeTopic.c_str(), 1);
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool VOneMqttClient::connected() {
	return mqttclient.connected();
}

void VOneMqttClient::loop() {
	mqttclient.loop();
}

void VOneMqttClient::publishTelemetryData(const char* deviceId, JSONVar& payloadObject) {   
    String topic = "telemetry/";
    topic +=  deviceId;
    Serial.print("Topic : ");
    Serial.println(topic.c_str());
   
    JSONVar telemetryObject;    
    telemetryObject["type"] = "telemetry";
    telemetryObject["payload"] = payloadObject;
    telemetryObject["timestamp"] = getDateTime();
  
    String telemetryJsonString = JSON.stringify(telemetryObject);    
    bool result = mqttclient.publish(topic.c_str(), telemetryJsonString.c_str());

    Serial.print("Publish telemetry message: ");
    Serial.print(result);
    Serial.print(":");
    Serial.println(telemetryJsonString);
}

void VOneMqttClient::publishTelemetryData(const char* deviceId, const char* sensorVariableName, const char* data) {
    String topic = "telemetry/";
    topic +=  deviceId;
    Serial.print("Topic : ");
    Serial.println(topic.c_str());
   
    JSONVar telemetryObject;    
    telemetryObject["type"] = "telemetry";

    JSONVar payloadObject;    
    payloadObject[sensorVariableName] = data;  
    telemetryObject["payload"] = payloadObject;
    
    telemetryObject["timestamp"] = getDateTime();
  
    String telemetryJsonString = JSON.stringify(telemetryObject);    
    bool result = mqttclient.publish(topic.c_str(), telemetryJsonString.c_str());

    Serial.print("Publish telemetry message: ");
    Serial.print(result);
    Serial.print(":");
    Serial.println(telemetryJsonString);
}

void VOneMqttClient::publishTelemetryData(const char* deviceId, const char* sensorVariableName, int data) {
    String topic = "telemetry/";
    topic +=  deviceId;
    Serial.print("Topic : ");
    Serial.println(topic.c_str());
   
    JSONVar telemetryObject;    
    telemetryObject["type"] = "telemetry";

    JSONVar payloadObject;    
    payloadObject[sensorVariableName] = data;  
    telemetryObject["payload"] = payloadObject;
    
    telemetryObject["timestamp"] = getDateTime();
  
    String telemetryJsonString = JSON.stringify(telemetryObject);    
    bool result = mqttclient.publish(topic.c_str(), telemetryJsonString.c_str());

    Serial.print("Publish telemetry message: ");
    Serial.print(result);
    Serial.print(":");
    Serial.println(telemetryJsonString); 
}

void VOneMqttClient::publishTelemetryData(const char* deviceId, const char* sensorVariableName, float data) {
 String topic = "telemetry/";
    topic +=  deviceId;
    Serial.print("Topic : ");
    Serial.println(topic.c_str());
   
    JSONVar telemetryObject;    
    telemetryObject["type"] = "telemetry";

    JSONVar payloadObject;    
    payloadObject[sensorVariableName] = data;  
    telemetryObject["payload"] = payloadObject;
    
    telemetryObject["timestamp"] = getDateTime();
  
    String telemetryJsonString = JSON.stringify(telemetryObject);    
    bool result = mqttclient.publish(topic.c_str(), telemetryJsonString.c_str());

    Serial.print("Publish telemetry message: ");
    Serial.print(result);
    Serial.print(":");
    Serial.println(telemetryJsonString);
  
}

void VOneMqttClient::publishTelemetryData(const char* deviceId, const char* sensorVariableName, double data)  {
 String topic = "telemetry/";
    topic +=  deviceId;
    Serial.print("Topic : ");
    Serial.println(topic.c_str());
   
    JSONVar telemetryObject;    
    telemetryObject["type"] = "telemetry";

    JSONVar payloadObject;    
    payloadObject[sensorVariableName] = data;  
    telemetryObject["payload"] = payloadObject;
    
    telemetryObject["timestamp"] = getDateTime();
  
    String telemetryJsonString = JSON.stringify(telemetryObject);    
    bool result = mqttclient.publish(topic.c_str(), telemetryJsonString.c_str());

    Serial.print("Publish telemetry message: ");
    Serial.print(result);
    Serial.print(":");
    Serial.println(telemetryJsonString);   
}

  

