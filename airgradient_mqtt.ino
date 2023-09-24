/*

Important: AirGradient intended this code is only for the DIY PRO PCB Version 3.7 that has a push button mounted.
I'm running this code on the 4.2 version that also has the push button.


This is the code for the AirGradient DIY PRO Air Quality Sensor with an ESP8266 Microcontroller.
It is a high quality sensor showing PM2.5, CO2, Temperature and Humidity on a small display and can send data over Wifi to an MQTT broker.

For build instructions please visit https://www.airgradient.com/diy/

Based on the original code at https://github.com/airgradienthq/arduino/blob/master/examples/DIY_PRO_V4_2/DIY_PRO_V4_2.ino published under MIT License.


This sketch requires installing the following libraries:
  WifiManager by tzapu, tablatronix
    https://github.com/tzapu/WiFiManager
  PubSubClient by Nick O'Leary
    https://pubsubclient.knolleary.net/
    https://github.com/knolleary/pubsubclient
  ArduinoJson by Benoît Blanchon
    https://arduinojson.org/
    https://github.com/bblanchon/ArduinoJson
  ESP8266 and ESP32 OLED driver for SSD1306 displays by ThingPulse, Fabrice Weinberg
    https://github.com/ThingPulse/esp8266-oled-ssd1306

  “U8g2” by oliver tested with version 2.32.15
  "Sensirion I2C SGP41" by Sensation Version 0.1.0
  "Sensirion Gas Index Algorithm" by Sensation Version 3.2.1
  "Arduino-SHT" by Johannes Winkelmann Version 1.2.2


The original AirGradient implementation writes to an API located at http://hw.airgradient.com/
This implementation writes to an MQTT server of your choice.

*/

#include <LittleFS.h>
#include <AirGradient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
//#include <ArduinoOTA.h>
#include <PubSubClient.h>
//#include <Wire.h>

#include <ESP8266HTTPClient.h>
//#include <WiFiClient.h>

#include <EEPROM.h>
#include "SHTSensor.h"

//#include "SGP30.h"
#include <SensirionI2CSgp41.h>
#include <NOxGasIndexAlgorithm.h>
#include <VOCGasIndexAlgorithm.h>
#include <U8g2lib.h>
#include "StringResources.h"

AirGradient ag = AirGradient();
SensirionI2CSgp41 sgp41;
VOCGasIndexAlgorithm voc_algorithm;
NOxGasIndexAlgorithm nox_algorithm;
SHTSensor sht;

// time in seconds needed for NOx conditioning
uint16_t conditioning_s = 10;

// for peristent saving and loading
int addr = 4;
byte value;

// Display bottom right
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Replace above if you have display on top left
//U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);


// CONFIGURATION START

//set to the endpoint you would like to use
String APIROOT = "http://hw.airgradient.com/";

// set to true to switch from Celcius to Fahrenheit
boolean inF = true;

// PM2.5 in US AQI (default ug/m3)
boolean inUSAQI = false;

// Display Position
boolean displayTop = true;

// set to true if you want to connect to wifi. You have 60 seconds to connect. Then it will go into an offline mode.
boolean connectWIFI=true;

WiFiClient client;
PubSubClient mqtt_client(client);

// PubSubClient expects char array
char mqtt_server[40];
char mqtt_user[40];
char mqtt_password[40];
char mqtt_location[40];
char mqtt_room[40];

// CONFIGURATION END


unsigned long currentMillis = 0;

const int oledInterval = 5000;
unsigned long previousOled = 0;

const int sendToServerInterval = 10000;
unsigned long previoussendToServer = 0;

const int tvocInterval = 1000;
unsigned long previousTVOC = 0;
int TVOC = 0;
int NOX = 0;

const int co2Interval = 5000;
unsigned long previousCo2 = 0;
int Co2 = 0;

const int pmInterval = 5000;
unsigned long previousPm = 0;
int pm25 = 0;
int pm01 = 0;
int pm10 = 0;
int pm03PCount = 0;

const int tempHumInterval = 2500;
unsigned long previousTempHum = 0;
float temp = 0;
int hum = 0;

