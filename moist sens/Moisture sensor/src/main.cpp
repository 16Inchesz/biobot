#include <Arduino.h>

#define MOISTURE_SENSOR_PIN A0
#define MOISTURE_SENSOR_MAX 750

/*
  80 ~ 100; fully submerged in liquid
*/

void setup() {
  Serial.begin(9600);
}

int sensor_value = 0;
void loop() {
  int sensor_reading = analogRead(MOISTURE_SENSOR_PIN);
  sensor_value = map(sensor_reading, 0, MOISTURE_SENSOR_MAX, 0, 100); //mapped from 0 to 100% based on sensor max values.
  Serial.print("moisture sensor value: ");
  Serial.print(sensor_value);
  Serial.println("%");

  //convert to millis or timer during integration
  delay(200);
}