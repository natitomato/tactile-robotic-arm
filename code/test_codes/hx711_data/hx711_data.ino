#include <Q2HX711.h>                              //Download it here: https://electronoobs.com/eng_arduino_hx711.php

const byte hx711_1_data_pin = 4;
const byte hx711_2_data_pin = 3;
const byte hx711_1_clock_pin = 2;
const byte hx711_2_clock_pin = 5;
uint32_t data_1, data_2;

Q2HX711 hx711_1(hx711_1_data_pin, hx711_1_clock_pin);
Q2HX711 hx711_2(hx711_2_data_pin, hx711_2_clock_pin);


void setup() {
  Serial.begin(115200);
}


void loop() {
  //    Serial.println(hx711.read()/100.0);       //84080 constant value for no load, load cell
  data_1 = hx711_1.read() / 100.0;
  data_2 = hx711_2.read() / 100.0;
  Serial.print(data_1);
  Serial.print("_");
  Serial.println(data_2);
}
