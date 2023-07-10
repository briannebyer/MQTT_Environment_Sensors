// libraries required
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
// for humidity
#include <DHT.h>
#define DHT11_PIN 2
DHT DHT(DHT11_PIN,DHT11);
// for light
#include <Arduino.h>
// for temperature
#include <OneWire.h>
#include <DallasTemperature.h>

// set wifi
#define WIFI_SSID "-----"
#define WIFI_PASSWORD "-----"

// 
#define MQTT_username "-----"
#define MQTT_password "-----"

// Raspberry Pi Mosquitto MQTT Broker
#define MQTT_HOST IPAddress(-----)
// For a cloud MQTT broker, type the domain name
//#define MQTT_HOST "example.com"
#define MQTT_PORT 1883

// Distance MQTT Topics
#define MQTT_PUB_HUM "esp/humidity"
#define MQTT_PUB_LIG "esp/light"
#define MQTT_PUB_TEM "esp/temperature"

// variables
int count;
// for light
int sensorVal = 0;
const int ANALOG_READ_PIN = A0; 
// for temperature
// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;     

// creating classes
AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;
// for temperature
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

unsigned long previousMillis = 0;   // Stores last time count was published
const long interval = 1000;        // Interval (every second) at which to publish sensor readings

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe("esp/distance", 1);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print(payload);

  // show humidity

  // show light

  // show temperature
}

void setup() {
  Serial.begin(115200);
  Serial.println();
   // humidity
  DHT.begin();
  // temperature
  sensors.begin();
  
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.setCredentials(MQTT_username, MQTT_password);
  
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  // mqttClient.onPublish(onMqttPublish);
   mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  mqttClient.setCredentials("-----", "-----");
  connectToWifi();
}
