#include <Arduino.h>
#include <AccelStepper.h>
#include <AccelStepper.cpp>
#include <MultiStepper.h>
#include <MultiStepper.cpp>

// to save half a cycle time
bool state = false;

const int MICROSTEPPING = 16;
const long R_TO_STEPS = 200.0 * MICROSTEPPING;
const long SPEED = 0.5 * R_TO_STEPS;
const long ACCELERATION = 800.0 * MICROSTEPPING;
const long GROUP_SPEED = 0.3 * R_TO_STEPS;
const long DEG_TO_STEPS = R_TO_STEPS / 360;

const int FAN_PIN = 9;
const int BED_PIN = 8;
const int E0_PIN = 10;
const int E1_PIN = 7;

const int BUTTON_PIN = 67;
const int ROTATE_PIN = 68;

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

const int NUM_FRAMES = 22;
const int NUM_STEPPERS = 4;
// Home is approximately the negative x axis
long positions[NUM_FRAMES][NUM_STEPPERS] = {
    {90, 0, 0, 0},
    {90, 90, 0, 0},
    {90, 90, 90, 0},
    {90, 90, 90, 90},
    {170, 150, 130, 110},
    {30, 50, 70, 90},
    {170, 170, 170, 170},
    {30, 30, 30, 30},
    {170, 170, 170, 170},
    {30, 30, 30, 30},
    {90, 170, 90, 170},
    {30, 30, 30, 30},
    {170, 90, 170, 90},
    {90, 90, 90, 90},
    {50, 90, 90, 90},
    {50, 90, 90, 50},
    {50, 130, 90, 50},
    {50, 130, 130, 50},
    {90, 90, 90, 0},
    {90, 90, 0, 0},
    {90, 0, 0, 0},
    {0, 0, 0, 0},
    // {0, 0, 0, 0},
    // {0, 0, 0, 0},
    // {0, 0, 0, 0},
    // {0, 0, 0, 0},
    // {0, 0, 0, 0},
};

AccelStepper stepper1 = AccelStepper(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);
AccelStepper stepper2 = AccelStepper(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);
AccelStepper stepper3 = AccelStepper(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
AccelStepper stepper4 = AccelStepper(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);

MultiStepper steppers;

void enableSteppers(bool enable)
{
    digitalWrite(X_ENABLE_PIN, !enable);
    digitalWrite(Y_ENABLE_PIN, !enable);
    digitalWrite(Z_ENABLE_PIN, !enable);
    digitalWrite(E_ENABLE_PIN, !enable);
}

void configIndividual()
{
    // Reset positions
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);
    stepper4.setCurrentPosition(0);

    // Configure steppers
    stepper1.setMaxSpeed(SPEED);
    stepper2.setMaxSpeed(SPEED);
    stepper3.setMaxSpeed(SPEED);
    stepper4.setMaxSpeed(SPEED);

    stepper1.setSpeed(SPEED);
    stepper2.setSpeed(SPEED);
    stepper3.setSpeed(SPEED);
    stepper4.setSpeed(SPEED);

    stepper1.setAcceleration(ACCELERATION);
    stepper2.setAcceleration(ACCELERATION);
    stepper3.setAcceleration(ACCELERATION);
    stepper4.setAcceleration(ACCELERATION);
}

void configGroup()
{
    // Reset positions
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);
    stepper4.setCurrentPosition(0);

    // Configure speeds
    stepper1.setMaxSpeed(GROUP_SPEED);
    stepper2.setMaxSpeed(GROUP_SPEED);
    stepper3.setMaxSpeed(GROUP_SPEED);
    stepper4.setMaxSpeed(GROUP_SPEED);

    stepper1.setSpeed(GROUP_SPEED);
    stepper2.setSpeed(GROUP_SPEED);
    stepper3.setSpeed(GROUP_SPEED);
    stepper4.setSpeed(GROUP_SPEED);
}

void hotGlueMode()
{
    configIndividual();
    while (digitalRead(BUTTON_PIN))
    {
        stepper1.runSpeed();
        stepper2.runSpeed();
        stepper3.runSpeed();
        stepper4.runSpeed();
    }
}

void blink(int wait)
{
    digitalWrite(LED_BUILTIN, state);
    state = !state;
    if (wait > 0)
    {
        delay(wait);
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(ROTATE_PIN, INPUT_PULLUP);

    for (int i = 0; i < NUM_FRAMES; i++)
    {
        for (int j = 0; j < NUM_STEPPERS; j++)
        {
            positions[i][j] = (long)positions[i][j] * DEG_TO_STEPS;
        }
    }

    // Configure enable pins
    pinMode(X_ENABLE_PIN, OUTPUT);
    pinMode(Y_ENABLE_PIN, OUTPUT);
    pinMode(Z_ENABLE_PIN, OUTPUT);
    pinMode(E_ENABLE_PIN, OUTPUT);

    // Enable and configure motors
    enableSteppers(true);
    configIndividual();

    // Add steppers
    steppers.addStepper(stepper1);
    steppers.addStepper(stepper2);
    steppers.addStepper(stepper3);
    steppers.addStepper(stepper4);
}

void runSequence() {
    for (auto &position : positions)
    {
        steppers.moveTo(position);
        steppers.runSpeedToPosition();
        delay(20);
        blink(0);

        if (!digitalRead(BUTTON_PIN))
        {
            steppers.moveTo(positions[NUM_FRAMES - 1]);
            steppers.runSpeedToPosition();
            digitalWrite(LED_BUILTIN, 0);
            return;
        }
    }

    long randomHold[NUM_STEPPERS] = {0, 0, 0, 0};

    // Random stuff
    while (true)
    {
        if (!digitalRead(BUTTON_PIN))
        {
            randomHold[1] = 0;
            randomHold[2] = 0;
            randomHold[3] = 0;

            steppers.moveTo(randomHold);
            steppers.runSpeedToPosition();
            digitalWrite(LED_BUILTIN, 0);
            return;
        }

        randomHold[0] += 120 * DEG_TO_STEPS;
        randomHold[1] = (rand() % 100 + 40) * DEG_TO_STEPS;
        randomHold[2] = (rand() % 100 + 40) * DEG_TO_STEPS;
        randomHold[3] = (rand() % 100 + 40) * DEG_TO_STEPS;

        steppers.moveTo(randomHold);
        steppers.runSpeedToPosition();
        blink(0);
    }
}

void loop()
{
    configIndividual();

    while (digitalRead(BUTTON_PIN))
    {
        blink(200);

        if (!digitalRead(ROTATE_PIN))
        {
            digitalWrite(LED_BUILTIN, 1);
            hotGlueMode();
        }
    }

    // Wakeup sequence
    // TODO make an actual sequence
    configGroup();
    runSequence();
    
}
