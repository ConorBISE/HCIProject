#include "ancsClient.h"

#include "BLEDevice.h"
#include "ancs.h"

static char LOG_TAG[] = "ANCSClient";

namespace ancs {

void ANCSClient::onDataSourceCharacteristicNotify(
        BLERemoteCharacteristic* pDataSourceCharacteristic,
        uint8_t* pData,
        size_t length,
        bool isNotify
) {
    uint32_t notificationUid = *((uint32_t*) &pData[1]);
    
    // Skip command id, notification ID
    int i = 5;

    Notification* notification = new Notification;

    while (i < length) {
        NotificationAttributeID attrId = (NotificationAttributeID) pData[i];
        uint16_t attrLen = (uint16_t) pData[i + 1];

        String attrString = String((uint8_t*) &pData[i + 3], attrLen);
        
        switch (attrId) {
            case NotificationAttributeID::AppIdentifier:
                notification->appIdentifier = attrString;
                break;

            case NotificationAttributeID::Title:
                notification->title = attrString;
                break;

            case NotificationAttributeID::Message:
                notification->message = attrString;
                break;

            case NotificationAttributeID::Date:
                notification->date = attrString;
                break;
        }
    
        i += attrLen + 3;
    }

    ancsServer->getNotificationCallback()(notification);
}

/**
 * Become a BLE client to a remote BLE server.  We are passed in the address of the BLE server
 * as the input parameter when the task is created.
 */
void ANCSClient::run(void* data) {

    BLEAddress* pAddress = (BLEAddress*)data;
    client  = BLEDevice::createClient();
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);

    BLESecurity *security = new BLESecurity();
    security->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    security->setCapability(ESP_IO_CAP_IO);
    security->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    // Connect to the remove BLE Server.
    client->connect(*pAddress);

    /** BEGIN ANCS SERVICE **/
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pAncsService = client->getService(ancsServiceUUID);
    if (pAncsService == nullptr) {
        ESP_LOGD(LOG_TAG, "Failed to find our service UUID: %s", ancsServiceUUID.toString().c_str());
        return;
    }
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    BLERemoteCharacteristic* pNotificationSourceCharacteristic = pAncsService->getCharacteristic(notificationSourceCharacteristicUUID);
    if (pNotificationSourceCharacteristic == nullptr) {
        ESP_LOGD(LOG_TAG, "Failed to find our characteristic UUID: %s", notificationSourceCharacteristicUUID.toString().c_str());
        return;
    }        
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    BLERemoteCharacteristic* pControlPointCharacteristic = pAncsService->getCharacteristic(controlPointCharacteristicUUID);
    if (pControlPointCharacteristic == nullptr) {
        ESP_LOGD(LOG_TAG, "Failed to find our characteristic UUID: %s", controlPointCharacteristicUUID.toString().c_str());
        return;
    }        
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    BLERemoteCharacteristic* pDataSourceCharacteristic = pAncsService->getCharacteristic(dataSourceCharacteristicUUID);
    if (pDataSourceCharacteristic == nullptr) {
        ESP_LOGD(LOG_TAG, "Failed to find our characteristic UUID: %s", dataSourceCharacteristicUUID.toString().c_str());
        return;
    }

    pDataSourceCharacteristic->registerForNotify([this](BLERemoteCharacteristic* pDataSourceCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
        onDataSourceCharacteristicNotify(pDataSourceCharacteristic, pData, length, isNotify);
    });

    pNotificationSourceCharacteristic->registerForNotify([this](BLERemoteCharacteristic* pDataSourceCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
        ANCSNotificationSourceResponse* message = (ANCSNotificationSourceResponse*) pData;
    
        if(message->eventId == EventID::NotificationAdded) {
            notificationQueue.push_back(message->notificationUid);
        }
    });
    
    while (true) {
        while (notificationQueue.size() > 0) {
            uint32_t notificationId = notificationQueue.front();
            uint8_t* notificationIdBytes = (uint8_t*) &notificationId;

            const uint8_t vIdentifier[] = {
                0x0, // Command id
                notificationIdBytes[0], notificationIdBytes[1], notificationIdBytes[2], notificationIdBytes[3],
                (uint8_t) NotificationAttributeID::AppIdentifier,
                (uint8_t) NotificationAttributeID::Title, 0x0, 0x10,
                (uint8_t) NotificationAttributeID::Message, 0x0, 0x10,
                (uint8_t) NotificationAttributeID::Date, 0x0, 0x5
            };
            
            pControlPointCharacteristic->writeValue((uint8_t*)vIdentifier, 15, true);
            notificationQueue.pop_front();
        }

        delay(100);
    }        
} // ANCSClient::run

}