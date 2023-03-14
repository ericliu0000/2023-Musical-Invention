#include <Arduino.h>
#include <SpeedyStepper.cpp>

const int MICROSTEPPING = 16;

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

SpeedyStepper stepper1;
SpeedyStepper stepper2;
SpeedyStepper stepper3;
SpeedyStepper stepper4;

void setup()
{
    // Set enable to low to enable steppers
    pinMode(X_ENABLE_PIN, OUTPUT);
    pinMode(Y_ENABLE_PIN, OUTPUT);
    pinMode(Z_ENABLE_PIN, OUTPUT);
    pinMode(E_ENABLE_PIN, OUTPUT);

    digitalWrite(X_ENABLE_PIN, 0);
    digitalWrite(Y_ENABLE_PIN, 0);
    digitalWrite(Z_ENABLE_PIN, 0);
    digitalWrite(E_ENABLE_PIN, 0);

    stepper1.connectToPins(X_STEP_PIN, X_DIR_PIN);
    stepper2.connectToPins(Y_STEP_PIN, Y_DIR_PIN);
    stepper3.connectToPins(Z_STEP_PIN, Z_DIR_PIN);
    stepper4.connectToPins(E_STEP_PIN, E_DIR_PIN);

    stepper1.setSpeedInStepsPerSecond(200.0 * MICROSTEPPING);
    stepper2.setSpeedInStepsPerSecond(200.0 * MICROSTEPPING);
    stepper3.setSpeedInStepsPerSecond(200.0 * MICROSTEPPING);
    stepper4.setSpeedInStepsPerSecond(200.0 * MICROSTEPPING);

    stepper1.setAccelerationInStepsPerSecondPerSecond(8000.0 * MICROSTEPPING);
    stepper2.setAccelerationInStepsPerSecondPerSecond(8000.0 * MICROSTEPPING);
    stepper3.setAccelerationInStepsPerSecondPerSecond(8000.0 * MICROSTEPPING);
    stepper4.setAccelerationInStepsPerSecondPerSecond(8000.0 * MICROSTEPPING);
}

void loop()
{
    stepper1.setupRelativeMoveInRevolutions(2 * MICROSTEPPING);
    stepper2.setupRelativeMoveInRevolutions(3 * MICROSTEPPING);
    stepper3.setupRelativeMoveInRevolutions(4 * MICROSTEPPING);
    stepper4.setupRelativeMoveInRevolutions(5 * MICROSTEPPING);

    while (!stepper4.motionComplete()) {
        stepper1.processMovement();
        stepper2.processMovement();
        stepper3.processMovement();
        stepper4.processMovement();
    }

    delay(500);
}