#ifndef IO_H
#define IO_H
#include "config.h"

bool encoderButtonPressed()
{
    return not digitalRead(ENCODER_BUTTON);
}

bool displayStopButtonPressed()
{
    return not digitalRead(STOP_BUTTON);
}

void debounceEncoderButton()
{
    while (encoderButtonPressed())
        ;
}

void debounceDisplayStopButton()
{
    while (displayStopButtonPressed())
        ;
}

void shortBeep()
{
    digitalWrite(BEEPER_PIN, HIGH);
    delay(100);
    digitalWrite(BEEPER_PIN, LOW);
}

void longBeep()
{
    digitalWrite(BEEPER_PIN, HIGH);
    delay(1000);
    digitalWrite(BEEPER_PIN, LOW);
}

void bringMotorToEndstop(AccelStepper *motor, uint16_t endstop, bool reverseDirections)
{
    motor->move(100000 * (reverseDirections ? -1 : 1));
    while (digitalRead(endstop))
    {
        motor->run();
    }
    motor->move(0);
    motor->setSpeed(0);
    motor->setCurrentPosition(0);
}

void pickUpPiece(Pieces piece)
{
    eStepper.move(PieceHeights[piece] * E_STEPS_PER_CM);
    while (eStepper.distanceToGo() > 0)
    {
        eStepper.run();
    }
    currentPosition.e = PieceHeights[piece] * E_STEPS_PER_CM;
    digitalWrite(ELECTROMAGNET_PIN, HIGH);
    delay(200);

    bringMotorToEndstop(&eStepper, E_ENDSTOP, true);
    currentPosition.e = 0;
}

void putDownPiece(Pieces piece)
{
    eStepper.move((PieceHeights[piece] - 0.1) * E_STEPS_PER_CM);
    while (eStepper.distanceToGo() > 0)
    {
        eStepper.run();
    }
    currentPosition.e = (PieceHeights[piece] - 0.1) * E_STEPS_PER_CM;
    digitalWrite(ELECTROMAGNET_PIN, LOW);
    delay(100);

    bringMotorToEndstop(&eStepper, E_ENDSTOP, true);
    currentPosition.e = 0;
}

void moveToPositionAsync(double x, double y, double speed)
{
    int xSteps = -x * X_STEPS_PER_CM;
    int ySteps = -y * Y_STEPS_PER_CM;
    long dx = abs(xSteps - xStepper.currentPosition());
    long dy = abs(ySteps - yStepper.currentPosition());
    double xSpeed, ySpeed, xAccel, yAccel;
    if ((dx == 0) and (dy == 0))
    {
        return;
    }
    if (dx == 0)
    {
        ySpeed = speed * X_STEPS_PER_CM;
        yAccel = Y_MAX_ACCEL;
        yStepper.setMaxSpeed(ySpeed);
        yStepper.setAcceleration(yAccel);
        yStepper.moveTo(ySteps <= 0 ? ySteps : 0);
    }
    else if (dy == 0)
    {
        xSpeed = speed * Y_STEPS_PER_CM;
        xAccel = X_MAX_ACCEL;
        xStepper.setMaxSpeed(xSpeed);
        xStepper.setAcceleration(xAccel);
        xStepper.moveTo(xSteps <= 0 ? xSteps : 0);
    }
    else
    {
        double dp = sqrt(dx * dx + dy * dy);

        xSpeed = speed / dp * dx * X_STEPS_PER_CM;
        ySpeed = speed / dp * dy * Y_STEPS_PER_CM;

        xAccel = X_MAX_ACCEL / dp * dx;
        yAccel = Y_MAX_ACCEL / dp * dy;

        xStepper.setMaxSpeed(xSpeed);
        yStepper.setMaxSpeed(ySpeed);
        xStepper.setAcceleration(xAccel);
        yStepper.setAcceleration(yAccel);
        xStepper.moveTo(xSteps <= 0 ? xSteps : 0);
        yStepper.moveTo(ySteps <= 0 ? ySteps : 0);
    }

    currentPosition.x = x;
    currentPosition.y = y;
}

void moveToPositionAsync(Vector2 *position, double speed)
{
    moveToPositionAsync(position->x, position->y, speed);
}

void moveToPosition(double x, double y, double speed)
{
    moveToPositionAsync(x, y, speed);
    while (xStepper.distanceToGo() != 0 or yStepper.distanceToGo() != 0)
    {
        xStepper.run();
        yStepper.run();
    }

    resetSpeedsAndAccelerations();
}

