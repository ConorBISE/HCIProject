#ifndef ANCS_H
#define ANCS_H

#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEClient.h"
#include "BLEUtils.h"
#include "BLE2902.h"

namespace ancs {

static BLEUUID ancsServiceUUID("7905F431-B5CE-4E99-A40F-4B1E122D00D0");
static BLEUUID notificationSourceCharacteristicUUID("9FBF120D-6301-42D9-8C58-25E699A21DBD");
static BLEUUID controlPointCharacteristicUUID("69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9");
static BLEUUID dataSourceCharacteristicUUID("22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB");

typedef void (*notification_callback_t) ();

class ANCSServer {
public:
    void setNotificationCallback(notification_callback_t notificationCallback) {
        this->notificationCallback = notificationCallback;
    }

    notification_callback_t getNotificationCallback() { return notificationCallback; };
    
    void run();

private:
    void _setServiceSolicitation(BLEAdvertisementData *a, BLEUUID uuid);
    notification_callback_t notificationCallback;
};

}

#endif // ANCS_H