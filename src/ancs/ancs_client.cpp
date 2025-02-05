#include <Arduino.h>
#include "BLEDevice.h"

#include "ancs_client.h"
#include "ancs.h"

namespace ancs {

uint8_t latestMessageID[4];
boolean pendingNotification = false;
boolean incomingCall = false;
uint8_t acceptCall = 0;

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

    pDataSourceCharacteristic->registerForNotify([](BLERemoteCharacteristic* pDataSourceCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
        for(int i = 0; i < length; i++){
            if(i > 7){
                Serial.write(pData[i]);
            }
        }
        Serial.println();
    });


    pDataSourceCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)v,2,true);

    pNotificationSourceCharacteristic->registerForNotify([](BLERemoteCharacteristic* pDataSourceCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
        if(pData[0]==0)
        {
            Serial.println("New notification!");
            latestMessageID[0] = pData[4];
            latestMessageID[1] = pData[5];
            latestMessageID[2] = pData[6];
            latestMessageID[3] = pData[7];
            
            switch(pData[2])
            {
                case 0:
                    Serial.println("Category: Other");
                break;
                case 1:
                    incomingCall = true;
                    Serial.println("Category: Incoming call");
                break;
                case 2:
                    Serial.println("Category: Missed call");
                break;
                case 3:
                    Serial.println("Category: Voicemail");
                break;
                case 4:
                    Serial.println("Category: Social");
                break;
                case 5:
                    Serial.println("Category: Schedule");
                break;
                case 6:
                    Serial.println("Category: Email");
                break;
                case 7:
                    Serial.println("Category: News");
                break;
                case 8:
                    Serial.println("Category: Health");
                break;
                case 9:
                    Serial.println("Category: Business");
                break;
                case 10:
                    Serial.println("Category: Location");
                break;
                case 11:
                    Serial.println("Category: Entertainment");
                break;
                default:
                break;
            }
        }
        else if(pData[0]==1)
        {
        Serial.println("Notification Modified!");
        if (pData[2] == 1) {
            Serial.println("Call Changed!");
        }
        }
        else if(pData[0]==2)
        {
        Serial.println("Notification Removed!");
        if (pData[2] == 1) {
            Serial.println("Call Gone!");
        }
        }
        pendingNotification = true;
    });
    
    pNotificationSourceCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)v,2,true);
    /** END ANCS SERVICE **/

    while(1){
        if(pendingNotification || incomingCall){
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


            while (incomingCall)
            {
                if (Serial.available() > 0) {
                acceptCall = Serial.read();
                Serial.println((char)acceptCall);
                }
                
                if (acceptCall == 49) { //call accepted , get number 1 from serial
                const uint8_t vResponse[]={0x02,   latestMessageID[0],latestMessageID[1],latestMessageID[2],latestMessageID[3],   0x00};
                pControlPointCharacteristic->writeValue((uint8_t*)vResponse,6,true);

                acceptCall = 0;
                //incomingCall = false;
                }
                else if (acceptCall == 48) {  //call rejected , get number 0 from serial
                const uint8_t vResponse[]={0x02,   latestMessageID[0],latestMessageID[1],latestMessageID[2],latestMessageID[3],   0x01};
                pControlPointCharacteristic->writeValue((uint8_t*)vResponse,6,true);

                acceptCall = 0;
                incomingCall = false;
                }
            }
            
                            
            pendingNotification = false;
        }
        delay(100); //does not work without small delay
    }        
} // ANCSClient::run

}