int buttonConfig=0;
int lastState = LOW;
int currentState;
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(DebugMessages::HelloWorld);

  u8g2.begin();
  sht.init();
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);
  //u8g2.setDisplayRotation(U8G2_R0);

  EEPROM.begin(512);
  delay(500);

  buttonConfig = String(EEPROM.read(addr)).toInt();
  if (buttonConfig>3) buttonConfig=0;
  delay(400);
  setConfig();
  Serial.println("buttonConfig: " + String(buttonConfig));
  
  updateOLED2(
    OLEDStrings::STARTUP_CONFIG_PROMPT_LINE1, 
    OLEDStrings::STARTUP_CONFIG_PROMPT_LINE2, 
    OLEDStrings::STARTUP_CONFIG_PROMPT_LINE3);
  
  delay(2000);
  pinMode(D7, INPUT_PULLUP);
  currentState = digitalRead(D7);
  if (currentState == LOW)
  {
    updateOLED2("Entering", "Config Menu", "");
    delay(3000);
    lastState = HIGH;
    setConfig();
    inConf();
  }

  if (connectWIFI)
  {
     connectToWifi();
  }

  updateOLED2("Warming Up", "Serial Number:", String(ESP.getChipId(), HEX));
  sgp41.begin(Wire);
  ag.CO2_Init();
  ag.PMS_Init();
  ag.TMP_RH_Init(0x44);

  // get file storage going
  //Start LittleFS
  // if(LittleFS.begin()){
  //   Serial.println("Mounted LittleFS");
  //   readConfig("/config.json");  
  // } else {
  //   Serial.println("An Error has occurred while mounting LittleFS");
  // }

}

void loop() {

  // probably need a better way to check this
  // if (connectWIFI){
  //     mqtt_connect();
  //   }

  currentMillis = millis();
  updateTVOC();
  updateOLED();
  updateCo2();
  updatePm();
  updateTempHum();
  sendToServer();
  //sendToMQTTServer();
  Serial.println(DebugMessages::HelloWorld);
}

void inConf(){
  setConfig();
  currentState = digitalRead(D7);

  if (currentState){
    Serial.println("currentState: high");
  } else {
    Serial.println("currentState: low");
  }

  if(lastState == HIGH && currentState == LOW) {
    pressedTime = millis();
  }

  else if(lastState == LOW && currentState == HIGH) {
    releasedTime = millis();
    long pressDuration = releasedTime - pressedTime;
    if( pressDuration < 1000 ) {
      buttonConfig=buttonConfig+1;
      if (buttonConfig>3) buttonConfig=0;
    }
  }

  if (lastState == LOW && currentState == LOW){
     long passedDuration = millis() - pressedTime;
      if( passedDuration > 4000 ) {
        // to do
//        if (buttonConfig==4) {
//          updateOLED2("Saved", "Release", "Button Now");
//          delay(1000);
//          updateOLED2("Starting", "CO2", "Calibration");
//          delay(1000);
//          Co2Calibration();
//       } else {
          updateOLED2("Saved", "Release", "Button Now");
          delay(1000);
          updateOLED2("Rebooting", "in", "5 seconds");
          delay(5000);
          EEPROM.write(addr, char(buttonConfig));
          EEPROM.commit();
          delay(1000);
          ESP.restart();
 //       }
    }

  }
  lastState = currentState;
  delay(100);
  inConf();
}


void setConfig() {
  if (buttonConfig == 0) {
    updateOLED2("Temp. in C", "PM in ug/m3", "Long Press Saves");
      u8g2.setDisplayRotation(U8G2_R0);
      inF = false;
      inUSAQI = false;
  }
    if (buttonConfig == 1) {
    updateOLED2("Temp. in C", "PM in US AQI", "Long Press Saves");
      u8g2.setDisplayRotation(U8G2_R0);
      inF = false;
      inUSAQI = true;
  } else if (buttonConfig == 2) {
    updateOLED2("Temp. in F", "PM in ug/m3", "Long Press Saves");
    u8g2.setDisplayRotation(U8G2_R0);
      inF = true;
      inUSAQI = false;
  } else  if (buttonConfig == 3) {
    updateOLED2("Temp. in F", "PM in US AQI", "Long Press Saves");
      u8g2.setDisplayRotation(U8G2_R0);
       inF = true;
      inUSAQI = true;
  }



  // to do
  // if (buttonConfig == 8) {
  //  updateOLED2("CO2", "Manual", "Calibration");
  // }
}

