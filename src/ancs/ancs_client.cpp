#include <Arduino.h>
#include "BLEDevice.h"

#include "ancs_client.h"
#include "ancs.h"

namespace ancs {

uint8_t latestMessageID[4];
boolean pendingNotification = false;
uint8_t acceptCall = 0;

void ANCSClient::onDataSourceCharacteristicNotify(
        BLERemoteCharacteristic* pDataSourceCharacteristic,
        uint8_t* pData,
        size_t length,
        bool isNotify
) {
    for(int i = 0; i < length; i++){
        if(i > 7){
            Serial.write(pData[i]);
        }
    }
    Serial.println();
}

void ANCSClient::onNotificationCharacteristicNotify(
        BLERemoteCharacteristic* pDataSourceCharacteristic,
        uint8_t* pData,
        size_t length,
        bool isNotify
) {
    ANCSNotificationSourceResponse* message = (ANCSNotificationSourceResponse*) pData;

    if(message->eventId == EventID::NotificationAdded)
    {
        Serial.println("New notification!");

        ancsServer->getNotificationCallback()();

        latestMessageID[0] = pData[4];
        latestMessageID[1] = pData[5];
        latestMessageID[2] = pData[6];
        latestMessageID[3] = pData[7];
            
        switch(message->categoryId)
        {
            case CategoryID::Other:
                Serial.println("Category: Other");
            break;
            case CategoryID::IncomingCall:
                Serial.println("Category: Incoming call");
            break;
            case CategoryID::MissedCall:
                Serial.println("Category: Missed call");
            break;
            case CategoryID::Voicemail:
                Serial.println("Category: Voicemail");
            break;
            case CategoryID::Social:
                Serial.println("Category: Social");
            break;
            case CategoryID::Schedule:
                Serial.println("Category: Schedule");
            break;
            case CategoryID::Email:
                Serial.println("Category: Email");
            break;
            case CategoryID::News:
                Serial.println("Category: News");
            break;
            case CategoryID::HealthAndFitness:
                Serial.println("Category: Health");
            break;
            case CategoryID::BusinessAndFinance:
                Serial.println("Category: Business");
            break;
            case CategoryID::Location:
                Serial.println("Category: Location");
            break;
            case CategoryID::Entertainment:
                Serial.println("Category: Entertainment");
            break;
            default:
            break;
        }
        }
    else if(message->eventId == EventID::NotificationModified)
    {
        Serial.println("Notification Modified!");
        if (message->categoryId == CategoryID::IncomingCall) {
            Serial.println("Call Changed!");
        }
    }
    else if(message->eventId == EventID::NotificationRemoved)
    {
        Serial.println("Notification Removed!");
        if (message->categoryId == CategoryID::IncomingCall) {
            Serial.println("Call Gone!");
        }
    }
    pendingNotification = true;
}

/**
 * Become a BLE client to a remote BLE server.  We are passed in the address of the BLE server
 * as the input parameter when the task is created.
 */
void ANCSClient::run(void* data) {

    BLEAddress* pAddress = (BLEAddress*)data;
    BLEClient*  pClient  = BLEDevice::createClient();
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);

    BLESecurity *security = new BLESecurity();
    security->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    security->setCapability(ESP_IO_CAP_IO);
    security->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    // Connect to the remove BLE Server.
    pClient->connect(*pAddress);

    /** BEGIN ANCS SERVICE **/
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pAncsService = pClient->getService(ancsServiceUUID);
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
    const uint8_t v[]={0x1,0x0};

    pDataSourceCharacteristic->registerForNotify([this](BLERemoteCharacteristic* pDataSourceCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
        onDataSourceCharacteristicNotify(pDataSourceCharacteristic, pData, length, isNotify);
    });

    pDataSourceCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)v,2,true);

    pNotificationSourceCharacteristic->registerForNotify([this](BLERemoteCharacteristic* pDataSourceCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
        onNotificationCharacteristicNotify(pDataSourceCharacteristic, pData, length, isNotify);
    });
    
    pNotificationSourceCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)v,2,true);
    /** END ANCS SERVICE **/

    while(1){
        if(pendingNotification){
            // CommandID: CommandIDGetNotificationAttributes
            // 32bit uid
            // AttributeID
            Serial.println("Requesting details...");
            const uint8_t vIdentifier[]={0x0,   latestMessageID[0],latestMessageID[1],latestMessageID[2],latestMessageID[3],   0x0};
            pControlPointCharacteristic->writeValue((uint8_t*)vIdentifier,6,true);
            const uint8_t vTitle[]={0x0,   latestMessageID[0],latestMessageID[1],latestMessageID[2],latestMessageID[3],   0x1, 0x0, 0x10};
            pControlPointCharacteristic->writeValue((uint8_t*)vTitle,8,true);
            const uint8_t vMessage[]={0x0,   latestMessageID[0],latestMessageID[1],latestMessageID[2],latestMessageID[3],   0x3, 0x0, 0x10};
            pControlPointCharacteristic->writeValue((uint8_t*)vMessage,8,true);
            const uint8_t vDate[]={0x0,   latestMessageID[0],latestMessageID[1],latestMessageID[2],latestMessageID[3],   0x5};
            pControlPointCharacteristic->writeValue((uint8_t*)vDate,6,true);

            // TODO - readd call logic?
                            
            pendingNotification = false;
        }
        delay(100); //does not work without small delay
    }        
} // ANCSClient::run

}