void moveToPosition(Vector2 *position, double speed)
{
    moveToPosition(position->x, position->y, speed);
}

void moveToSquare(uint8_t square, double speed)
{
    uint8_t rank = square >> 3;
    uint8_t file = square & 7;

    Vector2 target = H1Pos + OneSquareLeft * (7 - file) + OneSquareUp * rank;
    moveToPosition(&target, speed);
}

void moveToSquareAsync(uint8_t square, double speed)
{
    uint8_t rank = square >> 3;
    uint8_t file = square - (rank << 3);

    Vector2 target = H1Pos + OneSquareLeft * (7 - file) + OneSquareUp * rank;
    moveToPositionAsync(&target, speed);
}

void splitStringIntoTokens(String *input, int numTokens, String *delimiters, String tokens[])
{
    int currentToken = 0;
    int currentIndex = 0;
    char currentChar = '\0';
    while (currentToken < numTokens)
    {
        currentChar = input->charAt(currentIndex++);
        if (delimiters->indexOf(currentChar) != -1)
        {
            currentToken++;
            continue;
        }
        tokens[currentToken] += currentChar;
    }
}

void castle(uint8_t kingStartSquare, uint8_t kingEndSquare, uint8_t rookStartSquare, uint8_t rookEndSquare)
{
    moveToSquare(kingStartSquare, PIECE_MOVE_SPEED);
    pickUpPiece(Pieces::KING);
    moveToSquare(kingEndSquare, PIECE_MOVE_SPEED);
    putDownPiece(Pieces::KING);
    moveToSquare(rookStartSquare, PIECE_MOVE_SPEED);
    pickUpPiece(Pieces::ROOK);
    moveToSquare(rookEndSquare, PIECE_MOVE_SPEED);
    putDownPiece(Pieces::ROOK);
}

void dumpPiece(uint8_t square, Pieces pieceType)
{
    moveToSquare(square, PIECE_MOVE_SPEED);
    pickUpPiece(pieceType);
    moveToPosition(X_MAX_POS, Y_MAX_POS, PIECE_MOVE_SPEED);
    digitalWrite(ELECTROMAGNET_PIN, LOW);
}

Pieces characterToPieceType(String *s)
{
    char piece = s->charAt(0);
    switch (piece)
    {
    case 'P':
        return Pieces::PAWN;
    case 'N':
        return Pieces::KNIGHT;
    case 'B':
        return Pieces::BISHOP;
    case 'R':
        return Pieces::ROOK;
    case 'Q':
        return Pieces::QUEEN;
    case 'K':
        return Pieces::KING;
    default:
        return Pieces::PAWN;
    }
}

void movePiece(Pieces pieceType, uint8_t startingSquare, uint8_t endingSquare)
{
    moveToSquare(startingSquare, PIECE_MOVE_SPEED);
    pickUpPiece(pieceType);
    moveToSquare(endingSquare, PIECE_MOVE_SPEED);
    putDownPiece(pieceType);
}

void moveToCorner(bool colour)
{
    moveToPosition(0, colour ? Y_MAX_POS : 0, PIECE_MOVE_SPEED);
}

void makeMove(String *data)
{
    String tokens[7];
    String delimiters = "\r ";
    splitStringIntoTokens(data, 7, &delimiters, tokens);
    String *pieceColour = &tokens[0];
    String *pieceType = &tokens[1];
    String *startPos = &tokens[2];
    String *endPos = &tokens[3];
    String *capturedPiece = &tokens[4];
    String *givesCheck = &tokens[5];
    String *enPassantSquare = &tokens[6];

    if (not pieceType->compareTo("C"))
    {
        if (not startPos->compareTo("QUEENSIDE"))
        {
            if (not pieceColour->compareTo("True"))
                castle(4, 2, 0, 3);
            else
                castle(60, 58, 56, 59);
        }
        else
        {
            if (not pieceColour->compareTo("True"))
                castle(4, 6, 7, 5);
            else
                castle(60, 62, 63, 61);
        }
    }
    else
    {
        if (capturedPiece->compareTo("False"))
        {
            dumpPiece(endPos->toInt(), characterToPieceType(capturedPiece));
        }
        movePiece(characterToPieceType(pieceType), startPos->toInt(), endPos->toInt());
        if (enPassantSquare->compareTo("False"))
        {
            dumpPiece(enPassantSquare->toInt(), Pieces::PAWN);
        }
    }
    if (givesCheck->compareTo("False"))
    {
        longBeep();
    }
}

#endif