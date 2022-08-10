#ifndef MENU_H
#define MENU_H
#include "io.h"

#define LCD_ROWS 4

void clearDisplay(void)
{
    u8g.firstPage();
    do
    {
    } while (u8g.nextPage());
}

uint8_t displayMenuAndWaitForAnswer(uint8_t numEntries, char (*menuEntries)[18])
{
    uint8_t currentLine = 0;
    uint8_t currentMenuEntry = 0;
    bool pressed = false;
    int previousEncoderPosition = encoder.getPosition();
    int currentEncoderPosition = previousEncoderPosition;
    for (;;)
    {
        clearDisplay();
        u8g.firstPage();
        do
        {
            u8g.setFont(DISPLAY_FONT);
            for (uint8_t i = 0; i < LCD_ROWS; i++)
            {
                if (currentMenuEntry + i >= numEntries)
                {
                    break;
                }
                u8g.drawStr(16, 16 * i + 10, menuEntries[currentMenuEntry + i]);
            }
            u8g.drawStr(0, 16 * currentLine + 10, F(">"));
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
                break;
            }
            else if (currentEncoderPosition < previousEncoderPosition)
            {
                if (currentLine > 0)
                {
                    currentLine--;
                    break;
                }
                else if (currentMenuEntry > 0)
                {
                    currentMenuEntry--;
                    break;
                }
                else
                {
                    shortBeep();
                }
            }
            else if (currentEncoderPosition > previousEncoderPosition)
            {
                if (currentLine < min(LCD_ROWS - 1, numEntries - 1))
                {
                    currentLine++;
                    break;
                }
                else if (currentMenuEntry < numEntries - currentLine - 1)
                {
                    currentMenuEntry++;
                    break;
                }
                else
                {
                    shortBeep();
                }
            }
        }
        if (pressed)
            break;
    }
    return currentMenuEntry + currentLine;
}

void sliderMenu(String *title, int minimum, int maximum, int delta, int *result)
{
    double currentEncoderPosition = encoder.getPosition();
    double previousEncoderPosition = currentEncoderPosition;
    for (;;)
    {
        u8g.firstPage();
        do
        {
            u8g.setFont(DISPLAY_FONT);
            u8g.drawStr(0, 16, (*title).c_str());
            u8g.drawStr(0, 48, (String("Current pos: ") + String(*result)).c_str());
            u8g.drawStr(0, 64, F("Turn encoder to move."));
            xStepper.run();
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
                return;
            }
            else if (currentEncoderPosition != previousEncoderPosition)
            {
                *result += delta * (currentEncoderPosition - previousEncoderPosition);
                if (*result < minimum)
                {
                    *result = minimum;
                    shortBeep();
                }
                else if (*result > maximum)
                {
                    *result = maximum;
                    shortBeep();
                }
                break;
            }
        }
    }
}

void xAxisMovementMenu(double delta)
{
    double currentPosition = xStepper.currentPosition() / X_STEPS_PER_CM * -1;
    double currentEncoderPosition = encoder.getPosition();
    double previousEncoderPosition = currentEncoderPosition;
    bool pressed = false;
    xStepper.setAcceleration(50000);
    for (;;)
    {
        clearDisplay();
        u8g.firstPage();
        do
        {
            u8g.setFont(DISPLAY_FONT);
            u8g.drawStr(0, 16, F("Move X:"));
            u8g.drawStr(0, 48, (String("Current pos: ") + String(currentPosition, 1)).c_str());
            u8g.drawStr(0, 64, F("Turn encoder to move."));
            xStepper.run();
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
                while (xStepper.distanceToGo() != 0)
                {
                    xStepper.run();
                }
                break;
            }
            else if (currentEncoderPosition != previousEncoderPosition)
            {
                currentPosition -= delta * (currentEncoderPosition - previousEncoderPosition);
                if (currentPosition < 0)
                {
                    currentPosition = 0;
                    shortBeep();
                }
                else if (currentPosition > X_MAX_POS)
                {
                    currentPosition = X_MAX_POS;
                    shortBeep();
                }
                xStepper.moveTo(-1 * currentPosition * X_STEPS_PER_CM);
                xStepper.run();
                break;
            }
            xStepper.run();
        }
        if (pressed)
            break;
    }
    xStepper.setAcceleration(X_MAX_ACCEL);
}

