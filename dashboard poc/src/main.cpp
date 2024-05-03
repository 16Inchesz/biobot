/*ESP and MQTT code pulled from previous communication assignment*/
#include <Arduino.h>
#include "DHTesp.h"
#include "WiFi.h"
#include "PubSubClient.h"
#include <ArduinoJson.h>


#define DHTPIN 19 //changed for the C3-DevKitM-1 model

const char* ssid = "MQTT Test";
const char* password = "tomato45";
const char* mqtt_server = "workshop.line.pm";
const int mqtt_port = 1883;
const char* mqtt_topic = "nr_workshop/climate";

DHTesp dht;
WiFiClient espClient;
PubSubClient client(espClient);

/// @brief Function that sets up the wifi connection.
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

/// @brief Function to check if the ESP is reconnecting (debugging purposes)
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  dht.setup(DHTPIN, DHTesp::DHT22);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  //creation of json object
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["location"] = "Tilburg";
  doc["hw"] = "ESP32+DHT22";

  //serialize json in string format rather than standard serial. (easily transmitted)
  String jsonStr;
  serializeJson(doc, jsonStr);

  client.publish(mqtt_topic, jsonStr.c_str());
}
