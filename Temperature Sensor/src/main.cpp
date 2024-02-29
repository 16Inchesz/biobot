#include <Arduino.h>
#include "Grove_Temperature_And_Humidity_Sensor.h"

#define DHTTYPE 11
#define DHTPIN 2

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float temp_hum_value[2] = {0};

  //use vlaues for node-red later
  if (!dht.readTempAndHumidity(temp_hum_value)){
    Serial.print("Humidity: ");
    Serial.print(temp_hum_value[0]);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temp_hum_value[1]);
    Serial.println(" *C");
  } else {
    Serial.println("ERROR: Failed to get temperature and humidity.");
  }

  //convert to milli/timer during integration
  delay(2000);
}
