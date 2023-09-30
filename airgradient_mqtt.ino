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
#include "SampleInterval.h"
#include "QualitySample.h"
#include "MQTTConfiguration.h"
#include "Button.h"
//#include "ConfigManager.h"

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
MQTTConfiguration mqttConfiguration = MQTTConfiguration();

QualitySample airQuality = QualitySample();
// CONFIGURATION END

// use another QualitySample as 'previoussample' ?
unsigned long currentMillis = 0;
unsigned long previousOled = 0;
unsigned long previoussendToServer = 0;
unsigned long previousCo2 = 0;
unsigned long previousPm = 0;
unsigned long previousTempHum = 0;
unsigned long previousTVOC = 0;



int displayModeConfig=0;
int lastState = LOW;
int currentState;
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

// Pin connected to AirGradient push button
#define BUTTON_PIN D7

// Get button feedback for a pin. Option to set a maximum time in milliseconds to get input
Button GetButtonInput(int pin, int maxWaitTime = 0)
{
  Button result = Button();
  unsigned long pressedTime2 = 0;
  unsigned long releasedTime2 = 0;

  int shortpresstime = 2500;
  int longpresstime = 2500;

  int lastButtonState = LOW;
  bool isPressing = false;

  int loopStartTime = millis();
  while (!result.SingleClicked && !result.LongPressed)
  {
    // Pull-up resistor means a press will read as LOW - toggle to make it more intuitive
    int buttonState = !digitalRead(pin);

    // Button is pressed
    if (lastButtonState == LOW && buttonState == HIGH) 
    {
      pressedTime2 = millis();
      isPressing = true;
    }

    // Button is released - could be a single click
    else if (lastButtonState == HIGH && buttonState == LOW) 
    {
      isPressing = false;
      releasedTime2 = millis();
      long pressDuration = releasedTime2 - pressedTime2;
      if (pressDuration < shortpresstime) 
      {
        result.SingleClicked = true;
      }
    }

    if (isPressing)
    {
      long pressedDuration = millis() - pressedTime2;
      if (pressedDuration > longpresstime) 
      {
          result.LongPressed = true;
      }
    }
    
    // If a max-wait is configured, break out if we've exceeded that length of time
    if ((maxWaitTime > 0) && (maxWaitTime <= (loopStartTime + millis())))
      break;

    delay(100);
    lastButtonState = buttonState;
  }

  return result;
}

void setup() {
  Serial.begin(115200);
  //Serial.println(DebugMessages::HelloWorld);

  pinMode(BUTTON_PIN, INPUT_PULLUP);  

  // Alert the user that they have a chance to enter config mode
  updateOLED2(
    OLEDStrings::StartupConfigPromptLine1, 
    OLEDStrings::StartupConfigPromptLine2, 
    OLEDStrings::StartupConfigPromptLine3);

  Button pushButton = GetButtonInput(BUTTON_PIN, 4000);
  if (!pushButton.LongPressed && !pushButton.SingleClicked)
  {
    // User has chosen to enter configuration mode
    updateOLED2(
      OLEDStrings::EnteringConfigLine1,
      OLEDStrings::EnteringConfigLine2,
      OLEDStrings::EnteringConfigLine3
    );
  }
    Serial.println("No button detected.");

  u8g2.begin();
  sht.init();
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);
  //u8g2.setDisplayRotation(U8G2_R0);

  EEPROM.begin(512);
  delay(500);

  displayModeConfig = String(EEPROM.read(addr)).toInt();
  if (displayModeConfig > 3) displayModeConfig = 0;

  delay(400);

  // based on the value read from EEPROM, this shows the current config on-screen and will update local global vars to match
  setConfig();
  Serial.println("displayModeConfig: " + String(displayModeConfig));


  // get file storage going
  //Start LittleFS
  // filesystem has to be setup before connectToWifi, as writing MQTT data locally depends on it
  Serial.println("trying to mount the filesystem");
  if(LittleFS.begin()){
    Serial.println("Mounted LittleFS");
    readConfig("/config.json");  
  } else {
    Serial.println("An Error has occurred while mounting LittleFS");
  }

  if (connectWIFI)
  {
     connectToWifi();
  }

  updateOLED2(
    OLEDStrings::StartupMessageLine1, 
    OLEDStrings::StartupMessageLine2, 
    String(ESP.getChipId(), HEX));

  sgp41.begin(Wire);
  ag.CO2_Init();
  ag.PMS_Init();
  ag.TMP_RH_Init(0x44);
}

