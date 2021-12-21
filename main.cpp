#include <M5Stack.h>
#include "BLEDevice.h"
//#include "BLEScan.h"

// BLE server service UUID.
static BLEUUID serviceUUID("0000fff0-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service. 
static BLEUUID NotifyCharUUID("0000fff1-0000-1000-8000-00805f9b34fb");
static BLEUUID WriteCharUUID("0000fff2-0000-1000-8000-00805f9b34fb");

const static char* POS3_Name = "POS300-";

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteNotifyCharacteristic;
static BLERemoteCharacteristic* pRemoteWriteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pClient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    doScan = true;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    Serial.println(" - Connecting to server");
    pClient->connect(myDevice);
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteNotifyCharacteristic = pRemoteService->getCharacteristic(NotifyCharUUID);
    if (pRemoteNotifyCharacteristic == nullptr) {
      Serial.print("Failed to find characteristic UUID: ");
      Serial.println(NotifyCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    pRemoteWriteCharacteristic = pRemoteService->getCharacteristic(WriteCharUUID);
    if (pRemoteWriteCharacteristic == nullptr) {
      Serial.print("Failed to find characteristic UUID: ");
      Serial.println(WriteCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }

    // Read the value of the characteristic.
    if(pRemoteNotifyCharacteristic->canRead()) {
      std::string value = pRemoteNotifyCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteNotifyCharacteristic->canNotify())
      pRemoteNotifyCharacteristic->registerForNotify(notifyCallback);

    Serial.println(" - Success");
    connected = true;

    return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.haveName() &&
        advertisedDevice.getName().find(POS3_Name) != std::string::npos) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = false;
    }
  }
};


void setup() {
  M5.begin();

  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {

  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("Connected to the BLE Server.");
    } else {
      Serial.println("Failed to connect to the BLE Server!!");
    }
    doConnect = false;
  }

  if (connected) {
    // ここで必要なデータを送る
  }else if(doScan){
    BLEDevice::getScan()->start(0);
  }
  
  delay(1000);

}