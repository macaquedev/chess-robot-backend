#include "io.h"
#include "menu.h"
#include "initialisation.h"


void playGame()
{
    debounceEncoderButton();
    String title = "AI Difficulty";
    String pcData;
    int result = 1500;
    sliderMenu(&title, 1000, 3000, 100, &result);
    moveToCorner();
    pc.sendData(String("STOCKFISH ") + String(result));
    pc.waitForAcknowledgement();
    uint8_t moveCount = 0;
    for (;;)
    {
        pc.waitForData();
        pc.readData(&pcData);
        if (pcData.startsWith(".OK.")) {
            pcData = pcData.substring(5);
            successfulMoveDisplay(&pcData);
            pc.sendAcknowledgement();
        }
        else if (pcData.startsWith(".")) {
            pcData = pcData.substring(1);
            displayMessage(&pcData);
            pc.sendAcknowledgement();
        }
        else if (pcData.startsWith("_.")) {
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
        else if (pcData.startsWith("True") or pcData.startsWith("False")) {
            if (++moveCount == 5) {
                moveCount = 0;
                homeMotors();
            }
            makeMove(&pcData);
            moveToCorner();
            pc.sendAcknowledgement();
        }
    }
}