void yAxisMovementMenu(double delta)
{
    double currentPosition = yStepper.currentPosition() / Y_STEPS_PER_CM * -1;
    double currentEncoderPosition = encoder.getPosition();
    double previousEncoderPosition = currentEncoderPosition;
    bool pressed = false;
    yStepper.setAcceleration(50000);
    for (;;)
    {
        clearDisplay();
        u8g.firstPage();
        do
        {
            u8g.setFont(DISPLAY_FONT);
            u8g.drawStr(0, 16, F("Move Y:"));
            u8g.drawStr(0, 48, (String("Current pos: ") + String(currentPosition, 1)).c_str());
            u8g.drawStr(0, 64, F("Turn encoder to move."));

            xStepper.run();
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
                while (yStepper.distanceToGo() != 0)
                {
                    yStepper.run();
                }
                break;
            }
            else if (currentEncoderPosition != previousEncoderPosition)
            {
                currentPosition -= delta * (currentEncoderPosition - previousEncoderPosition);
                if (currentPosition < 0)
                {
                    currentPosition = 0;
                    shortBeep();
                }
                else if (currentPosition > Y_MAX_POS)
                {
                    currentPosition = Y_MAX_POS;
                    shortBeep();
                }
                yStepper.moveTo(-1 * currentPosition * Y_STEPS_PER_CM);
                yStepper.run();
                break;
            }
            yStepper.run();
        }
        if (pressed)
            break;
    }
    yStepper.setAcceleration(Y_MAX_ACCEL);
}

void extruderMovementMenu(double delta)
{
    double currentPosition = eStepper.currentPosition() / E_STEPS_PER_CM;
    double currentEncoderPosition = encoder.getPosition();
    double previousEncoderPosition = currentEncoderPosition;
    bool pressed = false;
    for (;;)
    {
        clearDisplay();
        u8g.firstPage();
        do
        {
            u8g.setFont(DISPLAY_FONT);
            u8g.drawStr(0, 16, F("Move E:"));
            u8g.drawStr(0, 48, (String("Current pos: ") + String(currentPosition, 1)).c_str());
            u8g.drawStr(0, 64, F("Turn encoder to move."));

            xStepper.run();
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
                while (eStepper.distanceToGo() != 0)
                {
                    eStepper.run();
                }
                break;
            }
            else if (currentEncoderPosition != previousEncoderPosition)
            {
                currentPosition -= delta * (currentEncoderPosition - previousEncoderPosition);
                if (currentPosition < 0)
                {
                    currentPosition = 0;
                    shortBeep();
                }
                else if (currentPosition > E_MAX_POS)
                {
                    currentPosition = E_MAX_POS;
                    shortBeep();
                }
                eStepper.moveTo(currentPosition * E_STEPS_PER_CM);
                eStepper.run();
                break;
            }
            eStepper.run();
        }
        if (pressed)
            break;
    }
}

void displayMessage(String *message) {
    clearDisplay();
    u8g.firstPage();
    String lines[4];
    String delimiters = "\n\r";
    splitStringIntoTokens(message, 4, &delimiters, lines);
    do
    {
        u8g.setFont(DISPLAY_SMALLFONT);
        uint8_t yCoord = 0;
        for (uint8_t i = 0; i < 4; i++) {
            if (not lines[i].compareTo("")) {
                break;
            }
            yCoord += 16; 
            u8g.drawStr(0, yCoord, lines[i].c_str());
        }
    } while (u8g.nextPage());
}

void successfulMoveDisplay(String *move) {
    u8g.firstPage();
    do
    {
        u8g.setFont(DISPLAY_SMALLFONT);
        u8g.drawStr(0, 16, "Successfully made move:");
        u8g.setFont(DISPLAY_FONT);
        u8g.drawStr(0, 30, move->c_str());
    } while (u8g.nextPage());
}

void dialogueBox(String *data)
{
    clearDisplay();
    u8g.firstPage();
    String lines[4];
    String delimiters = "\n\r";
    splitStringIntoTokens(data, 4, &delimiters, lines);
    do
    {
        u8g.setFont(DISPLAY_SMALLFONT);
        uint8_t yCoord = 0;
        for (uint8_t i = 0; i < 4; i++) {
            if (not lines[i].compareTo("")) {
                break;
            }
            yCoord += 16; 
            u8g.drawStr(0, yCoord, lines[i].c_str());
        }
        u8g.drawStr(0, 64, F("Press encoder to continue"));
    } while (u8g.nextPage());
    while (not encoderButtonPressed());
    debounceEncoderButton();
    shortBeep();
}

#endif