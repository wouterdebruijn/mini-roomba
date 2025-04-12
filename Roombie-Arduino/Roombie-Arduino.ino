#include <Arduino.h>

#include "Motor.h"
#include "Web.h"
#include "CliffSensor.h"
#include "Eyes.h"
#include <Servo.h>

#define EYE_SERVO_PWM 3

#define BRUSHES_EEP 4

#define IR_THRESHOLD 600
#define IR_THRESHOLD_REAR 600

#define MAX_SPEED 255

Motor leftDrive(5, 6, 2);
Motor rightDrive(9, 10, 2);

CliffSensor sensorFrontLeft(A1, 14, 0);
CliffSensor sensorFrontRight(A0, 15, 1);

CliffSensor sensorRearLeft(A2, 16, 2);
CliffSensor sensorRearRight(A3, 7, 3);

Servo servo;

CliffSensor *sensors[] = {
    &sensorFrontLeft,
    &sensorFrontRight,
    &sensorRearLeft,
    &sensorRearRight,
    nullptr};

Web wemos(&Serial1, sensors);

Eyes eyes(&wemos, &servo, 0);

int lockout;
int servoPos = 10;

// the setup function runs once when you press reset or power the board
void setup()
{
  pinMode(BRUSHES_EEP, OUTPUT);
  // servo.attach(3);
  servo.write(10);

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
    // One of the back cliff sensors if off the table, risk of falling
    // Atleast one of the front sensors is on the table (we aren't picked up)
    if ((rearLeftIR <= IR_THRESHOLD_REAR || rearRightIR <= IR_THRESHOLD_REAR) && (leftIR > IR_THRESHOLD || rightIR > IR_THRESHOLD))
    {
      leftDrive.state(true);

      // Emergency stop!
      // Reverse Left
      leftDrive.drive(-64);

      // Reverse Right
      rightDrive.drive(-128);

      lockout = 250;

      eyes.setHappiness(ANGRY);
    }
    else if (leftIR > IR_THRESHOLD && rightIR > IR_THRESHOLD)
    {
      leftDrive.state(true);

      // Forward
      // Forward left motor
      leftDrive.drive(MAX_SPEED);

      // Forward right motor
      rightDrive.drive(MAX_SPEED);

      lockout = 1;

      eyes.setHappiness(HAPPY);
    }
    else if (leftIR > IR_THRESHOLD && rightIR <= IR_THRESHOLD)
    {
      leftDrive.state(true);

      // Right cliff, turn left
      // Reverse left motor
      leftDrive.drive(-MAX_SPEED);

      // Forward right motor
      rightDrive.drive(MAX_SPEED);

      lockout = 80;

      eyes.setHappiness(SAD);
    }
    else if (leftIR <= IR_THRESHOLD && rightIR > IR_THRESHOLD)
    {
      leftDrive.state(true);
      
      // Left cliff, turn right
      // Forward left motor
      leftDrive.drive(MAX_SPEED);

      // Reverse right motor
      rightDrive.drive(-MAX_SPEED);
      lockout = 80;

      eyes.setHappiness(SAD);
    }
    // Both front sensors are off the table, emergency back sensors did not trigger, we stop driving
    else if (leftIR <= IR_THRESHOLD && rightIR <= IR_THRESHOLD)
    {
      rightDrive.state(false);

      // All cliff
      // Stop left motor
      leftDrive.drive(0);

      // Stop right motor
      rightDrive.drive(0);
      lockout = 20;

      eyes.setHappiness(ANGRY);
    }
  }
  // Not AUTO mode, idle
  else if (lockout == 0)
  {
    rightDrive.state(false);

    // Auto mode disabled, wait for enable
    lockout = 20;

    // Stop left motor
    leftDrive.drive(0);

    // Stop right motor
    rightDrive.drive(0);

    eyes.setHappiness(HAPPY);
  }

  // Drive function handled, handle other functions while waiting for lockout to finish
  while (lockout > 0)
  {
    delay(10);

    wemos.handle_command();

    digitalWrite(BRUSHES_EEP, wemos.brushesEnabled());

    lockout--;
  }

  // Send sensor metrics to Wemos
  wemos.sendMetrics();
}
