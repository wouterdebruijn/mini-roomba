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

// Counter keeping track of drive cycles
uint8_t driveCount = 0;
uint8_t onGroundCount = 0;

bool wasPickedUp = false;

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
    // Only one cliff sensor can be of the table, other one should be fine, otherwise we expect that we are picked up
    if (((rearLeftIR <= IR_THRESHOLD_REAR && rearRightIR > IR_THRESHOLD_REAR) || (rearRightIR <= IR_THRESHOLD_REAR && rearLeftIR > IR_THRESHOLD_REAR)) && (leftIR > 850 || rightIR > 850))
    {
      leftDrive.state(false);

      // Emergency stop!
      // Reverse Left
      leftDrive.drive(0);

      // Reverse Right
      rightDrive.drive(0);

      lockout = 500;

      eyes.setHappiness(SAD);
      eyes.setBuzzer(SOUND_SAD);
    }
    else if (leftIR > IR_THRESHOLD && rightIR > IR_THRESHOLD)
    {
      driveCount++;
      onGroundCount++;

      if (wasPickedUp)
      {
        Serial.println(onGroundCount);

        // Wait for 16 cycles of proper ground
        if (onGroundCount > 16)
        {
          // We were picked up and are now back, perform animation and continue
          eyes.setHappiness(SAD);
          eyes.blink(2);

          eyes.setBuzzer(SOUND_SAD);
          delay(1000);
          eyes.blink(3);

          delay(2000);
          wasPickedUp = false;

          lockout = 1;
        }
      }
      else
      {
        leftDrive.state(true);

        // Forward
        // Forward left motor
        leftDrive.drive(MAX_SPEED);

        // Forward right motor
        rightDrive.drive(MAX_SPEED);

        lockout = 1;

        // Only calc random value every 255 cycles (2550ms)
        if (driveCount == 0)
        {
          // idle animation, blink and maybe play happy noises.
          uint8_t blinkHappy = random(32);
          if (blinkHappy == 0)
          {
            eyes.blink(2);

            if (random(16) == 0)
            {
              eyes.setBuzzer(SOUND_HAPPY);
            }
          }
        }

        eyes.setHappiness(HAPPY);
      }
    }
    else if (leftIR > IR_THRESHOLD && rightIR <= IR_THRESHOLD)
    {
      onGroundCount = 0;
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
      onGroundCount = 0;
      leftDrive.state(true);

      // Left cliff, turn right
      // Forward left motor
      leftDrive.drive(MAX_SPEED);

      // Reverse right motor
      rightDrive.drive(-MAX_SPEED);
      lockout = 80;

      eyes.setHappiness(SAD);
    }
    // PICKED UP
    // Both front sensors are off the table, emergency back sensors did not trigger, we stop driving
    else if (leftIR <= IR_THRESHOLD && rightIR <= IR_THRESHOLD)
    {
      onGroundCount = 0;
      rightDrive.state(false);

      // All cliff
      // Stop left motor
      leftDrive.drive(0);

      // Stop right motor
      rightDrive.drive(0);
      lockout = 500;

      // Start small animation because of pickup
      eyes.setHappiness(ANGRY);
      delay(200);
      eyes.setBuzzer(SOUND_ANGRY);

      eyes.blink(1);
      delay(1000);
      eyes.blink(3);

      wasPickedUp = true;
    }
  }
  // Not AUTO mode, idle
  else if (lockout == 0)
  {
    onGroundCount = 0;
    rightDrive.state(false);

    // Auto mode disabled, wait for enable
    lockout = 20;

    // Stop left motor
    leftDrive.drive(0);

    // Stop right motor
    rightDrive.drive(0);

    eyes.setHappiness(HAPPY);

    // idle animation, blink and maybe play happy noises.
    uint8_t blinkHappy = random(64);
    if (blinkHappy == 0)
    {
      eyes.blink(2);

      if (random(16) == 0)
      {
        eyes.setBuzzer(SOUND_HAPPY);
      }
    }
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
