#include <Arduino.h>
#include <AccelStepper.h>

const int MICROSTEPPING = 16;
const float STEPS_PER_R = 200.0 * MICROSTEPPING;
const float SPEED = STEPS_PER_R * 2.0;
const float ACCELERATION = 50.0;

const int FAN_PIN = 9;
const int BED_PIN = 8;
const int E0_PIN = 10;
const int E1_PIN = 7;

const int X_STEP_PIN = 54;
const int X_DIR_PIN = 55;
const int X_ENABLE_PIN = 38;

const int Y_STEP_PIN = 60;
const int Y_DIR_PIN = 61;
const int Y_ENABLE_PIN = 56;

const int Z_STEP_PIN = 46;
const int Z_DIR_PIN = 48;
const int Z_ENABLE_PIN = 62;

const int E_STEP_PIN = 26;
const int E_DIR_PIN = 28;
const int E_ENABLE_PIN = 24;

AccelStepper stepper1 = AccelStepper(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);

void setup()
{
    stepper1.setMaxSpeed(SPEED);
    stepper1.setAcceleration(ACCELERATION);
}

void loop()
{
   stepper1.move(2 * STEPS_PER_R);
   stepper1.runToPosition();
   delay(1000);

}