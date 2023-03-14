#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

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

long positions[10][4] = {
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
};

AccelStepper stepper1 = AccelStepper(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
// AccelStepper stepper2 = AccelStepper(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
// AccelStepper stepper3 = AccelStepper(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);
// AccelStepper stepper4 = AccelStepper(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);

// MultiStepper steppers;

void setup()
{
    // Always enable stepper
    pinMode(X_ENABLE_PIN, OUTPUT);
    pinMode(Y_ENABLE_PIN, OUTPUT);
    pinMode(Z_ENABLE_PIN, OUTPUT);
    pinMode(E_ENABLE_PIN, OUTPUT);

    digitalWrite(X_ENABLE_PIN, 0);
    digitalWrite(Y_ENABLE_PIN, 0);
    digitalWrite(Z_ENABLE_PIN, 0);
    digitalWrite(E_ENABLE_PIN, 0);

    // Configure motors
    stepper1.setMaxSpeed(SPEED);
    stepper1.setSpeed(SPEED);
    stepper1.setAcceleration(ACCELERATION);

    /*
    stepper2.setMaxSpeed(SPEED);
    stepper2.setSpeed(SPEED);
    stepper2.setAcceleration(ACCELERATION);

    stepper3.setMaxSpeed(SPEED);
    stepper3.setSpeed(SPEED);
    stepper3.setAcceleration(ACCELERATION);

    stepper4.setMaxSpeed(SPEED);
    stepper4.setSpeed(SPEED);
    stepper4.setAcceleration(ACCELERATION);

    // Set positions
    stepper1.moveTo(10 * STEPS_PER_R);
    stepper2.moveTo(10 * STEPS_PER_R);
    stepper3.moveTo(10 * STEPS_PER_R);
    stepper4.moveTo(10 * STEPS_PER_R);
    */
}

void loop()
{
    stepper1.move(2 * STEPS_PER_R);
    stepper1.runToPosition();
    delay(1000);
    stepper1.move(-2 * STEPS_PER_R);
    stepper1.runToPosition();
    delay(1000);

    // for (auto& position : positions) {
    //     steppers.moveTo(position);
    //     steppers.runSpeedToPosition();
    //     delay(100);
    // }
}