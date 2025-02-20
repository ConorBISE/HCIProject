#include <Arduino.h>
#include "ancs/ancs.h" 

ancs::ANCSServer ancsServer;

void notificationCallback(ancs::Notification* notification) {
  Serial.print("Notification: ");
  Serial.println(notification->message);
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
