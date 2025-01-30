#include <Arduino.h>
#include "ancs/ancs.h" 

ancs::ANCSServer ancsServer;

void setup()
{
    Serial.begin(115200);
    ancsServer.run();
}
void loop()
{
}
