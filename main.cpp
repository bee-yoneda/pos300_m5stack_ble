#include <Pos300Disp.h>

//String ff_20 = "font/pos300font-20"; // without Ext

DispFrame *mDisp = NULL;
DISP_STATUS mStatus = DISP_STATUS_NOT_CONNECT;

static void notifyCallback(char* pData) {
    Serial.print("****data: ");
    Serial.println((char*)pData);
}

void setup() {
  M5.begin();

  blepos300.setup();
  blepos300.setNotifyCallback(notifyCallback);

  mDisp = new DispFrame();

}

void loop() {
    BLE_STATUS ble_st;

  if(mStatus == DISP_STATUS_NOT_CONNECT) {
    blepos300.doConnect();
    mStatus = DISP_STATUS_CONNECTING;
  }
  else if(mStatus == DISP_STATUS_CONNECTING) {
    ble_st = blepos300.getStatus();
    if(ble_st == BLE_STATUS_CONNECTING) {
      blepos300.doConnect();
    }
    else if(ble_st == BLE_STATUS_CONNECTED) {
      mStatus = DISP_STATUS_MEASURED;
      mDisp->display_frame();
    }
  }
  else if(mStatus == DISP_STATUS_MEASURED) {
    ble_st = blepos300.getStatus();
    if(ble_st == BLE_STATUS_NOT_CONNECT) {
      mStatus = DISP_STATUS_NOT_CONNECT;
    }
    else {
      // mDisp->display_frame();
    }
  }

  delay(1000);

}