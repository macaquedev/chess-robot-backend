#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <AccelStepper.h>
#include <U8glib.h>
#include <RotaryEncoder.h>
#include <EEPROM.h>

#include "pythonCommunication.h"

#define ENCODER_PIN_1 31
#define ENCODER_PIN_2 33
#define ENCODER_BUTTON 35
#define STOP_BUTTON 41
#define BEEPER_PIN 37

#define X_STEPPER_STEP A0
#define X_STEPPER_DIR A1
#define X_STEPPER_EN 38

#define Y_STEPPER_STEP 46
#define Y_STEPPER_DIR 48
#define Y_STEPPER_EN A8

// #define V1_ARM

#ifdef V1_ARM
#define E_STEPPER_PINS A12, A10, 44, A5
#define E_STEPS_PER_CM 512.8

#define E_MAX_SPEED 800
#define E_MAX_ACCEL 700
#define E_MAX_POS 9

#else 
#define E_STEPPER_STEP A6
#define E_STEPPER_DIR A7 
#define E_STEPPER_EN A2 
#define E_STEPS_PER_CM 45

#define E_MAX_SPEED 500
#define E_MAX_ACCEL 15000
#define E_MAX_POS 8.5

#endif

#define X_STEPS_PER_CM 51
#define Y_STEPS_PER_CM 51

#define X_ENDSTOP 3
#define Y_ENDSTOP 14
#define E_ENDSTOP 40

#define X_MAX_SPEED 500
#define X_MAX_ACCEL 4000
#define Y_MAX_SPEED 500
#define Y_MAX_ACCEL 4000


#define X_MAX_POS 35
#define Y_MAX_POS 28 // so the extruder doesn't hit the camera mount!

#define DISPLAY_PINS 23, 17, 16
#define DISPLAY_FONT u8g_font_profont11
#define DISPLAY_SMALLFONT u8g_font_profont10

#define PIECE_MOVE_SPEED 25

#define ELECTROMAGNET_PIN A9

enum Pieces
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

class Vector2 : public Printable
{
public:
    double x, y;
    Vector2(double x, double y) : x(x), y(y) {}
    size_t printTo(Print &p) const
    {
        size_t r = 0;

        r += p.print("(x=");
        r += p.print(x);
        r += p.print(", y=");
        r += p.print(y);
        r += p.print(")");
        return r;
    }
    Vector2 operator+(Vector2 other)
    {
        return Vector2(x + other.x, y + other.y);
    }
    Vector2 operator-(Vector2 other)
    {
        return Vector2(x - other.x, y - other.y);
    }
    Vector2 operator*(double other)
    {
        return Vector2(x * other, y * other);
    }
    Vector2 operator/(double other)
    {
        return Vector2(x / other, y / other);
    }
};

class Vector3 : public Printable
{
public:
    double x, y, e;
    Vector3(double x, double y, double e) : x(x), y(y), e(e) {}
    size_t printTo(Print &p) const
    {
        size_t r = 0;

        r += p.print("(x=");
        r += p.print(x);
        r += p.print(", y=");
        r += p.print(y);
        r += p.print(", e=");
        r += p.print(e);
        r += p.print(")");
        return r;
    }
    Vector3 operator+(Vector3 other)
    {
        return Vector3(x + other.x, y + other.y, e + other.e);
    }
    Vector3 operator+(Vector2 other)
    {
        return Vector3(x + other.x, y + other.y, e);
    }
    Vector3 operator-(Vector3 other)
    {
        return Vector3(x - other.x, y - other.y, e - other.e);
    }
    Vector3 operator-(Vector2 other)
    {
        return Vector3(x - other.x, y - other.y, e);
    }
};

Vector3 currentPosition(0, 0, 0);

double PieceHeights[] = {7.5, 6.6, 5.4, 7.05, 4.7, 4.1};

Vector2 H1Pos(5, 5);
Vector2 OneSquareUp(0, 3); 
Vector2 OneSquareLeft(3, 0);

// X motor + to go right, - to go left.
// Y motor - to go forwards, + to go backwards.
// buttons need to be pullup

AccelStepper xStepper(AccelStepper::MotorInterfaceType::DRIVER, X_STEPPER_STEP, X_STEPPER_DIR);
AccelStepper yStepper(AccelStepper::MotorInterfaceType::DRIVER, Y_STEPPER_STEP, Y_STEPPER_DIR);

#ifdef V1_ARM
AccelStepper eStepper(AccelStepper::MotorInterfaceType::FULL4WIRE, E_STEPPER_PINS);
#else
AccelStepper eStepper(AccelStepper::MotorInterfaceType::DRIVER, E_STEPPER_STEP, E_STEPPER_DIR);
#endif

U8GLIB_ST7920_128X64_1X u8g(DISPLAY_PINS);

RotaryEncoder encoder(ENCODER_PIN_1, ENCODER_PIN_2, RotaryEncoder::LatchMode::TWO03);

PythonCommunication pc;

#define LCDWidth u8g.getDisplayWidth()
#define ALIGN_CENTER(t) ((LCDWidth - (u8g2.getUTF8Width(t))) / 2)
#define ALIGN_RIGHT(t) (LCDWidth - u8g2.getUTF8Width(t))
#define ALIGN_LEFT 0

void resetSpeedsAndAccelerations()
{
    xStepper.setMaxSpeed(X_MAX_SPEED);
    yStepper.setMaxSpeed(Y_MAX_SPEED);
    eStepper.setMaxSpeed(E_MAX_SPEED);

    xStepper.setAcceleration(X_MAX_ACCEL);
    yStepper.setAcceleration(Y_MAX_ACCEL);
    eStepper.setAcceleration(E_MAX_ACCEL);
}

#endif