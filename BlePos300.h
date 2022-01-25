#ifndef BLE_POS300_H
#define BLE_POS300_H

#include <M5Stack.h>
#include "BLEDevice.h"

typedef void (*ble_notify_callback)(char* pData);

typedef enum {
  BLE_STATUS_NOT_CONNECT = 0,
  BLE_STATUS_CONNECTING,
  BLE_STATUS_CONNECTED,
} BLE_STATUS;


class BlePos300 {
public:
  BlePos300();

  void setup();
  void setNotifyCallback(ble_notify_callback _callback);
  void doConnect();
  void disConnect();
  BLE_STATUS getStatus();

private:
  bool connectToServer();
};

extern BlePos300 blepos300;

#endif /* BLE_POS300_H */