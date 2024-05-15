#include "DHTesp.h"
#include "WiFi.h"
#include "PubSubClient.h"
#include <ArduinoJson.h>

#define MOISTURE_SENSOR_PIN 32
#define MOISTURE_SENSOR_MAX 4095 //dry
#define MOISTURE_SENSOR_MIN 2375 //fully submerged
#define INTERVAL 1000

const char* ssid = "MQTT Test";
const char* password = "tomato45";
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "sensor_data";

WiFiClient espClient;
PubSubClient client(espClient);

int sensor_value = 0;
unsigned long previousMillis = 0;

/// @brief Function that sets up the wifi connection.
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

/// @brief Function to check if the ESP is reconnecting (debugging purposes)
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= INTERVAL){
    previousMillis = currentMillis;

    int sensor_reading = analogRead(MOISTURE_SENSOR_PIN);
    sensor_value = map(sensor_reading, MOISTURE_SENSOR_MAX, MOISTURE_SENSOR_MIN, 0, 100); //mapped from 0 to 100% based on sensor max values.

    StaticJsonDocument<200> doc;
    doc["moisture"] = sensor_value;
    String jsonStr;
    serializeJson(doc, jsonStr);

    client.publish(mqtt_topic, jsonStr.c_str());

    Serial2.println("ON");
    
    //debug
    //Serial.println("hello boss");
  }
}