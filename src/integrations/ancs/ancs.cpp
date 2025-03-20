// Based on https://github.com/gabrielchristino/ANCS

#include "ancs.h"

#include <Arduino.h>
#include <esp_log.h>
#include <esp_bt_main.h>
#include <string>
#include <sys/time.h>
#include <time.h>
#include "sdkconfig.h"

#include "ancsClient.h"


static char LOG_TAG[] = "ANCSServer";

namespace ancs {

class ANCSServerCallbacks: public BLEServerCallbacks {
public:
    ANCSServerCallbacks(ANCSServer* ancsServer) : ancsServer{ancsServer} {};

    void onConnect(BLEServer* server, esp_ble_gatts_cb_param_t *param) {
        ESP_LOGD(LOG_TAG, "Device connected. Mac address:");
        ESP_LOGD(LOG_TAG, BLEAddress(param->connect.remote_bda).toString().c_str());

        ANCSClient* pMyClient = new ANCSClient(ancsServer);
        pMyClient->setStackSize(18000);
        pMyClient->start(new BLEAddress(param->connect.remote_bda));
    };

    void onDisconnect(BLEServer* server) {
        ESP_LOGD(LOG_TAG, "Device disconnected");
    }

private:
    ANCSServer* ancsServer;
};

void ANCSServer::run() {
        ESP_LOGD(LOG_TAG, "Starting BLE work!");
        esp_log_buffer_char(LOG_TAG, LOG_TAG, sizeof(LOG_TAG));
        esp_log_buffer_hex(LOG_TAG, LOG_TAG, sizeof(LOG_TAG));

        // Initialize device
        BLEDevice::init("RoBuddie");
        BLEServer* server = BLEDevice::createServer();
        server->setCallbacks(new ANCSServerCallbacks(this));

        // Advertising parameters:
        // Soliciting ANCS
        BLEAdvertising *advertising = server->getAdvertising();
        BLEAdvertisementData advertisementData = BLEAdvertisementData();
        advertisementData.setFlags(0x01);
        _setServiceSolicitation(&advertisementData, ancsServiceUUID);
        advertising->setAdvertisementData(advertisementData);        

        // Set security
        BLESecurity *security = new BLESecurity();
        security->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
        security->setCapability(ESP_IO_CAP_OUT);
        security->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

        //Start advertising
        advertising->start();
        
        ESP_LOGD(LOG_TAG, "Advertising started!");
    }

    
/**
 * @brief Set the service solicitation (UUID)
 * @param [in] uuid The UUID to set with the service solicitation data.  Size of UUID will be used.
 */
void ANCSServer::_setServiceSolicitation(BLEAdvertisementData *a, BLEUUID uuid)
{
    char cdata[2];
    switch(uuid.bitSize()) {
        case 16: {
            // [Len] [0x14] [UUID16] data
            cdata[0] = 3;
            cdata[1] = ESP_BLE_AD_TYPE_SOL_SRV_UUID;  // 0x14
            a->addData(std::string(cdata, 2) + std::string((char *)&uuid.getNative()->uuid.uuid16,2));
            break;
        }
    
        case 128: {
            // [Len] [0x15] [UUID128] data
            cdata[0] = 17;
            cdata[1] = ESP_BLE_AD_TYPE_128SOL_SRV_UUID;  // 0x15
            a->addData(std::string(cdata, 2) + std::string((char *)uuid.getNative()->uuid.uuid128,16));
            break;
        }
        
        default:
            return;
    }
} // setServiceSolicitationData

}