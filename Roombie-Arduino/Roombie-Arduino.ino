#include <Arduino.h>

#include "Motor.h"
#include "Web.h"
#include "CliffSensor.h"
#include "Eyes.h"
#include <Servo.h>

#define EYE_SERVO_PWM 3

#define BRUSHES_EEP 4

#define IR_THRESHOLD 900
#define IR_THRESHOLD_REAR 900

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
uint8_t turnCount = 0;

bool wasPickedUp = false;
bool blockedWipers = false;

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

void wiggle()
{
  leftDrive.drive(-MAX_SPEED);
  rightDrive.drive(MAX_SPEED);
  leftDrive.state(true);
  delay(100);
  leftDrive.drive(MAX_SPEED);
  rightDrive.drive(-MAX_SPEED);
  delay(100);
  leftDrive.drive(-MAX_SPEED);
  rightDrive.drive(MAX_SPEED);
  delay(50);
}

// Big turncount results is frustration
void turnCountAnimation()
{
  eyes.setHappiness(ANGRY);
  eyes.setBuzzer(SOUND_ANGRY);

  wiggle();
}

void blockWipers() {
  blockedWipers = true;
  digitalWrite(BRUSHES_EEP, LOW);
}

void unlockWipers() {
  blockedWipers = false;
  digitalWrite(BRUSHES_EEP, wemos.brushesEnabled());
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
    if (((rearLeftIR <= IR_THRESHOLD_REAR && rearRightIR > IR_THRESHOLD_REAR) || (rearRightIR <= IR_THRESHOLD_REAR && rearLeftIR > IR_THRESHOLD_REAR)) && (leftIR > IR_THRESHOLD || rightIR > IR_THRESHOLD))
    {
      blockWipers();
      leftDrive.state(true);

      // Emergency stop!
      // Reverse Left
      leftDrive.drive(-128);

      // Reverse Right
      rightDrive.drive(-128);

      lockout = 80;

      eyes.setHappiness(SAD);
      eyes.setBuzzer(SOUND_SAD);
    }
    else if (turnCount > 8)
    {
      blockWipers();
      turnCountAnimation();
      turnCount = 0;
    }
    // PICKED UP
    // All sensors all up
    else if (leftIR <= IR_THRESHOLD && rightIR <= IR_THRESHOLD && rearLeftIR <= IR_THRESHOLD_REAR && rearRightIR <= IR_THRESHOLD_REAR)
    {
      blockWipers();
      onGroundCount = 0;
      rightDrive.state(false);

      // All cliff
      // Stop left motor
      leftDrive.drive(0);

      // Stop right motor
      rightDrive.drive(0);
      lockout = 500;

      // Start small animation because of pickup
      delay(200);

      if (!wasPickedUp)
      {
        // Were not picked up before, act surprised
        eyes.setHappiness(SAD);
        eyes.setBuzzer(SOUND_SURPRISE);
      }

      eyes.blink(1);
      delay(1000);

      eyes.setHappiness(ANGRY);
      eyes.setBuzzer(SOUND_ANGRY);
      eyes.blink(3);

      wasPickedUp = true;
    }
    else if (leftIR > IR_THRESHOLD && rightIR > IR_THRESHOLD)
    {
      unlockWipers();
      driveCount++;
      onGroundCount++;

      if (wasPickedUp)
      {
        // Wait for 16 cycles of proper ground
        if (onGroundCount > 16)
        {
          // We were picked up and are now back, perform animation and continue
          eyes.setHappiness(SAD);
          eyes.blink(2);

          eyes.setBuzzer(SOUND_SAD);
          delay(1000);
          eyes.blink(3);

          delay(500);

          leftDrive.state(true);
          wiggle();

          leftDrive.state(false);

          eyes.setHappiness(HAPPY);
          eyes.setBuzzer(SOUND_HAPPY);

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

        // Only calc random value every 64 cycles
        if (driveCount > 64)
        {
          // idle animation, blink and maybe play happy noises.
          uint8_t blinkHappy = random(2);
          if (blinkHappy == 0)
          {
            eyes.blink(2);

            if (random(8) == 0)
            {
              leftDrive.drive(0);
              rightDrive.drive(0);

              delay(200);

              wiggle();
              eyes.setBuzzer(SOUND_HAPPY);
            }
          }

          driveCount = 0;
        }

        eyes.setHappiness(HAPPY);
      }
    }
    else if (leftIR > IR_THRESHOLD && rightIR <= IR_THRESHOLD)
    {
      unlockWipers();
      onGroundCount = 0;
      leftDrive.state(true);

      // Right cliff, turn left
      // Reverse left motor
      leftDrive.drive(-MAX_SPEED);

      // Forward right motor
      rightDrive.drive(MAX_SPEED);
      lockout = 80;

      turnCount++;
    }
    else if (leftIR <= IR_THRESHOLD && rightIR > IR_THRESHOLD)
    {
      unlockWipers();
      onGroundCount = 0;
      leftDrive.state(true);

      // Left cliff, turn right
      // Forward left motor
      leftDrive.drive(MAX_SPEED);

      // Reverse right motor
      rightDrive.drive(-MAX_SPEED);
      lockout = 80;

      turnCount++;
    }
    else if (leftIR <= IR_THRESHOLD && rightIR <= IR_THRESHOLD)
    {
      blockWipers();
      leftDrive.state(true);
      // All cliff
      // Stop left motor
      leftDrive.drive(-MAX_SPEED);

      // Stop right motor
      rightDrive.drive(-128);

      lockout = 80;
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

    digitalWrite(BRUSHES_EEP, wemos.brushesEnabled() && !blockedWipers);

    lockout--;
  }

  // Send sensor metrics to Wemos
  wemos.sendMetrics();
}
