#include <BlePos300.h>
#include <jpeg.h>

// BLE server service UUID.
static BLEUUID serviceUUID("0000fff0-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service. 
static BLEUUID NotifyCharUUID("0000fff1-0000-1000-8000-00805f9b34fb");
static BLEUUID WriteCharUUID("0000fff2-0000-1000-8000-00805f9b34fb");

const char* POS3_Name = "POS300-";

static BLE_STATUS m_Status = BLE_STATUS_NOT_CONNECT;
static BLERemoteCharacteristic* m_pRemoteNotifyCharacteristic;
static BLERemoteCharacteristic* m_pRemoteWriteCharacteristic;
static BLEAdvertisedDevice* m_Device;
static BLEClient*  m_pClient;
static ble_notify_callback m_notifyCallback;

BlePos300::BlePos300() {
  m_Status = BLE_STATUS_NOT_CONNECT;
  m_notifyCallback = NULL;
}

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    // Serial.print("Notify callback for characteristic ");
    // Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    // Serial.print(" of data length ");
    // Serial.println(length);
    // Serial.print("data: ");
    // Serial.println((char*)pData);
    if(m_notifyCallback != NULL) {
      m_notifyCallback((char*)pData);
    }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pClient) {
  }

  void onDisconnect(BLEClient* pclient) {
    m_Status = BLE_STATUS_NOT_CONNECT;
    m_pClient = NULL;
    Serial.println("onDisconnect");
  }
};

bool BlePos300::connectToServer() {
  m_pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");

  m_pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  m_pClient->connect(m_Device);
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = m_pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    m_pClient->disconnect();
    return false;
  }

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  m_pRemoteNotifyCharacteristic = pRemoteService->getCharacteristic(NotifyCharUUID);
  if (m_pRemoteNotifyCharacteristic == nullptr) {
    Serial.print("Failed to find characteristic UUID: ");
    Serial.println(NotifyCharUUID.toString().c_str());
    m_pClient->disconnect();
    return false;
  }
  m_pRemoteWriteCharacteristic = pRemoteService->getCharacteristic(WriteCharUUID);
  if (m_pRemoteWriteCharacteristic == nullptr) {
    Serial.print("Failed to find characteristic UUID: ");
    Serial.println(WriteCharUUID.toString().c_str());
    m_pClient->disconnect();
    return false;
  }

  // Read the value of the characteristic.
  if(m_pRemoteNotifyCharacteristic->canRead()) {
    std::string value = m_pRemoteNotifyCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if(m_pRemoteNotifyCharacteristic->canNotify())
    m_pRemoteNotifyCharacteristic->registerForNotify(notifyCallback);

  Serial.println(" - Success");

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
      m_Device = new BLEAdvertisedDevice(advertisedDevice);
      m_Status = BLE_STATUS_CONNECTING;
    }
  }
};


void BlePos300::setup() {
  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void BlePos300::setNotifyCallback(ble_notify_callback _callback) {
  m_notifyCallback = _callback;
}

void BlePos300::doConnect() {
  switch(m_Status) {
    default:
    case BLE_STATUS_NOT_CONNECT:
      BLEDevice::getScan()->start(0);
      break;
    case BLE_STATUS_CONNECTING:
      if(connectToServer()) {
        Serial.println("Connected to the BLE Server.");
        m_Status = BLE_STATUS_CONNECTED;
      }
      else {
        Serial.println("Failed to connect to the BLE Server!!");
        m_Status = BLE_STATUS_NOT_CONNECT;
      }
      break;
    case BLE_STATUS_CONNECTED:
      // 何もしない
      break;
  }
}

void BlePos300::disConnect() {
  switch(m_Status) {
    default:
    case BLE_STATUS_NOT_CONNECT:
    case BLE_STATUS_CONNECTING:
      break;
    case BLE_STATUS_CONNECTED:
      m_pClient->disconnect();
      break;
  }
}

BLE_STATUS BlePos300::getStatus() {
  return m_Status;
}

BlePos300 blepos300;
