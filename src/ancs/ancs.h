#ifndef ANCS_H
#define ANCS_H

#include <Arduino.h>
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

struct Notification {
    uint32_t notificationUid;
    String appIdentifier;
    String title;
    String message;
    String date;
};

typedef void (*notification_callback_t) (Notification*);

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

enum class EventID: uint8_t {
    NotificationAdded = 0,
    NotificationModified = 1,
    NotificationRemoved = 2
};

enum class CategoryID: uint8_t {
    Other = 0,
    IncomingCall = 1,
    MissedCall = 2,
    Voicemail = 3,
    Social = 4,
    Schedule = 5,
    Email = 6,
    News = 7,
    HealthAndFitness = 8,
    BusinessAndFinance = 9,
    Location = 10,
    Entertainment = 11
};

struct ANCSNotificationSourceResponse {
    EventID eventId;
    uint8_t eventFlags; 
    CategoryID categoryId;
    uint8_t categoryCount;
    uint32_t notificationUid;
};

enum class NotificationAttributeID: uint8_t {
    AppIdentifier = 0,
    Title = 1,
    Subtitle = 2,
    Message = 3,
    MessageSize = 4,
    Date = 5,
    PositiveActionLabel = 6,
    NegativeActionLabel = 7
};

}

#endif // ANCS_H