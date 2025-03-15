#include <Arduino.h>

#define EYE_SERVO_PWM 3

#define MOTOR_L_INT1 9
#define MOTOR_L_INT2 10
#define MOTOR_R_INT1 5
#define MOTOR_R_INT2 6
#define DRIVE_MOTORS_EEP 2
#define BRUSHES_EEP 4

#define IR_R_L_LED 7
#define IR_R_L_REC  A0
#define IR_R_R_LED 14
#define IR_R_R_REC  A1

#define IR_F_L_LED 15
#define IR_F_L_REC  A2
#define IR_F_R_LED 16
#define IR_F_R_REC  A3

#define IR_TIMES  3
#define IR_THRESHOLD 600
#define IR_THRESHOLD_REAR 600

#define MAX_SPEED 255

int lockout;

#define BUFFER_SIZE 8
char buffer[BUFFER_SIZE];
int bufferPos;

bool cmd_turn;
bool cmd_auto;
bool cmd_brushes;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(MOTOR_L_INT1, OUTPUT);
  pinMode(MOTOR_L_INT2, OUTPUT);
  pinMode(MOTOR_R_INT1, OUTPUT);
  pinMode(MOTOR_R_INT2, OUTPUT);
  pinMode(DRIVE_MOTORS_EEP, OUTPUT);

  pinMode(IR_F_R_LED, OUTPUT);
  pinMode(IR_F_R_REC, INPUT_PULLUP);
  pinMode(IR_F_L_LED, OUTPUT);
  pinMode(IR_F_L_REC, INPUT_PULLUP);

  pinMode(IR_R_R_LED, OUTPUT);
  pinMode(IR_R_R_REC, INPUT_PULLUP);
  pinMode(IR_R_L_LED, OUTPUT);
  pinMode(IR_R_L_REC, INPUT_PULLUP);

  pinMode(BRUSHES_EEP, OUTPUT);

  Serial.begin(115200);
  Serial.println("");

  bufferPos = 0;

  digitalWrite(DRIVE_MOTORS_EEP, HIGH);
  digitalWrite(BRUSHES_EEP, LOW);
  lockout = 0;

  cmd_turn = false;
  cmd_auto = true;
  cmd_brushes = false;
}

int readIR(int led, int rec) {
  int ambient = 0;
  int result = 0;

  for (int i=0; i < IR_TIMES; i++) {
    digitalWrite(led, LOW);
    delay(5);
    ambient = analogRead(rec);
    digitalWrite(led, HIGH);
    result += ambient - analogRead(rec);
    digitalWrite(led, LOW);
  }

  // Return avg
  return result / IR_TIMES;
}

// the loop function runs over and over again forever
void loop() {
  int leftIR = readIR(IR_F_L_LED, IR_F_L_REC);
  int rightIR = readIR(IR_F_R_LED, IR_F_R_REC);

  int rearLeftIR = readIR(IR_R_L_LED, IR_R_L_REC);
  int rearRightIR = readIR(IR_R_R_LED, IR_R_R_REC);

  if (cmd_turn) {
    digitalWrite(MOTOR_L_INT1, LOW);
    analogWrite(MOTOR_L_INT2, 128);

    // Forward right motor
    analogWrite(MOTOR_R_INT1, 128);
    digitalWrite(MOTOR_R_INT2, LOW);

    Serial.println("Turn Left");

    cmd_turn = false;
    lockout = 100;
  }

  if (cmd_auto && lockout == 0) {
    if (rearLeftIR <= IR_THRESHOLD_REAR || rearRightIR <= IR_THRESHOLD_REAR) {
      // Emergency stop!
      // Reverse Left
      digitalWrite(MOTOR_L_INT1, LOW);
      analogWrite(MOTOR_L_INT2, 64);

      // Reverse Right
      digitalWrite(MOTOR_R_INT1, LOW);
      analogWrite(MOTOR_R_INT2, 96);

      lockout = 250;

      Serial.println("EMERGENCY STOP");
    } else if (leftIR > IR_THRESHOLD && rightIR > IR_THRESHOLD) {
      // Forward
      // Forward left motor
      analogWrite(MOTOR_L_INT1, MAX_SPEED);
      digitalWrite(MOTOR_L_INT2, LOW);

      // Forward right motor
      analogWrite(MOTOR_R_INT1, MAX_SPEED);
      digitalWrite(MOTOR_R_INT2, LOW);

      lockout = 1;
    } else if (leftIR > IR_THRESHOLD && rightIR <= IR_THRESHOLD) {
      // Right cliff, turn left
      // Reverse left motor
      digitalWrite(MOTOR_L_INT1, LOW);
      analogWrite(MOTOR_L_INT2, MAX_SPEED);

      // Forward right motor
      analogWrite(MOTOR_R_INT1, MAX_SPEED);
      digitalWrite(MOTOR_R_INT2, LOW);

      Serial.println("Turn Left");

      lockout = 80;
    } else if (leftIR <= IR_THRESHOLD && rightIR > IR_THRESHOLD) {
      // Left cliff, turn right
      // Forward left motor
      analogWrite(MOTOR_L_INT1, MAX_SPEED);
      digitalWrite(MOTOR_L_INT2, LOW);

      // Reverse right motor
      digitalWrite(MOTOR_R_INT1, LOW);
      analogWrite(MOTOR_R_INT2, MAX_SPEED);

      Serial.println("Turn Right");

      lockout = 80;
    } else if (leftIR <= IR_THRESHOLD && rightIR <= IR_THRESHOLD) {
      // All cliff
      // Stop left motor
      digitalWrite(MOTOR_L_INT1, LOW);
      digitalWrite(MOTOR_L_INT2, LOW);

      // Stop right motor
      digitalWrite(MOTOR_R_INT1, LOW);
      digitalWrite(MOTOR_R_INT2, LOW);

      lockout = 20;
    }
  } else if (lockout == 0) {
    lockout = 20;

    // Stop left motor
    digitalWrite(MOTOR_L_INT1, LOW);
    digitalWrite(MOTOR_L_INT2, LOW);

    // Stop right motor
    digitalWrite(MOTOR_R_INT1, LOW);
    digitalWrite(MOTOR_R_INT2, LOW);
  }

  while (lockout > 0) {
    delay(10);
    lockout--;

    if (Serial.available() > 0 && bufferPos < BUFFER_SIZE) {
      // Read one byte to buffer
      buffer[bufferPos] = Serial.read();
      // Add length to buffer
      bufferPos++;
    } else if (bufferPos > 0) {
      if (buffer[0] == 'c' && buffer[1] == ':' && buffer[2] == '1') {
        // cmd:1, testing turn
        cmd_turn = true;

        bufferPos = 0;
      }

      if (buffer[0] == 'c' && buffer[1] == ':' && buffer[2] == '2') {
        // cmd:2, toggle auto
        cmd_auto = !cmd_auto;

        Serial.print("Toggled auto to ");
        Serial.println(cmd_auto);

        bufferPos = 0;
      }

      if (buffer[0] == 'c' && buffer[1] == ':' && buffer[2] == '3') {
        // cmd:2, toggle auto
        cmd_brushes = !cmd_brushes;

        Serial.print("Toggled cmd_brushes to ");
        Serial.println(cmd_brushes);

        digitalWrite(BRUSHES_EEP, cmd_brushes);

        bufferPos = 0;
      }
    }

    Serial.print("RL:");
    Serial.print(rearLeftIR);
    Serial.print(",RR:");
    Serial.println(rearRightIR);
  }
}
