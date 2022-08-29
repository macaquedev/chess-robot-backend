#include "io.h"
#include "menu.h"
#include "initialisation.h"

void playGame()
{

    char menuEntries[][18] = {"Play against AI", "Play Online"};
    uint8_t answer = displayMenuAndWaitForAnswer(2, menuEntries);
    debounceEncoderButton();
    bool colour = true;
    switch (answer)
    {
    case 0:
    {
        String title = "AI Difficulty";
        int result = 1500;
        sliderMenu(&title, 1000, 3000, 100, &result);
        colour = selectColourMenu();
        moveToCorner(colour);
        pc.sendData(String("STOCKFISH ") + String(result) + String(" ") + String(colour));
        break;
    }

    case 1:
    {
        pc.sendData(String("LICHESS"));
        pc.waitForData();
        String pcData;
        pc.readData(&pcData);
        pcData = pcData.substring(1);
        displayMessage(&pcData);
        pc.sendAcknowledgement();
        pc.waitForData();
        pc.readData(&pcData);
        colour = (bool)pcData.toInt();
        moveToCorner(colour);
        pc.sendAcknowledgement();
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
            moveToCorner(colour);
            pc.sendAcknowledgement();
        }
        else if (pcData.startsWith("1")) {
            colour = true;
        }
        else if (pcData.startsWith("0")) {
            colour = false;
        }
    }
}