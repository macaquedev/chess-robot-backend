#ifndef INIT_H
#define INIT_H

#include "calibration.h"

void homeMotors()
{
    bringMotorToEndstop(&eStepper, E_ENDSTOP, true);
    bringMotorToEndstop(&xStepper, X_ENDSTOP, false);
    bringMotorToEndstop(&yStepper, Y_ENDSTOP, false);
    currentPosition.x = 0;
    currentPosition.y = 0;
    currentPosition.e = 0;
}

void prepareMenu()
{
    bool back = true;
    while (back)
    {
        char menuEntries[][18] = {"Back", "Home Motors", "Move X", "Move Y", "Move Extruder", "Calibrate...", "Reset Calibration"};
        uint8_t answer = displayMenuAndWaitForAnswer(7, menuEntries);
        switch (answer)
        {
        case 0:
        {
            back = false;
            break;
        }
        case 1:
            homeMotors();
            shortBeep();
            break;

        case 2:
        {
            char menuEntries2[][18] = {"Move X 10cm", "Move X 1cm", "Move X 1mm"};
            double deltas[] = {10, 1, 0.1};
            double delta = deltas[displayMenuAndWaitForAnswer(3, menuEntries2)];
            xAxisMovementMenu(delta);
            shortBeep();
            break;
        }

        case 3:
        {
            char menuEntries2[][18] = {"Move Y 10cm", "Move Y 1cm", "Move Y 1mm"};
            double deltas[] = {10, 1, 0.1};
            double delta = deltas[displayMenuAndWaitForAnswer(3, menuEntries2)];
            yAxisMovementMenu(delta);
            shortBeep();
            break;
        }

        case 4:
        {
            char menuEntries2[][18] = {"Move E 1cm", "Move E 1mm"};
            double deltas[] = {1, 0.1};
            double delta = deltas[displayMenuAndWaitForAnswer(2, menuEntries2)];
            extruderMovementMenu(delta);
            shortBeep();
            break;
        }

        case 5:
        {
            calibrationWizard();
            break;
        }

        case 6:
        {
            EEPROM.write(0, 0);
            fillEEPROM();
            break;
        }
        default:
            break;
        }
        clearDisplay();
    }
}

#endif