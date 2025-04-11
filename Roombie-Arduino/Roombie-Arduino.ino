#include <Arduino.h>

#include "Motor.h"
#include "Web.h"
#include "CliffSensor.h"

#define EYE_SERVO_PWM 3

#define BRUSHES_EEP 4

#define IR_THRESHOLD 600
#define IR_THRESHOLD_REAR 600

#define MAX_SPEED 255

Motor leftDrive(9, 10, 2);
Motor rightDrive(5, 6, 2);

CliffSensor sensorFrontLeft(A1, 14, 0);
CliffSensor sensorFrontRight(A0, 15, 1);

CliffSensor sensorRearLeft(A2, 16, 2);
CliffSensor sensorRearRight(A3, 7, 3);


CliffSensor *sensors[] = {
    &sensorFrontLeft,
    &sensorFrontRight,
    &sensorRearLeft,
    &sensorRearRight,
    nullptr};

Web wemos(&Serial1, sensors);

int lockout;

// the setup function runs once when you press reset or power the board
void setup()
{
  pinMode(BRUSHES_EEP, OUTPUT);

  Serial.begin(115200);

  wemos.serial->begin(19200);

  digitalWrite(BRUSHES_EEP, LOW);
  leftDrive.state(true);
  lockout = 0;
}

// the loop function runs over and over again forever
void loop()
{
  uint16_t leftIR = sensorFrontLeft.read();
  uint16_t rightIR = sensorFrontRight.read();

  uint16_t rearLeftIR = sensorRearLeft.read();
  uint16_t rearRightIR = sensorRearRight.read();

  if (wemos.autoEnabled() && lockout == 0)
  {
    if (rearLeftIR <= IR_THRESHOLD_REAR || rearRightIR <= IR_THRESHOLD_REAR)
    {
      // Emergency stop!
      // Reverse Left
      leftDrive.drive(-64);

      // Reverse Right
      rightDrive.drive(-96);

      lockout = 250;
    }
    else if (leftIR > IR_THRESHOLD && rightIR > IR_THRESHOLD)
    {
      // Forward
      // Forward left motor
      leftDrive.drive(MAX_SPEED);

      // Forward right motor
      rightDrive.drive(MAX_SPEED);

      lockout = 1;
    }
    else if (leftIR > IR_THRESHOLD && rightIR <= IR_THRESHOLD)
    {
      // Right cliff, turn left
      // Reverse left motor
      leftDrive.drive(-MAX_SPEED);

      // Forward right motor
      rightDrive.drive(MAX_SPEED);

      lockout = 80;
    }
    else if (leftIR <= IR_THRESHOLD && rightIR > IR_THRESHOLD)
    {
      // Left cliff, turn right
      // Forward left motor
      leftDrive.drive(MAX_SPEED);

      // Reverse right motor
      rightDrive.drive(-MAX_SPEED);
      lockout = 80;
    }
    else if (leftIR <= IR_THRESHOLD && rightIR <= IR_THRESHOLD)
    {
      // All cliff
      // Stop left motor
      leftDrive.drive(0);

      // Stop right motor
      rightDrive.drive(0);

      lockout = 20;
    }
  }
  else if (lockout == 0)
  {
    // Auto mode disabled, wait for enable
    lockout = 20;

    // Stop left motor
    leftDrive.drive(0);

    // Stop right motor
    rightDrive.drive(0);
  }

  while (lockout > 0)
  {
    delay(10);

    wemos.handle_command();
    wemos.sendMetrics();

    digitalWrite(BRUSHES_EEP, wemos.brushesEnabled());

    lockout--;
  }

}