void loop() 
{
  Serial.println("Start of loop");

  //probably need a better way to check this
  if (connectWIFI){
      mqtt_connect();
    }
  currentMillis = millis();
  updateTVOC();
  updateOLED();
  updateCo2();
  updatePm();
  updateTempHum();
  //sendToServer();
  sendToMQTTServer();
}

// void inConf()
// {

//   // check if ESP8266 has interupts for noting when bits change
//   setConfig();
//   currentState = digitalRead(D7);

  
//   // Button pushButton = GetButtonInput(BUTTON_PIN);

//   // if (pushButton.SingleClicked) Serial.println("single cliekd!");
//   // if (pushButton.LongPressed) Serial.println("long press!");


//   if (currentState){
//     Serial.println(DebugMessages::ConfigButtonPinHigh);
//   } else {
//     Serial.println(DebugMessages::ConfigButtonPinLow);
//   }

//   if(lastState == HIGH && currentState == LOW) {
//     pressedTime = millis();
//   }

//   else if(lastState == LOW && currentState == HIGH) {
//     releasedTime = millis();
//     long pressDuration = releasedTime - pressedTime;
//     if( pressDuration < 1000 ) {
//       displayModeConfig=displayModeConfig+1;
//       if (displayModeConfig>3) displayModeConfig=0;
//     }
//   }

//   if (lastState == LOW && currentState == LOW)
//   {
//      long passedDuration = millis() - pressedTime;
//       if( passedDuration > 4000 ) 
//       {
//         // to do
// //        if (displayModeConfig==4) {
// //          updateOLED2("Saved", "Release", "Button Now");
// //          delay(1000);
// //          updateOLED2("Starting", "CO2", "Calibration");
// //          delay(1000);
// //          Co2Calibration();
// //       } else {
//           updateOLED2(
//             OLEDStrings::ConfigSavedLine1,
//             OLEDStrings::ConfigSavedLine2,
//             OLEDStrings::ConfigSavedLine3);
//           delay(1000);

//           updateOLED2(
//             OLEDStrings::RebootingLine1,
//             OLEDStrings::RebootingLine2,
//             OLEDStrings::RebootingLine3);
//           delay(5000);

//           EEPROM.write(addr, char(displayModeConfig));
//           EEPROM.commit();
//           delay(1000);
//           ESP.restart();
//  //       }
//     }

//   }
//   lastState = currentState;
//   delay(100);
//   inConf();
// }

