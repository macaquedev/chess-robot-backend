#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <EEPROM.h>

#include "menu.h"

void fillEEPROM()
{
    int x = EEPROM.read(0);
    if (x != 117)
    {
        EEPROM.write(0, 117);
        H1Pos = Vector2(5, 5);
        OneSquareUp = Vector2(0, 2); 
        OneSquareLeft = Vector2(2, 0);
        EEPROM.put(1, H1Pos);
        EEPROM.put(1 + sizeof(Vector2), OneSquareUp);
        EEPROM.put(1 + 2 * sizeof(Vector2), OneSquareLeft);
    }
    else
    {
        EEPROM.get(1, H1Pos);
        EEPROM.get(1 + sizeof(Vector2), OneSquareUp);
        EEPROM.get(1 + 2 * sizeof(Vector2), OneSquareLeft);
    }
}

void positionSelectionMenu(Vector2 *position)
{
    moveToPosition(position, PIECE_MOVE_SPEED);
    eStepper.moveTo(E_MAX_POS * E_STEPS_PER_CM);
    while (eStepper.distanceToGo() != 0)
    {
        eStepper.run();
    }
    double currentEncoderPosition = encoder.getPosition();
    double previousEncoderPosition = currentEncoderPosition;
    bool pressed = false;
    bool isCurrentlyOnXAxis = true;
    for (;;)
    {
        clearDisplay();
        u8g.firstPage();
        do
        {
            u8g.setFont(DISPLAY_FONT);
            u8g.drawStr(0, 12, (String(F("Calibrating ")) + String(isCurrentlyOnXAxis ? "x" : "y") + String(" position.")).c_str());
            u8g.setFont(DISPLAY_SMALLFONT);
            u8g.drawStr(0, 28, (String(F("(x, y) = (")) + String(position->x, 2) + String(F(", ")) + String(position->y, 2) + String(F(")"))).c_str());
            u8g.drawStr(0, 38, F("Press STOP to change axis"));
            u8g.drawStr(0, 48, F("Press encoder to continue"));

        } while (u8g.nextPage());
        for (;;)
        {
            encoder.tick();
            previousEncoderPosition = currentEncoderPosition;
            currentEncoderPosition = encoder.getPosition();
            if (encoderButtonPressed())
            {
                debounceEncoderButton();
                shortBeep();
                pressed = true;
                while (yStepper.distanceToGo() != 0 or xStepper.distanceToGo() != 0)
                {
                    xStepper.run();
                    yStepper.run();
                }
                break;
            }
            else if (displayStopButtonPressed())
            {
                debounceDisplayStopButton();
                shortBeep();
                isCurrentlyOnXAxis = not isCurrentlyOnXAxis;
            }
            else if (currentEncoderPosition != previousEncoderPosition)
            {
                if (isCurrentlyOnXAxis)
                {
                    position->x -= 0.2 * (currentEncoderPosition - previousEncoderPosition);
                    if (position->x < 0)
                    {
                        position->x = 0;
                        shortBeep();
                    }
                    else if (position->x > X_MAX_POS)
                    {
                        position->x = X_MAX_POS;
                        shortBeep();
                    }
                }
                else
                {
                    position->y -= 0.2 * (currentEncoderPosition - previousEncoderPosition);
                    if (position->y < 0)
                    {
                        position->y = 0;
                        shortBeep();
                    }
                    else if (position->y > Y_MAX_POS)
                    {
                        position->y = Y_MAX_POS;
                        shortBeep();
                    }
                }
                moveToPositionAsync(position, 10);
                break;
            }
            xStepper.run();
            yStepper.run();
        }
        if (pressed)
            break;
    }
    bringMotorToEndstop(&eStepper, E_ENDSTOP, true);
}

