#include "pythonCommunication.h"

PythonCommunication::PythonCommunication()
{
}

void PythonCommunication::begin()
{
    Serial.begin(115200);
    sendAcknowledgement();
    waitForAcknowledgement();
}

void PythonCommunication::readData(String *data)
{
    *data = Serial.readString();
}

void PythonCommunication::waitForData()
{
    while (!Serial.available())
        ;
}

void PythonCommunication::sendData(String data)
{
    Serial.println(data.c_str());
}

void PythonCommunication::sendAcknowledgement()
{
    sendData(String("OK"));
}

void PythonCommunication::waitForAcknowledgement()
{
    String data;
    for (;;)
    {
        waitForData();
        readData(&data); 
        Serial.println(data);
        if (data == String("OK\r"))
        {
            return;
        }
    }
}