#include <M5Stack.h>
#include <BlePos300.h>

String ff_20 = "font/pos300font-20"; // without Ext

static void notifyCallback(char* pData) {
    Serial.print("****data: ");
    Serial.println((char*)pData);
}

void setup() {
  M5.begin();

  blepos300.setup();
  blepos300.setNotifyCallback(notifyCallback);

  M5.Lcd.loadFont(ff_20, SD);
  M5.Lcd.setCursor(60, 40);
  M5.Lcd.print("POS-300  データ読取 測定ABC 実測値 範囲");
}

void loop() {

  blepos300.doConnect();
  
  delay(1000);

}