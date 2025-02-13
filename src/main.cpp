#include <Arduino.h>
#include "ancs/ancs.h" 

ancs::ANCSServer ancsServer;

void notificationCallback() {
  Serial.println("Notification!");
}

void setup()
{
    Serial.begin(115200);
    ancsServer.setNotificationCallback(notificationCallback);
    ancsServer.run();
}
void loop()
{

}