void updateTVOC()
{
 uint16_t error;
    char errorMessage[256];
    uint16_t defaultRh = 0x8000;
    uint16_t defaultT = 0x6666;
    uint16_t srawVoc = 0;
    uint16_t srawNox = 0;
    uint16_t defaultCompenstaionRh = 0x8000;  // in ticks as defined by SGP41
    uint16_t defaultCompenstaionT = 0x6666;   // in ticks as defined by SGP41
    uint16_t compensationRh = 0;              // in ticks as defined by SGP41
    uint16_t compensationT = 0;               // in ticks as defined by SGP41

    delay(1000);

    compensationT = static_cast<uint16_t>((temp + 45) * 65535 / 175);
    compensationRh = static_cast<uint16_t>(hum * 65535 / 100);

    if (conditioning_s > 0) {
        error = sgp41.executeConditioning(compensationRh, compensationT, srawVoc);
        conditioning_s--;
    } else {
        error = sgp41.measureRawSignals(compensationRh, compensationT, srawVoc,
                                        srawNox);
    }

    if (currentMillis - previousTVOC >= tvocInterval) {
      previousTVOC += tvocInterval;
      TVOC = voc_algorithm.process(srawVoc);
      NOX = nox_algorithm.process(srawNox);
      Serial.println(String(TVOC));
    }
}

void updateCo2()
{
    if (currentMillis - previousCo2 >= co2Interval) {
      previousCo2 += co2Interval;
      Co2 = ag.getCO2_Raw();
      Serial.println(String(Co2));
    }
}

void updatePm()
{
    if (currentMillis - previousPm >= pmInterval) {
      previousPm += pmInterval;
      pm01 = ag.getPM1_Raw();
      pm25 = ag.getPM2_Raw();
      pm10 = ag.getPM10_Raw();
      pm03PCount = ag.getPM0_3Count();
      Serial.println(String(pm25));
    }
}

void updateTempHum()
{
    if (currentMillis - previousTempHum >= tempHumInterval) {
      previousTempHum += tempHumInterval;

      if (sht.readSample()) {
      Serial.print("SHT:\n");
      Serial.print("  RH: ");
      Serial.print(sht.getHumidity(), 2);
      Serial.print("\n");
      Serial.print("  T:  ");
      Serial.print(sht.getTemperature(), 2);
      Serial.print("\n");
      temp = sht.getTemperature();
      hum = sht.getHumidity();
  } else {
      Serial.print("Error in readSample()\n");
  }
      Serial.println(String(temp));
    }
}

void updateOLED() {
   if (currentMillis - previousOled >= oledInterval) {
     previousOled += oledInterval;

    String ln3;
    String ln1;

    if (inUSAQI) {
      ln1 = "AQI:" + String(PM_TO_AQI_US(pm25)) +  " CO2:" + String(Co2);
    } else {
      ln1 = "PM:" + String(pm25) +  " CO2:" + String(Co2);
    }

     String ln2 = "TVOC:" + String(TVOC) + " NOX:" + String(NOX);

      if (inF) {
        ln3 = "F:" + String((temp* 9 / 5) + 32) + " H:" + String(hum)+"%";
        } else {
        ln3 = "C:" + String(temp) + " H:" + String(hum)+"%";
       }
     updateOLED2(ln1, ln2, ln3);
   }
}

void updateOLED2(String ln1, String ln2, String ln3) {
  char buf[9];
  u8g2.firstPage();
  u8g2.firstPage();
  do {
  u8g2.setFont(u8g2_font_t0_16_tf);
  u8g2.drawStr(1, 10, String(ln1).c_str());
  u8g2.drawStr(1, 30, String(ln2).c_str());
  u8g2.drawStr(1, 50, String(ln3).c_str());
    } while ( u8g2.nextPage() );
}

