#include "WiFi.h"
#include "PubSubClient.h"

#define RXPIN2 16
#define TXPIN2 17


const char* ssid = "MQTT Test";
const char* password = "tomato45";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

String previousMessage;

int soil = 0;
int prev_soil;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void callback(String topic, byte* message, unsigned int length){
  Serial.print("message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i=0; i<length-1; i++){
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if(topic == "soil_type"){
    if (messageTemp != previousMessage){
      Serial.print("message received: ");
      if (messageTemp == "Dirt"){
        Serial.println("Dirt");
        soil = 1;
      } else if (messageTemp == "Sand") {
        Serial.println("Sand");
        soil = 2;
      } else {
        Serial.println("Nothing to be done");
        soil = 0;
      }
    }
  }
  previousMessage = messageTemp;
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("soil_type");
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
  Serial2.begin(9600, SERIAL_8N1, RXPIN2, TXPIN2);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  if ((soil == 1 || soil == 2) && soil != prev_soil){
    if(soil == 1){
      Serial2.println("1");
    }else{
      Serial2.println("2");
    }
  } else if ((soil == 0) && (soil != prev_soil))  {
    Serial2.println("0");
    prev_soil = 0;
  }

  client.loop();
}
