#include "initialisation.h"
#include "game.h"

void setup() {
    String text = F("ChessRobotOS 1.0\nConnecting to PC...");
    displayMessage(&text);
    pc.begin();
    text = "ChessRobotOS 1.0\nConnected!";
    displayMessage(&text);
    delay(1000);
    text = "ChessRobotOS 1.0\nHoming motors...";
    displayMessage(&text);
    delay(1000);

    pinMode(X_ENDSTOP, INPUT_PULLUP);
    pinMode(Y_ENDSTOP, INPUT_PULLUP);
    pinMode(E_ENDSTOP, INPUT_PULLUP);

    pinMode(X_STEPPER_EN, OUTPUT);
    pinMode(Y_STEPPER_EN, OUTPUT);
    pinMode(E_STEPPER_EN, OUTPUT);
    pinMode(ELECTROMAGNET_PIN, OUTPUT);

    digitalWrite(X_STEPPER_EN, LOW);
    digitalWrite(Y_STEPPER_EN, LOW);
    #ifndef V1_ARM
    digitalWrite(E_STEPPER_EN, LOW);
    #endif
    digitalWrite(ELECTROMAGNET_PIN, LOW);

    pinMode(STOP_BUTTON, INPUT_PULLUP);
    pinMode(ENCODER_BUTTON, INPUT_PULLUP);

    pinMode(BEEPER_PIN, OUTPUT);

    resetSpeedsAndAccelerations();

    fillEEPROM();

    homeMotors();
}

void loop()
{
    char menuEntries[][18] = {"Prepare", "Play"};
    uint8_t answer = displayMenuAndWaitForAnswer(2, menuEntries);
    switch (answer)
    {
    case 0:
        prepareMenu();
        break;

    case 1:
    {
        playGame();
        break;
    }

    default:
        break;
    }
}