void sendToMQTTServer() {
 char noxstr[4];
  itoa(NOX, noxstr, 4);
  char tvocstr[4];
  itoa(TVOC, tvocstr, 4);
  char pm01str[4];
  itoa(pm01, pm01str, 4);
  char pm25str[4];
  itoa(pm25, pm25str, 4);
  char pm10str[4];
  itoa(pm10, pm10str, 4);
  char pm03str[4];
  itoa(pm03PCount, pm03str, 4);
  char co2str[4];
  itoa(Co2, co2str, 4);
  char tempstr[4];
  itoa(temp, tempstr, 4);
  char humstr[4];
  itoa(hum, humstr, 4);

  char noxTopic[10];
  String noxTopicString = "nox_index";
  noxTopicString.toCharArray(noxTopic, 10);
  
  char tvocTopic[12];
  String tvocTopicString = "tvoc_index";
  tvocTopicString.toCharArray(tvocTopic, 12);
  
  char pm01Topic[12];
  String pm01TopicString = "pm01";
  pm01TopicString.toCharArray(pm01Topic, 12);
  
  char pm25Topic[12];
  String pm25TopicString = "pm25";
  pm25TopicString.toCharArray(pm25Topic, 12);
  
  // char pm25AQITopic[12];
  String pm25AQITopicString = "pm25AQI";
  // pm25AQITopicString.toCharArray(pm25AQITopic, 12);
  
  char pm10Topic[12];
  String pm10TopicString = "pm10";
  pm10TopicString.toCharArray(pm10Topic, 12);
  
  char pm03Topic[12];
  String pm03TopicString = "pm03";
  pm03TopicString.toCharArray(pm03Topic, 12);
  
  char co2Topic[12];
  String co2TopicString = "co2";
  co2TopicString.toCharArray(co2Topic, 12);
  
  char temperatureTopic[12];
  String tempTopicString = "temperature";
  tempTopicString.toCharArray(temperatureTopic, 12);
  
  char humidityTopic[12];
  String humTopicString = "humidity";
  humTopicString.toCharArray(humidityTopic, 12);
  
  
  mqtt_publish(noxTopic, noxstr);
  mqtt_publish(tvocTopic, tvocstr);
  mqtt_publish(pm01Topic, pm01str);
  mqtt_publish(pm25Topic, pm25str);
  //mqtt_publish(pm25AQITopic, String(PM_TO_AQI_US(pm25)));
  mqtt_publish(pm10Topic, pm10str);
  mqtt_publish(pm03Topic, pm03str);
  mqtt_publish(co2Topic, co2str);
  mqtt_publish(temperatureTopic, tempstr); 
  mqtt_publish(humidityTopic, humstr);  

  // Check for new messages
  mqtt_client.loop(); 
}

void sendToServer() {
   if (currentMillis - previoussendToServer >= sendToServerInterval) {
     previoussendToServer += sendToServerInterval;
      String payload = "{\"wifi\":" + String(WiFi.RSSI())
      + (Co2 < 0 ? "" : ", \"rco2\":" + String(Co2))
      + (pm01 < 0 ? "" : ", \"pm01\":" + String(pm01))
      + (pm25 < 0 ? "" : ", \"pm02\":" + String(pm25))
      + (pm10 < 0 ? "" : ", \"pm10\":" + String(pm10))
      + (pm03PCount < 0 ? "" : ", \"pm003_count\":" + String(pm03PCount))
      + (TVOC < 0 ? "" : ", \"tvoc_index\":" + String(TVOC))
      + (NOX < 0 ? "" : ", \"nox_index\":" + String(NOX))
      + ", \"atmp\":" + String(temp)
      + (hum < 0 ? "" : ", \"rhum\":" + String(hum))
      + "}";

      if(WiFi.status()== WL_CONNECTED){
        Serial.println(payload);
        String POSTURL = APIROOT + "sensors/airgradient:" + String(ESP.getChipId(), HEX) + "/measures";
        Serial.println(POSTURL);
        WiFiClient client;
        HTTPClient http;
        http.begin(client, POSTURL);
        http.addHeader("content-type", "application/json");
        int httpCode = http.POST(payload);
        String response = http.getString();
        Serial.println(httpCode);
        Serial.println(response);
        http.end();
      }
      else {
        Serial.println("WiFi Disconnected");
      }
   }
}

