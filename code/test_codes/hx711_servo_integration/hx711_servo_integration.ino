#include <Q2HX711.h>                              //Download it here: https://electronoobs.com/eng_arduino_hx711.php
#include <Servo.h>
const byte hx711_1_data_pin = 3;
const byte hx711_2_data_pin = 4;
const byte hx711_clock_pin = 2;
uint32_t data;
Q2HX711 hx711_1(hx711_1_data_pin, hx711_clock_pin);
Q2HX711 hx711_2(hx711_2_data_pin, hx711_clock_pin);

Servo myservo;                                    // create servo object to control a servo
                                                  // twelve servo objects can be created on most boards

void setup() {
  Serial.begin(115200);
  myservo.attach(9);
}
void loop() {
  //  Serial.println(hx711.read()/100.0);         //84080 constant value for no load, load cell
  data = hx711.read() / 100.0;
  Serial.println(data);
  if (data > 84000) {
    myservo.write(135);
  }
  else {
    myservo.write(90);
  }
  //  else{
  //      myservo.write(180);
  //  }
  //  Serial.println(hx711_2.read()/100.0);       //84080 constant value for no load, load cell

  //  delay(500);
}
