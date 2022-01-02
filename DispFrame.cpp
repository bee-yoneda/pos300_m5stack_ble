#include "DispFrame.h"
#include <jpeg.h>

// String ff_20 = "font/pos300font-20"; // without Ext
String ff_20 = "font/MSGothic16"; // without Ext

void
DispFrame::display_frame_fixed(){
  M5.Lcd.startWrite();
  M5.Lcd.setWindow(0, 0, 320, 240);

  // タイトルの表示設定
  m_Frame[TITLE].setPos(10,10,200,30);
  m_Frame[TITLE].setText("POS-300 データ読取");

  // 実測値、範囲値タイトルの表示設定
  m_Frame[MEASUARED_VALUE_TITLE].setPos(140,50,80,30);
  m_Frame[MEASUARED_VALUE_TITLE].setText((const char*)"実測値");
  m_Frame[MEASUARED_RANGE_TITLE].setPos(230,50,80,30);
  m_Frame[MEASUARED_RANGE_TITLE].setText((const char*)"範囲値");

  // 測定タイトルの表示設定
  m_Frame[MEASUARE_A].setPos(50,90,70,30);
  m_Frame[MEASUARE_A].setText((const char*)"測定A");
  m_Frame[MEASUARE_B].setPos(50,130,70,30);
  m_Frame[MEASUARE_B].setText((const char*)"測定B");
  m_Frame[MEASUARE_C].setPos(50,170,70,30);
  m_Frame[MEASUARE_C].setText((const char*)"測定C");

  // // 表示(日本語有り)
  // for(int i = 0; i<MEASUARED_VALUE_A; i++) {
  //   int x = m_Frame[i].getXpos();
  //   int y = m_Frame[i].getYpos();
  //   int w = m_Frame[i].getWidth();
  //   int h = m_Frame[i].getHeight();
  //   uint16_t frcol = m_Frame[i].getFrameColor();
  //   uint16_t bgcol = m_Frame[i].getBgColor();
  //   uint16_t txtcol = m_Frame[i].getTextColor();
  //   String str = m_Frame[i].getText();
  //   M5.Lcd.fillRect(x, y, w, h, frcol);
  //   M5.Lcd.fillRect(x+1, y+1, w-2, h-2, bgcol);
  //   M5.Lcd.setTextColor(txtcol);
  //   M5.Lcd.drawCentreString(str, x + w/2, y + (h-M5.Lcd.fontHeight(1))/2 , 1);
  // }
  M5.Lcd.drawJpg(title_jpg, sizeof(title_jpg), 20, 5, 280, 40);
  M5.Lcd.drawJpg(measure_title_A, sizeof(measure_title_A), 20, 90, 70, 30);
  M5.Lcd.drawJpg(measure_title_B, sizeof(measure_title_B), 20, 130, 70, 30);
  M5.Lcd.drawJpg(measure_title_C, sizeof(measure_title_C), 20, 170, 70, 30);
  M5.Lcd.drawJpg(measured_value_title, sizeof(measured_value_title), 110, 50, 80, 30);
  M5.Lcd.drawJpg(measured_range_title, sizeof(measured_range_title), 210, 50, 80, 30);
  M5.Lcd.endWrite();

}

void
DispFrame::display_frame_measured(){
  M5.Lcd.startWrite();
  M5.Lcd.setWindow(0, 0, 320, 240);

  // 測定結果の表示設定
  m_Frame[MEASUARED_VALUE_A].setPos(140,90,80,30);
  m_Frame[MEASUARED_VALUE_A].setFrameColor(LIGHTGREY);
  m_Frame[MEASUARED_VALUE_A].setBgColor(BLACK);
  m_Frame[MEASUARED_VALUE_A].setTextColor(WHITE);
  m_Frame[MEASUARED_VALUE_A].setText("1000mm");
  m_Frame[MEASUARED_VALUE_B].setPos(140,130,80,30);
  m_Frame[MEASUARED_VALUE_B].setFrameColor(LIGHTGREY);
  m_Frame[MEASUARED_VALUE_B].setBgColor(BLACK);
  m_Frame[MEASUARED_VALUE_B].setTextColor(WHITE);
  m_Frame[MEASUARED_VALUE_B].setText("0mm");
  m_Frame[MEASUARED_VALUE_C].setPos(140,170,80,30);
  m_Frame[MEASUARED_VALUE_C].setFrameColor(LIGHTGREY);
  m_Frame[MEASUARED_VALUE_C].setBgColor(BLACK);
  m_Frame[MEASUARED_VALUE_C].setTextColor(WHITE);
  m_Frame[MEASUARED_VALUE_C].setText("500mm");

  m_Frame[MEASUARED_RANGE_A].setPos(230,90,80,30);
  m_Frame[MEASUARED_RANGE_A].setFrameColor(LIGHTGREY);
  m_Frame[MEASUARED_RANGE_A].setBgColor(BLACK);
  m_Frame[MEASUARED_RANGE_A].setTextColor(WHITE);
  m_Frame[MEASUARED_RANGE_A].setText("1000~9999mm");
  m_Frame[MEASUARED_RANGE_B].setPos(230,130,80,30);
  m_Frame[MEASUARED_RANGE_B].setFrameColor(LIGHTGREY);
  m_Frame[MEASUARED_RANGE_B].setBgColor(BLACK);
  m_Frame[MEASUARED_RANGE_B].setTextColor(WHITE);
  m_Frame[MEASUARED_RANGE_B].setText("0~10mm");
  m_Frame[MEASUARED_RANGE_C].setPos(230,170,80,30);
  m_Frame[MEASUARED_RANGE_C].setFrameColor(LIGHTGREY);
  m_Frame[MEASUARED_RANGE_C].setBgColor(BLACK);
  m_Frame[MEASUARED_RANGE_C].setTextColor(WHITE);
  m_Frame[MEASUARED_RANGE_C].setText("50~100mm");

  for(int i = MEASUARED_VALUE_A; i<FRAME_MAX; i++) {
    int x = m_Frame[i].getXpos();
    int y = m_Frame[i].getYpos();
    int w = m_Frame[i].getWidth();
    int h = m_Frame[i].getHeight();
    uint16_t frcol = m_Frame[i].getFrameColor();
    uint16_t bgcol = m_Frame[i].getBgColor();
    uint16_t txtcol = m_Frame[i].getTextColor();
    String str = m_Frame[i].getText();
    M5.Lcd.fillRect(x, y, w, h, frcol);
    M5.Lcd.fillRect(x+1, y+1, w-2, h-2, bgcol);
    M5.Lcd.setTextColor(txtcol);
    M5.Lcd.drawCentreString(str, x + w/2, y + (h-M5.Lcd.fontHeight(1))/2 , 1);
  }
  M5.Lcd.endWrite();
}


void
DispFrame::display_frame(){
  M5.Lcd.clearDisplay(m_bg_color);
  M5.Lcd.loadFont(ff_20, SD);
  display_frame_fixed();
  display_frame_measured();
  M5.Lcd.unloadFont();
}
