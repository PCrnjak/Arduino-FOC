/**
   Utility arduino sketch which finds pole pair number of the motor

   To run it just set the correct pin numbers for the BLDC driver and encoder A and B channel as well as the encoder PPR value.

   The program will rotate your motor a specific amount and check how much it moved, and by doing a simple calculation calculate your pole pair number.
   The pole pair number will be outputted to the serial terminal.

   If the pole pair number is well estimated your motor will start to spin in voltage mode with 2V target.

   If the code calculates negative pole pair number please invert your encoder A and B channel pins or motor connector.

   Try running this code several times to avoid statistical errors.
   > But in general if your motor spins, you have a good pole pairs number.
*/
#include <SimpleFOC.h>

HardwareSerial Serial2(PA3, PA2);
// motor instance
//  BLDCMotor( int phA, int phB, int phC, int pp, int en)
// its not important how many pole pairs do you set, the progam will find it by itself
// If code is not working switch USE THIS : 
//BLDCMotor motor = BLDCMotor(PULS1, PULS2, PULS3, 7);
BLDCMotor motor = BLDCMotor(PULS2, PULS1, PULS3, 7);
//BLDCMotor motor = BLDCMotor(PB8, PB7, PB9, 7, PC13);
//  Encoder(int encA, int encB , int cpr, int index)

// interrupt routine intialisation
void doA() {
  encoder.handleA();
}
void doB() {
  encoder.handleB();
}

void doIndex() {
  encoder.handleIndex();
}

void setup() {

  // initialise encoder hardware
  encoder.init();
  // hardware interrupt enable
  encoder.enableInterrupts(doA, doB, doIndex);
  // link the motor to the sensor
  motor.linkSensor(&encoder);

  // power supply voltage
  motor.voltage_power_supply = 12;

  // set FOC loop to be used
  motor.controller = ControlType::voltage;

  // initialize motor
  motor.init();


  // monitoring port
  Serial2.begin(115200);

  // pole pairs calculation routine
  Serial2.println("Motor pole pair number estimation example");
  Serial2.println("---------------------------------------------\n");

  float pp_search_voltage = 4; // maximum power_supply_voltage/2
  float pp_search_angle = 6 * M_PI; // search electrical angle to turn

  // move motor to the electrical angle 0
  motor.setPhaseVoltage(pp_search_voltage, 0);
  _delay(1000);
  // read the encoder angle
  float angle_begin = encoder.getAngle();
  _delay(50);

  // move the motor slowly to the electrical angle pp_search_angle
  float motor_angle = 0;
  while (motor_angle <= pp_search_angle) {
    motor_angle += 0.01;
    motor.setPhaseVoltage(pp_search_voltage, motor_angle);
  }
  _delay(1000);
  // read the encoder value for 180
  float angle_end = encoder.getAngle();
  _delay(50);
  // turn off the motor
  motor.setPhaseVoltage(0, 0);
  _delay(1000);

  // calculate the pole pair number
  int pp = round((pp_search_angle) / (angle_end - angle_begin));

  Serial2.print("Estimated pole pairs number is: ");
  Serial2.println(pp);
  Serial2.println("Electrical angle / Encoder angle = Pole pairs ");
  Serial2.print(pp_search_angle * 180 / M_PI);
  Serial2.print("/");
  Serial2.print((angle_end - angle_begin) * 180 / M_PI);
  Serial2.print(" = ");
  Serial2.println((pp_search_angle) / (angle_end - angle_begin));
  Serial2.println();


  // a bit of monitoring the result
  if (pp <= 0 ) {
    Serial2.println("Pole pair number cannot be negative");
    Serial2.println(" - Try changing the search_voltage value or motor/encoder configuration.");
    return;
  } else if (pp > 30) {
    Serial2.println("Pole pair number very high, possible error.");
  } else {
    Serial2.println("If pp is estimated well your motor should turn now!");
    Serial2.println(" - If it is not moving try to relaunch the program!");
    Serial2.println(" - You can also try to adjust the target voltage using serial terminal!");
  }


  // set the pole pair number to the motor
  motor.pole_pairs = pp;
  //align encoder and start FOC
  motor.initFOC();
  _delay(1000);

  Serial2.println("\n Motor ready.");
  Serial2.println("Set the target voltage using serial terminal:");
}

// uq voltage
float target_voltage = 2;

void loop() {

  // main FOC algorithm function
  // the faster you run this function the better
  // Arduino UNO loop  ~1kHz
  // Bluepill loop ~10kHz
  motor.loopFOC();

  // Motion control function
  // velocity, position or voltage (defined in motor.controller)
  // this function can be run at much lower frequency than loopFOC() function
  // You can also use motor.move() and set the motor.target in the code
  motor.move(target_voltage);

  // communicate with the user
  serialReceiveUserCommand();
}


// utility function enabling serial communication with the user to set the target values
// this function can be implemented in serialEvent function as well
void serialReceiveUserCommand() {

  // a string to hold incoming data
  static String received_chars;

  while (Serial2.available()) {
    // get the new byte:
    char inChar = (char)Serial2.read();
    // add it to the string buffer:
    received_chars += inChar;
    // end of user input
    if (inChar == '\n') {

      // change the motor target
      target_voltage = received_chars.toFloat();
      Serial2.print("Target voltage: ");
      Serial2.println(target_voltage);

      // reset the command buffer
      received_chars = "";
    }
  }
}
