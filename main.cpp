#include <Pos300Disp.h>
#include <driver/adc.h>

DISP_STATUS mStatus = DISP_STATUS_NOT_CONNECT;

static void notifyCallback(char* pData) {
    Serial.print("****data: ");
    Serial.println((char*)pData);
    dispFrame.disp_measured(pData);
}

void setup() {
  M5.begin();

  adc_power_acquire(); // ADC Power ON

  dispFrame.disp_connecting();

  blepos300.setup();
  blepos300.setNotifyCallback(notifyCallback);

}

void loop() {
    BLE_STATUS ble_st;

  if(mStatus == DISP_STATUS_NOT_CONNECT) {
    blepos300.doConnect();
    mStatus = DISP_STATUS_CONNECTING;
    delay(1000);
  }
  else if(mStatus == DISP_STATUS_CONNECTING) {
    ble_st = blepos300.getStatus();
    if(ble_st == BLE_STATUS_CONNECTING) {
      blepos300.doConnect();
      delay(1000);
    }
    else if(ble_st == BLE_STATUS_CONNECTED) {
      mStatus = DISP_STATUS_MEASURED;
      dispFrame.display_frame();
    }
  }
  else if(mStatus == DISP_STATUS_MEASURED) {
    ble_st = blepos300.getStatus();
    if(ble_st == BLE_STATUS_NOT_CONNECT) {
      mStatus = DISP_STATUS_NOT_CONNECT;
      dispFrame.disp_connecting();
    }
    else {
      if (M5.BtnA.wasReleased()) {
        Serial.printf("A released\n");
        dispFrame.select_measure_next();
      }
      else if (M5.BtnB.wasReleased()) {
        Serial.printf("B released\n");
        mStatus = DISP_STATUS_EDIT;
      }
    }
  }

  m5.update();
}