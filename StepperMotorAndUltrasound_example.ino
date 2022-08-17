/*
 * Source code to project:
 * https://youtu.be/Aaega41eIPw
 */

//pins Arduino MKR1000 <-> ULN2003
#define STEPPER_PIN_1 12 //in1
#define STEPPER_PIN_2 11 //in2
#define STEPPER_PIN_3 10 //in3
#define STEPPER_PIN_4 9 //in4

//pins HC-SR04 1 <-> Arduino MKR1000
int trigPinLeft = 3;
int echoPinLeft = 2;

//pins HC-SR04 2 <-> Arduino MKR1000
int trigPinRight = 5;
int echoPinRight = 4;

//stepper motor phases
int stepper_pins[4] = {STEPPER_PIN_1, STEPPER_PIN_2, STEPPER_PIN_3, STEPPER_PIN_4};
const uint8_t phase_pattern[][4] = {
  {1, 0, 0, 0},// full step
  {1, 1, 0, 0},// half step
  {0, 1, 0, 0},// full step
  {0, 1, 1, 0},// half step
  {0, 0, 1, 0},// full step
  {0, 0, 1, 1},// half step
  {0, 0, 0, 1},// full step
  {1, 0, 0, 1}// half step
};

//helper variables
int trigPin;
int echoPin;
String monitorText;
bool direction = 0;
int step_number = 0;
int step_count = 0;
int phase_pattern_length = sizeof(phase_pattern) / sizeof(int);

void setup() {
  //setup steppermotor pins
  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);

  //setup ultrasound sensor pins
  pinMode(trigPinLeft, OUTPUT); // Sets the trigPinLeft as an Output
  pinMode(echoPinLeft, INPUT); // Sets the echoPinLeft as an Input

  pinMode(trigPinRight, OUTPUT); // Sets the trigPinRight as an Output
  pinMode(echoPinRight, INPUT); // Sets the echoPinRight as an Input

  Serial.begin(9600); // Starts the serial communication
}

void start() {
  SetUltrasoundSensor(0);
}

void loop() {
  OneStep(direction);
  delay(2); //delay between steps (2 ms)
  step_count ++;

  //measure distance
  if (step_count == 8) {
    delayMicroseconds(2);

    //get echo delay
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);

    float distance = duration * 0.0343 / 2; // sound speed in air about 0.0343 cm/s

    //write to serial monitor
    Serial.print(monitorText);
    Serial.println(distance);

    // change motor rotation direction when distance < 4 cm
    if (distance < 4 && distance > -1) {
      SetUltrasoundSensor(!direction);
    }
  }

  //for triggering distance measurement 5 times in a second (100 * 2 ms)
  if (step_count == 100) {
    step_count = 0;
  }
}

/*
 * switch ultrasound sensor
 */
void SetUltrasoundSensor(bool d) {

  direction = d;

  if (direction) {
    trigPin = trigPinLeft;
    echoPin = echoPinLeft;
    Serial.println("<----- direction");
    monitorText = "Distance left: ";
  } else {
    trigPin = trigPinRight;
    echoPin = echoPinRight;
    Serial.println("direction ------>");
    monitorText = "Distance right: ";
  }
}

/*
    move stepper motor
*/
void OneStep(bool d) {
  //set 4 motor pins high/low
  for (uint8_t i = 0; i < 4 ; i++) {
    digitalWrite(stepper_pins[i], phase_pattern[step_number][i]);
  }
  
  //set next step phase between 0 to phase_pattern_length
  if (d) {
    step_number = (step_number + 1 < phase_pattern_length) ? step_number + 1 : 0;
  } else {
    step_number = (step_number - 1 > -1) ? step_number - 1 : phase_pattern_length - 1;
  }
}
