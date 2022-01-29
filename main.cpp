#include <M5Stack.h>
#include <driver/adc.h>
#include "BlePos300.h"
#include "DispFrame.h"
#include "Keyboard.h"

typedef enum {
  DISP_STATUS_NOT_CONNECT = 0,
  DISP_STATUS_CONNECTING,
  DISP_STATUS_MEASURED,
  DISP_STATUS_EDIT,
} DISP_STATUS;

DISP_STATUS mStatus = DISP_STATUS_NOT_CONNECT;

Keyboard kbd;

static void notifyCallback(char* pData) {
    if(mStatus == DISP_STATUS_MEASURED) {
      dispFrame.disp_measured(pData);
    }
}

void setup() {
  M5.begin();

  adc_power_acquire(); // ADC Power ON

  dispFrame.init();
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
        dispFrame.select_measure_next();
      }
      else if (M5.BtnB.wasReleased()) {
        mStatus = DISP_STATUS_EDIT;
        kbd.setup();
        while (kbd.loop()) {
          delay(1);
        }
        String min_str = kbd.getString_1();
        String max_str = kbd.getString_2();
        kbd.close();
        if(dispFrame.disp_range(min_str, max_str) < 0) {
          // エラー表示？今は何もしない
        }
        mStatus = DISP_STATUS_MEASURED;
        dispFrame.display_frame(true);
      }
      else if (M5.BtnC.wasReleased()) {
        dispFrame.saveData();
      }
    }
  }

  m5.update();
}