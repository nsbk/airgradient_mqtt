/*

Script that reads AirGradient sensors and pushes data to a MQTT server

*/


#include <AirGradient.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h> // ESP8266 WiFi library
#include <PubSubClient.h>// PubSubClient library to initialize MQTT protocol

#include "arduino_config.h"

// Config ----------------------------------------------------------------------

// WiFi config
const char* SSID = SECRET_SSID;
const char* PASSWORD = SECRET_PASS;
const char* MQTT_SERVER = MQTT_SERVER_ADDRESS;

// Hardware options for AirGradient DIY sensor.
const bool HAS_PM = true;
const bool HAS_CO2 = true;
const bool HAS_SHT = true;

// IP address config
#define staticip
#ifdef staticip
IPAddress static_ip(192, 168, 50, IP_LAST_CHUNK);
IPAddress gateway(192, 168, 50, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

// Temperature MQTT Topics
const char* MQTT_PUB_METRICS = MQTT_TOPIC;

// The frequency of measurement updates.
const int UPDATE_FREQUENCY_SECONDS = 30;
// For housekeeping.
long lastUpdate;
int counter = 0;

// Config End ------------------------------------------------------------------


WiFiClient espClient;
PubSubClient client(espClient);
AirGradient ag = AirGradient();
unsigned long last_publish_time = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

void setup_wifi() {

  delay(10000);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  // Set static IP address if configured.
  #ifdef staticip
  WiFi.config(static_ip,gateway,subnet);
  #endif

  // Set WiFi mode to client (without this it may try to act as an AP).
  WiFi.mode(WIFI_STA);

  // Configure Hostname
  if ((DEVICE_ID != NULL) && (DEVICE_ID[0] == '\0')) {
    Serial.printf("No Device ID is Defined, Defaulting to board defaults");
  }
  else {
    wifi_station_set_hostname(DEVICE_ID);
    WiFi.setHostname(DEVICE_ID);
  }
  
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Create a random client ID
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("DEBUG", "hello world");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


// Generate metrics from sensors and publish to the MQTT server
char* GenerateMetrics() {
  StaticJsonDocument<256> doc;
  // Declare a buffer to hold the result
  char output[256];

  Serial.println("Reading PM2...");
  if (HAS_PM) {
    int stat = ag.getPM2_Raw();
    doc["pm02"] = stat;
  }

  Serial.println("Reading CO2...");
  if (HAS_CO2) {
    int stat = ag.getCO2_Raw();
    doc["rco2"] = stat;
  }

  Serial.println("Reading HT...");
  if (HAS_SHT) {
    TMP_RH stat = ag.periodicFetchData();

    doc["atmp"] = stat.t;
    doc["rhum"] = stat.rh;
  }

  serializeJson(doc, output, sizeof(output));
    Serial.println("Publishing stats");
    Serial.println(output);
    client.publish(MQTT_PUB_METRICS, output);

  return output;
}

void setup() {
  pinMode(D2, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  // Enable active sensors.
  if (HAS_PM) ag.PMS_Init();
  if (HAS_CO2) ag.CO2_Init();
  if (HAS_SHT) ag.TMP_RH_Init(0x44);
}

void loop() {
  char* stats;

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  unsigned long now = millis();
  if (now - last_publish_time > UPDATE_FREQUENCY_SECONDS * 1000) {
    last_publish_time = now;
    GenerateMetrics();
  }
}
