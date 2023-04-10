/**
 * Beauty and the Beast "Invention" for Maurice's Entrance
 *
 * Uses:
 * AccelStepper
 * Copyright (c) 2010-2021 Mike McCauley
 * Licensed under GPLv3
 *
 * Arduino SDK
 * Copyright (c) 2005-2013 Arduino Team
 * Licensed under LGPLv2.1
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include <AccelStepper.h>
#include <AccelStepper.cpp>
#include <MultiStepper.h>
#include <MultiStepper.cpp>

// Program constants
bool flip = false;

// Stepper motor constants
const int MICROSTEPPING = 16;
const long REVOLUTIONS_TO_STEPS = 200.0 * MICROSTEPPING;
const long ACCELERATION = 800.0 * MICROSTEPPING;
const long SPEED = 0.5 * REVOLUTIONS_TO_STEPS;
const long GROUP_SPEED = 0.3 * REVOLUTIONS_TO_STEPS;
const long DEGREES_TO_STEPS = REVOLUTIONS_TO_STEPS / 360;

// User input constants
const int ACTIVATE_PIN = 67;
const int DISABLE_PIN = 68;
const int PARTIAL_PIN = 69;

// Driver pin constants
const int X_STEP_PIN = 54;
const int Y_STEP_PIN = 60;
const int Z_STEP_PIN = 46;
const int E_STEP_PIN = 26;

const int X_DIRECTION_PIN = 55;
const int Y_DIRECTION_PIN = 61;
const int Z_DIRECTION_PIN = 48;
const int E_DIRECTION_PIN = 28;

const int X_ENABLE_PIN = 38;
const int Y_ENABLE_PIN = 56;
const int Z_ENABLE_PIN = 62;
const int E_ENABLE_PIN = 24;

const int NUM_STEPPERS = 4;

// Predefined sequence for full operation
const int NUM_FRAMES = 13;
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

// Predefined sequence for entrance
const int PARTIAL_CYCLES = 20;
long partialPositions[4][NUM_STEPPERS] = {
    {60, 90, 0, -70},
    {0, 30, 0, -70},
    {60, 90, 0, 0},
    {0, 0, 0, 0},
};

// Objects for stepper control
AccelStepper stepper1 = AccelStepper(AccelStepper::DRIVER, E_STEP_PIN, E_DIRECTION_PIN);
AccelStepper stepper2 = AccelStepper(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIRECTION_PIN);
AccelStepper stepper3 = AccelStepper(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIRECTION_PIN);
AccelStepper stepper4 = AccelStepper(AccelStepper::DRIVER, X_STEP_PIN, X_DIRECTION_PIN);

MultiStepper steppers;

/**
 * Sets power state of stepper motor drivers. Assumes pull down logic
 * for enable pins.
 *
 * @param enable Desired power state for drivers
 */
void enableSteppers(bool enable)
{
    digitalWrite(X_ENABLE_PIN, !enable);
    digitalWrite(Y_ENABLE_PIN, !enable);
    digitalWrite(Z_ENABLE_PIN, !enable);
    digitalWrite(E_ENABLE_PIN, !enable);
}

/**
 * Sets up motors for operation by:
 * 1) enabling all stepper motors;
 * 2) resetting stored positions to 0; and,
 * 3) re-setting the desired motor speeds.
 */
void config()
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

/**
 * Runs initial sequence when machine is brought onto the stage.
 * Results in a 14 second long run time.
 */
void runPartialSequence()
{
    config();
    // Wake up sequence
    steppers.moveTo(partialPositions[1]);
    steppers.runSpeedToPosition();
    steppers.moveTo(partialPositions[3]);
    steppers.runSpeedToPosition();

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

/**
 * Runs full sequence, resembling full function of machine.
 * This runs until manually disabled.
 */
void runSequence()
{
    config();

    // Wakeup animation
    for (auto &position : positions)
    {
        steppers.moveTo(position);
        steppers.runSpeedToPosition();

        if (!digitalRead(DISABLE_PIN))
        {
            // Park and stop if requested
            steppers.moveTo(positions[NUM_FRAMES - 1]);
            steppers.runSpeedToPosition();
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

        // Update positions
        hold[0] += 100 * DEGREES_TO_STEPS;
        hold[1] = (rand() % 140 + 10) * DEGREES_TO_STEPS;
        hold[2] += (flip ? -90 : 90) * DEGREES_TO_STEPS;
        hold[3] = (hold[3] == -140 * DEGREES_TO_STEPS) ? -80 * DEGREES_TO_STEPS : -140 * DEGREES_TO_STEPS;

        // For motor 3, flip angle
        if (hold[2] <= -355 * DEGREES_TO_STEPS)
        {
            flip = false;
        }
        else if (hold[2] >= -95 * DEGREES_TO_STEPS)
        {
            flip = true;
        }

        steppers.moveTo(hold);
        steppers.runSpeedToPosition();
    }
}

void setup()
{
    // Set up user input pins
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(ACTIVATE_PIN, INPUT_PULLUP);
    pinMode(PARTIAL_PIN, INPUT_PULLUP);
    pinMode(DISABLE_PIN, INPUT_PULLUP);

    // Convert stored positions from degrees to steps
    for (int i = 0; i < NUM_FRAMES; i++)
    {
        for (int j = 0; j < NUM_STEPPERS; j++)
        {
            positions[i][j] = (long)positions[i][j] * DEGREES_TO_STEPS;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < NUM_STEPPERS; j++)
        {
            partialPositions[i][j] = (long)partialPositions[i][j] * DEGREES_TO_STEPS;
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

    // I am not smart enough to debounce the inputs. Here is the solution.
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