// Wifi Manager
 void connectToWifi() {
   WiFiManager wifiManager;
   //WiFi.disconnect(); //to delete previous saved hotspot
   String HOTSPOT = "AirGradient-" + String(ESP.getChipId(), HEX);
   updateOLED2("90s to connect", "to Wifi Hotspot", HOTSPOT);
   wifiManager.setTimeout(90);

   if (!wifiManager.autoConnect((const char * ) HOTSPOT.c_str())) {
     updateOLED2("booting into", "offline mode", "");
     Serial.println("failed to connect and hit timeout");
     delay(6000);
   }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void mqtt_connect() 
{
  int8_t ret;

  // Stop if already connected.
  //if (mqtt_client.connected() == MQTT_CONNECTED) {
  if (mqtt_client.state() == MQTT_CONNECTED) {
    Serial.println("MQTT Already connected");
    return;
  }

  Serial.print(F("Connecting to MQTT... "));

  String CID = String(ESP.getChipId(),HEX);

  mqtt_client.setServer(mqtt_server,1883);

  uint8_t retries = 3;
  while ((ret = mqtt_client.connect((char *)CID.c_str(), mqtt_user, mqtt_password)) == 0) { // connect will return 0 for not connected   
    //showTextRectangle("MQTT", "Failed "+String(mqtt_client.state()),true);
    Serial.println(mqtt_client.state());
    Serial.println(F("Retrying MQTT connection in 5 seconds..."));
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println(F("MQTT Connected!"));
}

void mqtt_publish(char *sub_topic, const char *payload) 
{
  char mqtt_topic[80];
  strcpy(mqtt_topic, mqtt_location);
  strcat(mqtt_topic, "/");
  strcat(mqtt_topic, mqtt_room);
  strcat(mqtt_topic, "/");
  strcat(mqtt_topic, sub_topic);
      
  mqtt_client.publish(mqtt_topic, payload);
}

void readConfig(String cFilename)
{
  if (LittleFS.exists(cFilename)) 
  {
    Serial.println("Reading config file");
    File configFile = LittleFS.open(cFilename,"r");
    if (configFile) 
    {
      String configData;
      while (configFile.available())
      {
        configData += char(configFile.read());
      }
      DynamicJsonDocument json(1024);
      auto deserializeError = deserializeJson(json, configData);
      serializeJson(json, Serial);
      if (!deserializeError) 
      {
        Serial.println("\nParsed json");
        strcpy(mqtt_server, json["mqtt_server"]); 
        strcpy(mqtt_user, json["mqtt_user"]);
        strcpy(mqtt_password, json["mqtt_password"]);
        strcpy(mqtt_location, json["mqtt_location"]);
        strcpy(mqtt_room, json["mqtt_room"]);
      }
      else 
      {
        Serial.println("\nFailed to parse json");
      }
      configFile.close();
    } else 
    {
      Serial.println("Failed to open config file");
    }
  } else 
  {
    Serial.println("No config file to read");
  }
}

void saveConfig(String cFilename) 
{
  Serial.println("Saving config");
  
  DynamicJsonDocument json(1024);
  json["mqtt_server"] = mqtt_server;
  json["mqtt_user"] = mqtt_user;
  json["mqtt_password"] = mqtt_password;
  json["mqtt_location"] = mqtt_location;
  json["mqtt_room"] = mqtt_room;
  File configFile = LittleFS.open(cFilename,"w");
  if (configFile) {
    serializeJson(json, Serial);
    serializeJson(json, configFile);
    configFile.close();      
  } else {
    Serial.println("Failed to open config file for writing");
  }
}

// Calculate PM2.5 US AQI
int PM_TO_AQI_US(int pm02) {
  if (pm02 <= 12.0) return ((50 - 0) / (12.0 - .0) * (pm02 - .0) + 0);
  else if (pm02 <= 35.4) return ((100 - 50) / (35.4 - 12.0) * (pm02 - 12.0) + 50);
  else if (pm02 <= 55.4) return ((150 - 100) / (55.4 - 35.4) * (pm02 - 35.4) + 100);
  else if (pm02 <= 150.4) return ((200 - 150) / (150.4 - 55.4) * (pm02 - 55.4) + 150);
  else if (pm02 <= 250.4) return ((300 - 200) / (250.4 - 150.4) * (pm02 - 150.4) + 200);
  else if (pm02 <= 350.4) return ((400 - 300) / (350.4 - 250.4) * (pm02 - 250.4) + 300);
  else if (pm02 <= 500.4) return ((500 - 400) / (500.4 - 350.4) * (pm02 - 350.4) + 400);
  else return 500;
};