// todo - config state machine for both OLED readout and reseting wifi
void setConfig() {
  if (displayModeConfig == 0) 
  {
    updateOLED2(
      OLEDStrings::ConfigTempC,
      OLEDStrings::ConfigPMugm3,
      OLEDStrings::ConfigSaveMessage);

      u8g2.setDisplayRotation(U8G2_R0);
      inF = false;
      inUSAQI = false;
  }
  if (displayModeConfig == 1) 
  {
    updateOLED2(
      OLEDStrings::ConfigTempC,
      OLEDStrings::ConfigPMAQI,
      OLEDStrings::ConfigSaveMessage);

      u8g2.setDisplayRotation(U8G2_R0);
      inF = false;
      inUSAQI = true;
  } 
  else if (displayModeConfig == 2) 
  {
    updateOLED2(
      OLEDStrings::ConfigTempF,
      OLEDStrings::ConfigPMugm3,
      OLEDStrings::ConfigSaveMessage);

    
    u8g2.setDisplayRotation(U8G2_R0);
      inF = true;
      inUSAQI = false;
  } 
  else  if (displayModeConfig == 3) {
    updateOLED2(
      OLEDStrings::ConfigTempF,
      OLEDStrings::ConfigPMAQI,
      OLEDStrings::ConfigSaveMessage);
    
    u8g2.setDisplayRotation(U8G2_R0);
       inF = true;
      inUSAQI = true;
  }



  // to do
  // if (displayModeConfig == 8) {
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

    compensationT = static_cast<uint16_t>((airQuality.TemperatureC + 45) * 65535 / 175);
    compensationRh = static_cast<uint16_t>(airQuality.RelativeHumidity * 65535 / 100);

    if (conditioning_s > 0) 
    {
        error = sgp41.executeConditioning(compensationRh, compensationT, srawVoc);
        conditioning_s--;
    } 
    else 
    {
        error = sgp41.measureRawSignals(compensationRh, compensationT, srawVoc, srawNox);
    }

    if (currentMillis - previousTVOC >= SampleInterval::TVOC) 
    {
      previousTVOC += SampleInterval::TVOC;
      airQuality.TVOC = voc_algorithm.process(srawVoc);
      airQuality.NOX = nox_algorithm.process(srawNox);
    }
}

void updateCo2()
{
    if (currentMillis - previousCo2 >= SampleInterval::CO2) 
    {
      previousCo2 += SampleInterval::CO2;
      airQuality.CO2 = ag.getCO2_Raw();
    }
}

void updatePm()
{
    if (currentMillis - previousPm >= SampleInterval::PM) 
    {
      previousPm += SampleInterval::PM;
      airQuality.PM1 = ag.getPM1_Raw();
      airQuality.PM25 = ag.getPM2_Raw();
      airQuality.PM10 = ag.getPM10_Raw();
    }
}

void updateTempHum()
{
    if (currentMillis - previousTempHum >= SampleInterval::TemperatureHumidity) 
    {
      previousTempHum += SampleInterval::TemperatureHumidity;

      if (sht.readSample()) 
      {      
      airQuality.TemperatureC = sht.getTemperature();
      airQuality.RelativeHumidity = sht.getHumidity();
    } 
    else 
    {
        Serial.println(DebugMessages::SensirionSensorReadError);
    }
  }
}

void updateOLED() 
{
  if (currentMillis - previousOled >= SampleInterval::ScreenUpdate) 
  {
    previousOled += SampleInterval::ScreenUpdate;

    String line1;
    String line2;
    String line3;

    if (inUSAQI) 
    {
      line1 = OLEDStrings::AQIAbbreviation + ":" + String(airQuality.GetAQI()) + " " + 
      OLEDStrings::CO2Abbreviation + ":" + String(airQuality.CO2);
    } 
    else 
    {
      line1 = OLEDStrings::PMAbbreviation + ":" + String(airQuality.PM25) +  " " + 
      OLEDStrings::CO2Abbreviation + ":" + String(airQuality.CO2);
    }

    line2 = OLEDStrings::TVOCAbbreviation + ":" + String(airQuality.TVOC) + " " + 
    OLEDStrings::NOXAbbreviation + ":" + String(airQuality.NOX);

    if (inF) 
    {
      line3 = OLEDStrings::TemperatureFAbbreviation + ":" + String(airQuality.TemperatureF()) + " " + 
      OLEDStrings::RelativeHumidityAbbreviation + ":" + String(airQuality.RelativeHumidity) + QualitySample::RelativeHumidityUnits;
    } 
    else 
    {
      line3 = OLEDStrings::TemperatureCAbbreviation + ":" + String(airQuality.TemperatureC) + " " + 
      OLEDStrings::RelativeHumidityAbbreviation + ":" + String(airQuality.RelativeHumidity) + QualitySample::RelativeHumidityUnits;
    }
    
    updateOLED2(line1, line2, line3);
  }
}

