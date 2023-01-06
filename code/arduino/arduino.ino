#include <Arduino.h>
#include <Wire.h>                     // Instantiate the Wire library
#include <TFLI2C.h>                   // TFLuna-I2C Library v.0.1.1
#include <Q2HX711.h>
#include <Servo.h>

TFLI2C tflI2C;                        // Initializing tf-lidar
Servo myservo1;                       // Initializing gripper servo object
Servo myservo2;                       // Initializing rotating servo object

const byte hx711_1_data_pin = 4;
const byte hx711_2_data_pin = 3;
const byte hx711_1_clock_pin = 2;
const byte hx711_2_clock_pin = 5;
const byte buzzer_pin = 7;
int16_t tfDist;                                   // Distance in centimeters
int16_t tfAddr = TFL_DEF_ADR;                     // Use this default I2C address
int8_t lidar_min_dist = 10;                       // 10cm distance from lidar to fruit to start gripping
int close_gripper = 180;
int start_rotate = 180;            // Variable to store closing of gripper
int open_gripper = 0;
int reverse_rotate = 0;                            // Variable to store opening of gripper
int stop_gripper = 90;
int stop_rotate = 90;               // Variable to store stopping of gripper
bool gripper_stopped = 0;                         // 0 -> gripper not stopped; 1 -> gripper stopped
bool gripper_rotated = 0;                         // 0 -> gripper not rotated; 1 -> gripper rotated
bool gripper_disengaged = 0;                      // 0 -> gripper engaged; 1 -> gripper disengaged
uint32_t lidar_d,                                 // Variable to store lidar distance in cm
         load_cell_1, load_cell_2;                // Variables to store load cell ADC readings
uint32_t load_cell_1_no_load = 83700;             // No load load cell reading **(Needs callibration)  **    84040 constant
uint32_t load_cell_2_no_load = 82300;             // No load load cell reading **(Needs callibration)  **    82800 constant
char data;

Q2HX711 hx711_1(hx711_1_data_pin, hx711_1_clock_pin);
Q2HX711 hx711_2(hx711_2_data_pin, hx711_2_clock_pin);

void engage_gripper(void);
void rotate_gripper(void);
void engage_hx711(void);
void engage_lidar(void);
void disengage_gripper(void);


void setup() {
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, HIGH);                   // Use buzzer to debug, for now indicates initialization of setup code
  myservo1.attach(10);
//  myservo1.write(stop_gripper);                     // Setting gripper servo to stop initially
  myservo2.attach(8);
  myservo2.write(stop_rotate);                      // Setting rotating servo to stop initially
  Serial.begin(115200);                             // Initalize serial port
  Wire.begin();                                     // Initalize Wire library
  delay(500);                                       // Giving 500ms delay to give arduino time to set everything up
  digitalWrite(buzzer_pin, LOW);
}


//void loop() {
//  if (Serial.available() > 0) {                     // Always send one character at a time from rpi
//    data = Serial.read();                           // Reads single character from serial port
////        Serial.println(data);                     // Debug code
//
//    switch (data) {
//      case 'O':                                     // Character sent from rpi to start lidar, change to whichever required
//        engage_lidar();
//        break;
//
//      case 'N':                                     // Character sent from rpi for new orange, re-initialises a few variables
//        gripper_disengaged = 1;
//        gripper_stopped = 0;
//        gripper_rotated = 0;
//
//      case ' ':                                     // Add cases for every new character needed to be read by raspberrypi if necessary
//        break;
//
//    }
//    if (lidar_d != 0) {                             // Reading lidar data and engaging closing of gripper and rotating when orange is below "lidar_min_dist"
//      if (lidar_d <= lidar_min_dist) {
//        engage_hx711();
//        engage_gripper();                           // Gripper is engaged until orange is grasped
//        if (gripper_stopped == 1) {
//          rotate_gripper();                         // Implementing plucking of orange by rotation of gripper
//          delay(500);
//          disengage_gripper();                      // Dropping of orange 500ms after rotating gripper
//        }
//      }
//      else {
////        debug code if necessary
//      }
//    }
//  }
//
//
//  myservo1.write(stop_gripper);                     // Servo stops and waits for serial command from rpi
//  lidar_d = 0;
////  Serial.println("");                             // Printing to serial monitor for rpi to read (only if implementation is necessary)
//
//}

void loop() {
  Serial.println("Waiting for orange...");
  engage_lidar();
  if (lidar_d <= lidar_min_dist) {
    engage_hx711();                             // Reading force feedback data from two end effectors
    engage_gripper();                           // Engaging gripper until orange is grasped
    if (gripper_stopped == 1) {                 // engage_gripper() updates gripper_stopped to 1 after grasping of orange
      //2sec delay before rotate                                            //      Serial.println("2 second delay before rotate started...");
      delay(2000);
      rotate_gripper();                         // Implementing plucking of orange by rotation of gripper
      myservo1.write(stop_gripper);
      //5sec delay before release                                          //      Serial.println("Gripper rotated, 5 second delay before disengage gripper started...");
      delay(5000);
      disengage_gripper();                      // Dropping of orange 5000ms after rotating gripper
      //Gripper released, going to default pos                                          //      Serial.println("Gripper disengaged, going back to default position in 2 seconds...");
      delay(2000);
      myservo2.write(reverse_rotate);
      delay(1000);
      myservo2.write(stop_rotate);
      //2sec delay before checking new orange                                          //      Serial.println("2 second delay before checking for new orange...");
      delay(2000);
    }
  }
}


void engage_gripper(void) {                         // Function to implement grasping of gripper
  gripper_disengaged = 0;
  while (load_cell_1 > load_cell_1_no_load && load_cell_2 > load_cell_2_no_load) {          // Need to cross verify this relation  **
    myservo1.write(close_gripper);                                                          // Loop runs until orange is gripped
    gripper_stopped = 0;
    engage_hx711();
    Serial.println("Gripper engaged");
  }
  myservo1.write(stop_gripper);
  gripper_stopped = 1;                              // Updating gripper_stopped to 1 for main loop logic
  Serial.println("Gripper stopped");

}


void disengage_gripper(void) {                      // Function to implement dropping of orange
  myservo1.write(open_gripper);
  delay(800);                                      // Change according to calculated time  **
  myservo1.write(stop_gripper);
  gripper_disengaged = 1;
}


void rotate_gripper(void) {                         // Function to implement rotation of gripper
  myservo2.write(start_rotate);
  delay(1000);
  myservo2.write(stop_rotate);
  gripper_rotated = 1;
}


void engage_hx711(void) {                           // Function to collect hx711 raw data
  load_cell_1 = hx711_1.read() / 100.0;
  //  Serial.println(load_cell_1);                    // Debug code
  load_cell_2 = hx711_2.read() / 100.0;
  //  Serial.println(load_cell_1);                    // Debug code
}


void engage_lidar(void) {                           // Function to get distance of orange
  if (tflI2C.getData(tfDist, tfAddr)) {
    lidar_d = tfDist;
    //    Serial.println(String(tfDist) + " cm");        //+ String(tfDist / 2.54) + " inches");          // Debug code
  }
}
