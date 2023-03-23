#include <Arduino.h>
#include <AccelStepper.h>
#include <AccelStepper.cpp>
#include <MultiStepper.h>
#include <MultiStepper.cpp>

// for led, flip
bool state = false;
bool flip = false;

const int MICROSTEPPING = 16;
const long R_TO_STEPS = 200.0 * MICROSTEPPING;
const long SPEED = 0.5 * R_TO_STEPS;
const long ACCELERATION = 800.0 * MICROSTEPPING;
const long GROUP_SPEED = 0.3 * R_TO_STEPS;
const long DEG_TO_STEPS = R_TO_STEPS / 360;

const int ACTIVATE_PIN = 67;
const int DISABLE_PIN = 68;
const int PARTIAL_PIN = 69;

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

const int NUM_FRAMES = 13;
const int NUM_STEPPERS = 4;
long positions[NUM_FRAMES][NUM_STEPPERS] = {
    {50, 0, 0, 0},
    {50, 70, 0, 0},
    {70, 110, -90, 0},
    {50, 110, -180, -70},
    {80, 90, -210, -90},
    {-30, 130, -140, -30},
    {60, 90, -250, -85},
    {-30, 40, -90, -40},
    {0, 90, -180, -90},
    {0, 45, -180, -40},
    {0, 0, -135, -40},
    {0, 0, -90, 0},
    {0, 0, 0, 0},
};

const int PARTIAL_CYCLES = 20;
long partialPositions[4][NUM_STEPPERS] = {
    {60, 90, 0, -70},
    {0, 30, 0, -70},
    {60, 90, 0, 0},
    {0, 0, 0, 0},
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

void configGroup()
{
    enableSteppers(true);

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

void blink(int wait)
{
    // digitalWrite(LED_BUILTIN, state);
    digitalWrite(LED_BUILTIN, false);
    state = !state;
    if (wait > 0)
    {
        delay(wait);
    }
}

void runPartialSequence()
{
    configGroup();
    // Run for certain number of cycles, then stop
    for (int i = 0; i < PARTIAL_CYCLES; i++)
    {
        steppers.moveTo(partialPositions[i % 4]);
        steppers.runSpeedToPosition();

        // Disable if needed
        if (!digitalRead(DISABLE_PIN))
        {
            steppers.moveTo(partialPositions[3]);
            steppers.runSpeedToPosition();
            return;
        }
    }
}

void runSequence()
{
    configGroup();

    // Wakeup animation
    for (auto &position : positions)
    {
        steppers.moveTo(position);
        steppers.runSpeedToPosition();

        if (!digitalRead(DISABLE_PIN))
        {
            steppers.moveTo(positions[NUM_FRAMES - 1]);
            steppers.runSpeedToPosition();
            digitalWrite(LED_BUILTIN, 0);
            return;
        }
    }

    // Indefinite loop
    long hold[4] = {0, 0, 0, 0};
    while (true)
    {
        // Park and stop when requested
        if (!digitalRead(DISABLE_PIN))
        {
            // Reset positions of oscillating elements
            hold[1] = 0;
            hold[3] = 0;

            steppers.moveTo(hold);
            steppers.runSpeedToPosition();
            return;
        }

        // Update posiitons
        hold[0] += 100 * DEG_TO_STEPS;
        hold[1] = (rand() % 140 + 10) * DEG_TO_STEPS;
        hold[2] += (flip ? -90 : 90) * DEG_TO_STEPS;
        hold[3] = (hold[3] == -140 * DEG_TO_STEPS) ? -80 * DEG_TO_STEPS : -140 * DEG_TO_STEPS;

        // For motor 3, flip angle
        if (hold[2] <= -355 * DEG_TO_STEPS)
        {
            flip = false;
        }
        else if (hold[2] >= -95 * DEG_TO_STEPS)
        {
            flip = true;
        }

        steppers.moveTo(hold);
        steppers.runSpeedToPosition();
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(ACTIVATE_PIN, INPUT_PULLUP);
    pinMode(PARTIAL_PIN, INPUT_PULLUP);
    pinMode(DISABLE_PIN, INPUT_PULLUP);

    // Convert to degrees
    for (int i = 0; i < NUM_FRAMES; i++)
    {
        for (int j = 0; j < NUM_STEPPERS; j++)
        {
            positions[i][j] = (long)positions[i][j] * DEG_TO_STEPS;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < NUM_STEPPERS; j++)
        {
            partialPositions[i][j] = (long)partialPositions[i][j] * DEG_TO_STEPS;
        }
    }

    // Configure enable pins
    pinMode(X_ENABLE_PIN, OUTPUT);
    pinMode(Y_ENABLE_PIN, OUTPUT);
    pinMode(Z_ENABLE_PIN, OUTPUT);
    pinMode(E_ENABLE_PIN, OUTPUT);

    // Add steppers
    steppers.addStepper(stepper1);
    steppers.addStepper(stepper2);
    steppers.addStepper(stepper3);
    steppers.addStepper(stepper4);
}

void loop()
{
    // Disable steppers
    enableSteppers(false);

    delay(200);

    if (!digitalRead(ACTIVATE_PIN))
    {
        runSequence();
    }
    else if (!digitalRead(PARTIAL_PIN))
    {
        runPartialSequence();
    }
}
