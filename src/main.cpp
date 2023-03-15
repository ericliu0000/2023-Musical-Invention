#include <Arduino.h>
#include <AccelStepper.h>
#include <AccelStepper.cpp>
#include <MultiStepper.h>
#include <MultiStepper.cpp>

const int MICROSTEPPING = 16;
const long STEPS_PER_R = 200.0 * MICROSTEPPING;
const long SPEED = 1 * STEPS_PER_R;
const long ACCELERATION = 800.0 * MICROSTEPPING;
const long GROUP_SPEED = 0.3 * STEPS_PER_R;

const int FAN_PIN = 9;
const int BED_PIN = 8;
const int E0_PIN = 10;
const int E1_PIN = 7;

const int BUTTON_PIN = 67;

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

const int NUM_FRAMES = 4;
const int NUM_STEPPERS = 4;
long positions[NUM_FRAMES][NUM_STEPPERS] = {
    {90, 180, 270, 360},
    {0, 0, 0, 0},
    {360, 270, 180, 90},
    {0, 0, 0, 0},
    // {0, 0, 0, 0},
    // {0, 0, 0, 0},
    // {0, 0, 0, 0},
    // {0, 0, 0, 0},
    // {0, 0, 0, 0},
    // {0, 0, 0, 0},
};

AccelStepper stepper1 = AccelStepper(1, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepper2 = AccelStepper(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
AccelStepper stepper3 = AccelStepper(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);
AccelStepper stepper4 = AccelStepper(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);

MultiStepper steppers;

void configIndividual()
{
    // Reset positions
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);
    stepper4.setCurrentPosition(0);

    // Configure steppers
    stepper1.setMaxSpeed(SPEED);
    stepper1.setSpeed(SPEED);
    stepper1.setAcceleration(ACCELERATION);

    stepper2.setMaxSpeed(SPEED);
    stepper2.setSpeed(SPEED);
    stepper2.setAcceleration(ACCELERATION);

    stepper3.setMaxSpeed(SPEED);
    stepper3.setSpeed(SPEED);
    stepper3.setAcceleration(ACCELERATION);

    stepper4.setMaxSpeed(SPEED);
    stepper4.setSpeed(SPEED);
    stepper4.setAcceleration(ACCELERATION);
}

void configGroup()
{
    // Reset positions
    // stepper1.setCurrentPosition(0);
    // stepper2.setCurrentPosition(0);
    // stepper3.setCurrentPosition(0);
    // stepper4.setCurrentPosition(0);

    // Configure speeds
    stepper1.setSpeed(GROUP_SPEED);
    stepper2.setSpeed(GROUP_SPEED);
    stepper3.setSpeed(GROUP_SPEED);
    stepper4.setSpeed(GROUP_SPEED);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    for (int i = 0; i < NUM_FRAMES; i++)
    {
        for (int j = 0; j < NUM_STEPPERS; j++)
        {
            positions[i][j] = (long)positions[i][j] * STEPS_PER_R / 360;
        }
    }

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
    configIndividual();

    // Set positions
    stepper1.moveTo(10 * STEPS_PER_R);
    stepper2.moveTo(10 * STEPS_PER_R);
    stepper3.moveTo(10 * STEPS_PER_R);
    stepper4.moveTo(10 * STEPS_PER_R);

    // Add steppers
    steppers.addStepper(stepper1);
    steppers.addStepper(stepper2);
    steppers.addStepper(stepper3);
    steppers.addStepper(stepper4);
}

void loop()
{
    while (digitalRead(BUTTON_PIN)) {
        ;
    }

    configIndividual();

    stepper1.runToNewPosition(-300);
    delay(200);

    stepper2.runToNewPosition(-300);
    delay(200);

    stepper3.runToNewPosition(-300);
    delay(200);

    stepper4.runToNewPosition(-300);
    delay(200);

    configGroup();
    for (auto &position : positions)
    {
        steppers.moveTo(position);
        steppers.runSpeedToPosition();
        delay(200);
    }
}
