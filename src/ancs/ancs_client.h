#ifndef ANCS_CLIENT_H
#define ANCS_CLIENT_H

#include "Task.h"
#include "ancs.h"

namespace ancs {

class ANCSClient: public Task {
public:
    ANCSClient(ANCSServer* ancsServer) : ancsServer{ancsServer} {};

private:
    void run(void* data) override;
    void onDataSourceCharacteristicNotify(BLERemoteCharacteristic* pDataSourceCharacteristic, uint8_t* pData, size_t length, bool isNotify);
    void onNotificationCharacteristicNotify(BLERemoteCharacteristic* pDataSourceCharacteristic, uint8_t* pData, size_t length, bool isNotify);

    ANCSServer* ancsServer;
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

}

#endif // ANCS_CLIENT_H