void updateOLED2(String ln1, String ln2, String ln3) 
{
  char buf[9];
  u8g2.firstPage();
  u8g2.firstPage();
  do 
  {
    u8g2.setFont(u8g2_font_t0_16_tf);
    u8g2.drawStr(1, 10, String(ln1).c_str());
    u8g2.drawStr(1, 30, String(ln2).c_str());
    u8g2.drawStr(1, 50, String(ln3).c_str());
  } 
    while ( u8g2.nextPage() );
}

void sendToMQTTServer() 
{
  char noxstr[10];
  itoa(airQuality.NOX, noxstr, 10);

  char tvocstr[10];
  itoa(airQuality.TVOC, tvocstr, 10);
  
  char pm01str[10];
  itoa(airQuality.PM1, pm01str, 10);
  
  char pm25str[10];
  itoa(airQuality.PM25, pm25str, 10);
  
  char pm25AQIstr[10];
  itoa(airQuality.GetAQI(), pm25AQIstr, 10);
  
  char pm10str[10];
  itoa(airQuality.PM10, pm10str, 10);
  
  char co2str[10];
  itoa(airQuality.CO2, co2str, 10);
  
  char tempstr[10];
  itoa(airQuality.TemperatureF(), tempstr, 10);
  
  char humstr[10];
  itoa(airQuality.RelativeHumidity, humstr, 10);

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
  
  char pm25AQITopic[12];
  String pm25AQITopicString = "pm25AQI";
  pm25AQITopicString.toCharArray(pm25AQITopic, 12);
  
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
  mqtt_publish(pm25AQITopic, pm25AQIstr);
  mqtt_publish(pm10Topic, pm10str);
  mqtt_publish(co2Topic, co2str);
  mqtt_publish(temperatureTopic, tempstr); 
  mqtt_publish(humidityTopic, humstr);  

  // Check for new messages
  mqtt_client.loop(); 
}

