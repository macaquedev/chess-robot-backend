#include "io.h"
#include "menu.h"
#include "initialisation.h"

void playGame()
{

    char menuEntries[][18] = {"Play against AI", "Play Online"};
    uint8_t answer = displayMenuAndWaitForAnswer(2, menuEntries);
    debounceEncoderButton();
    switch (answer)
    {
    case 0:
    {
        String title = "AI Difficulty";
        int result = 1500;
        sliderMenu(&title, 1000, 3000, 100, &result);
        moveToCorner();
        pc.sendData(String("STOCKFISH ") + String(result));
        pc.waitForAcknowledgement();
        break;
    }

    case 1:
    {
        pc.sendData(String("LICHESS"));
        pc.waitForAcknowledgement();
        digitalWrite(13, HIGH);
        delay(1000);
        digitalWrite(13, LOW);
        moveToCorner();
        break;
    }

    default:
        break;
    }

    String pcData;
    uint8_t moveCount = 0;
    for (;;)
    {
        pc.waitForData();
        pc.readData(&pcData);
        if (pcData.startsWith(".OK."))
        {
            pcData = pcData.substring(5);
            successfulMoveDisplay(&pcData);
            pc.sendAcknowledgement();
        }
        else if (pcData.startsWith("."))
        {
            pcData = pcData.substring(1);
            displayMessage(&pcData);
            pc.sendAcknowledgement();
        }
        else if (pcData.startsWith("_."))
        {
            pcData = pcData.substring(2);
            dialogueBox(&pcData);
            pc.sendAcknowledgement();
        }
        else if (pcData.startsWith("__."))
        {
            pcData = pcData.substring(3);
            dialogueBox(&pcData);
            pc.sendAcknowledgement();
            homeMotors();
            break;
        }
        else if (pcData.startsWith("True") or pcData.startsWith("False"))
        {
            if (++moveCount == 5)
            {
                moveCount = 0;
                homeMotors();
            }
            makeMove(&pcData);
            moveToCorner();
            pc.sendAcknowledgement();
        }
    }
}