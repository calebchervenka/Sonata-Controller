#include <NimBLEDevice.h>

#include "lighting.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic, NimBLEConnInfo& connInfo) {
      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.println(value.c_str());
        update_config_from_str(value.append("\r").c_str());
        set_all_flags();
      }
    }
};

class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, NimBLEConnInfo& connInfo) {
      Serial.println("iPhone Connected!");
    };

    void onDisconnect(BLEServer* pServer, NimBLEConnInfo& connInfo) {
      Serial.println("iPhone Disconnected. Restarting Advertising...");
      delay(500);
      pServer->startAdvertising(); 
    }
};

void wireless_init() {
  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

  BLEDevice::init("ESP32-Gems-Device");
  
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         NIMBLE_PROPERTY::READ |
                                         NIMBLE_PROPERTY::WRITE
                                       );

  pCharacteristic->setCallbacks(new CharacteristicCallbacks());
  pServer->setCallbacks(new ServerCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();
  
  BLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);

  NimBLEAdvertisementData scanResponseData;
  
  scanResponseData.setName("Sonata-Dash-Pro");
  scanResponseData.setCompleteServices(NimBLEUUID(SERVICE_UUID));
  pAdvertising->setScanResponseData(scanResponseData);

  pAdvertising->start();
}