void sendToServer() 
{
   if (currentMillis - previoussendToServer >= SampleInterval::Reporting) 
   {
     previoussendToServer += SampleInterval::Reporting;
      String payload = "{\"wifi\":" + String(WiFi.RSSI())
      + (airQuality.CO2 < 0 ? "" : ", \"rco2\":" + String(airQuality.CO2))
      + (airQuality.PM1 < 0 ? "" : ", \"pm01\":" + String(airQuality.PM1))
      + (airQuality.PM25 < 0 ? "" : ", \"pm02\":" + String(airQuality.PM25))
      + (airQuality.PM10 < 0 ? "" : ", \"pm10\":" + String(airQuality.PM10))
      + (airQuality.TVOC < 0 ? "" : ", \"tvoc_index\":" + String(airQuality.TVOC))
      + (airQuality.NOX < 0 ? "" : ", \"nox_index\":" + String(airQuality.NOX))
      + ", \"atmp\":" + String(airQuality.TemperatureF())
      + (airQuality.RelativeHumidity < 0 ? "" : ", \"rhum\":" + String(airQuality.RelativeHumidity))
      + "}";

      if(WiFi.status()== WL_CONNECTED)
      {
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
      else 
      {
        Serial.println("WiFi Disconnected");
      }
   }
}


//flag for saving data - defaults to false unless AP is setup
bool shouldSaveConfig = false;

//callback function telling us we need to save the config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// Wifi Manager
void connectToWifi() {
  WiFiManager wifiManager;
   
  // Delete previously saved data
  //wifiManager.resetSettings(); 
   
   Serial.println("Setup the WifiManage callback");
  // When data is saved, trigger the callback that can save the custom parameters locally
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // Adds custom parameters for entering MQTT configuration.
  // id/name, placeholder/prompt, default, length
  WiFiManagerParameter custom_mqtt_server("server", "MQTT Server", "", 40);
  WiFiManagerParameter custom_mqtt_user("user", "MQTT Username", "", 40);
  WiFiManagerParameter custom_mqtt_password("password", "MQTT Password", "", 40);
  WiFiManagerParameter custom_mqtt_topic("topic", "MQTT Tp[oc", "", 80);

  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_password);
  wifiManager.addParameter(&custom_mqtt_topic);
   
   Serial.println("Custom parameters for mqtt are set");

   String HOTSPOT = "AG-" + String(ESP.getChipId(), HEX);
   updateOLED2("90s to connect", "to Wifi Hotspot", HOTSPOT);
   wifiManager.setTimeout(90);

   if (!wifiManager.autoConnect((const char * ) HOTSPOT.c_str())) 
   {
     updateOLED2("booting into", "offline mode", "");
     Serial.println("failed to connect and hit timeout");
     delay(6000);
   }


  // when is the callback executed so this goes from false to true?
  if (shouldSaveConfig) 
  {
    Serial.println("Storing the MQTT config in local variables for saving");

    strcpy(mqttConfiguration.Server, custom_mqtt_server.getValue());
    strcpy(mqttConfiguration.User, custom_mqtt_user.getValue());
    strcpy(mqttConfiguration.Password, custom_mqtt_password.getValue());
    strcpy(mqttConfiguration.Topic, custom_mqtt_topic.getValue());

    saveConfig("/config.json");
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
    //Serial.println("MQTT Already connected");
    return;
  }

  Serial.print(F("Connecting to MQTT... "));

  String CID = String(ESP.getChipId(),HEX);

  mqtt_client.setServer(mqttConfiguration.Server,1883);

  uint8_t retries = 3;
  
  // connect will return 0 for not connected   
  while ((ret = mqtt_client.connect((char *)CID.c_str(), mqttConfiguration.User, mqttConfiguration.Password)) == 0) 
  { 
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
  strcpy(mqtt_topic, mqttConfiguration.Topic);
  strcat(mqtt_topic, "/");
  strcat(mqtt_topic, sub_topic);

  // Serial.print("topic: ");
  // Serial.println(mqtt_topic);
  // Serial.print("subtopic: ");
  // Serial.println(sub_topic);
  // Serial.print("payload: ");
  // Serial.println(payload);
      
      // todo - receive value. does this denote failure?
  bool publishResult = mqtt_client.publish(mqtt_topic, payload);

  if (publishResult)
  {
    //Serial.println("publish succeeded");
  }
  else
  {
    Serial.println("publish failed");
  }
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
        strcpy(mqttConfiguration.Server, json["mqtt_server"]); 
        strcpy(mqttConfiguration.User, json["mqtt_user"]);
        strcpy(mqttConfiguration.Password, json["mqtt_password"]);

        // this is temporary. Topic should be a WiFiManager field and get consume as is.
        char mqtt_topic[80];
        
        char mqtt_location[40];
        char mqtt_room[40];
        strcpy(mqtt_location, json["mqtt_location"]);
        strcpy(mqtt_room, json["mqtt_room"]);

        strcpy(mqtt_topic, mqtt_location);
        strcat(mqtt_topic, "/");
        strcat(mqtt_topic, mqtt_room);

        strcpy(mqttConfiguration.Topic, mqtt_topic);

        //strcpy(mqttConfiguration.Topic, json["mqtt_topic"]);
        Serial.println(mqttConfiguration.Server);
        Serial.println(mqttConfiguration.User);
        Serial.println(mqttConfiguration.Topic);
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
  json["mqtt_server"] = mqttConfiguration.Server;
  json["mqtt_user"] = mqttConfiguration.User;
  json["mqtt_password"] = mqttConfiguration.Password;
  json["mqtt_topic"] = mqttConfiguration.Topic;

  File configFile = LittleFS.open(cFilename, "w");
  if (configFile) 
  {
    serializeJson(json, Serial);
    serializeJson(json, configFile);
    configFile.close();      
  } 
  else 
  {
    Serial.println("Failed to open config file for writing.");
  }
}