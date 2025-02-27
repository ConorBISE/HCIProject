#ifndef ANCS_CLIENT_H
#define ANCS_CLIENT_H

#include "Task.h"
#include "ancs.h"
#include <deque>

namespace ancs {

class ANCSClient: public Task {
public:
    ANCSClient(ANCSServer* ancsServer) : ancsServer{ancsServer} {};

private:
    void run(void* data) override;
    void onDataSourceCharacteristicNotify(BLERemoteCharacteristic* pDataSourceCharacteristic, uint8_t* pData, size_t length, bool isNotify);

    ANCSServer* ancsServer;
    BLEClient* client;
    bool pendingNotification = false;
    std::deque<uint32_t> notificationQueue;
};

}

#endif // ANCS_CLIENT_H