void calibrationWizard()
{
    startOfCalibration:
    clearDisplay();
    u8g.firstPage();
    do
    {
        u8g.setFont(DISPLAY_FONT);
        u8g.drawStr(0, 16, F("Calibration wizard:"));
        u8g.setFont(DISPLAY_SMALLFONT);
        u8g.drawStr(0, 46, F("Press encoder to continue"));
        u8g.drawStr(0, 58, F("Press STOP to go back."));
    } while (u8g.nextPage());
    for (;;)
    {
        if (encoderButtonPressed())
        {
            debounceEncoderButton();
            shortBeep();
            break;
        }
        else if (displayStopButtonPressed())
        {
            debounceDisplayStopButton();
            shortBeep();
            return;
        }
    }
    u8g.firstPage();
    do
    {
        u8g.setFont(DISPLAY_FONT);
        u8g.drawStr(0, 12, F("Step 1: "));
        u8g.setFont(DISPLAY_SMALLFONT);
        u8g.drawStr(0, 24, F("Locate the H1 square"));
        u8g.drawStr(0, 36, F("and move extruder there"));
        u8g.drawStr(0, 48, F("using the encoder"));
        u8g.drawStr(0, 60, F("Press encoder to continue"));
    } while (u8g.nextPage());

    for (;;)
    {
        if (encoderButtonPressed())
        {
            debounceEncoderButton();
            shortBeep();
            break;
        }
        else if (displayStopButtonPressed())
        {
            debounceDisplayStopButton();
            shortBeep();
            return;
        }
    }
    positionSelectionMenu(&H1Pos);
    EEPROM.put(1, H1Pos);

    u8g.firstPage();
    do
    {
        u8g.setFont(DISPLAY_FONT);
        u8g.drawStr(0, 12, F("Step 2: "));
        u8g.setFont(DISPLAY_SMALLFONT);
        u8g.drawStr(0, 24, F("Locate the A1 square"));
        u8g.drawStr(0, 36, F("and move extruder there"));
        u8g.drawStr(0, 48, F("using the encoder"));
        u8g.drawStr(0, 60, F("Press encoder to continue"));
    } while (u8g.nextPage());

    Vector2 A1Pos = H1Pos + (OneSquareLeft * 7);
    positionSelectionMenu(&A1Pos);
    OneSquareLeft = (A1Pos - H1Pos) / 7;
    EEPROM.put(1 + 2 * sizeof(Vector2), OneSquareLeft);

    u8g.firstPage();
    do
    {
        u8g.setFont(DISPLAY_FONT);
        u8g.drawStr(0, 12, F("Step 3: "));
        u8g.setFont(DISPLAY_SMALLFONT);
        u8g.drawStr(0, 24, F("Locate the H8 square"));
        u8g.drawStr(0, 36, F("and move extruder there"));
        u8g.drawStr(0, 48, F("using the encoder"));
        u8g.drawStr(0, 60, F("Press encoder to continue"));
    } while (u8g.nextPage());

    Vector2 H8Pos = H1Pos + (OneSquareUp * 7);
    positionSelectionMenu(&H8Pos);
    OneSquareUp = (H8Pos - H1Pos) / 7;
    EEPROM.put(1 + sizeof(Vector2), OneSquareUp);

    u8g.firstPage();
    do
    {
        u8g.setFont(DISPLAY_FONT);
        u8g.drawStr(0, 12, F("Step 4: "));
        u8g.setFont(DISPLAY_SMALLFONT);
        u8g.drawStr(0, 24, F("The extruder will move"));
        u8g.drawStr(0, 36, F("to A8. Please press the"));
        u8g.drawStr(0, 48, F("encoder to proceed, or"));
        u8g.drawStr(0, 60, F("STOP to repeat calibration"));
    } while (u8g.nextPage());

    Vector2 A8Pos = H8Pos + OneSquareLeft * 7;

    moveToPosition(&A8Pos, PIECE_MOVE_SPEED);

    for (;;)
    {
        if (encoderButtonPressed())
        {
            debounceEncoderButton();
            shortBeep();
            break;
        }
        else if (displayStopButtonPressed())
        {
            debounceDisplayStopButton();
            shortBeep();
            goto startOfCalibration;  // sorry!
        }
    }

    moveToPosition(0, 0, PIECE_MOVE_SPEED);
}

#endif
