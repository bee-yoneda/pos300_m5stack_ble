#include "DispFrame.h"
#include <jpeg.h>
#include <Data.h>

Data m_Data;

void
DispFrame::display_measure(uint8_t idx) {
  uint16_t x, y, w, h;
  const uint8_t* jpg = NULL;

  x = MEASURE_TITLE_POS_X;

  switch(idx) {
    case MEASUARED_VALUE_A:
      y = MEASURE_A_POS_Y;
      if(idx == m_focus_idx) {
        jpg = measure_title_A_focus;
      }
      else {
        jpg = measure_title_A;
      }
      break;
    case MEASUARED_VALUE_B:
      y = MEASURE_B_POS_Y;
      if(idx == m_focus_idx) {
        jpg = measure_title_B_focus;
      }
      else {
        jpg = measure_title_B;
      }
      break;
    case MEASUARED_VALUE_C:
      y = MEASURE_C_POS_Y;
      if(idx == m_focus_idx) {
        jpg = measure_title_C_focus;
      }
      else {
        jpg = measure_title_C;
      }
      break;
    default:
      return;
  }

  if(idx == m_focus_idx) {
    w = MEASURE_TITLE_WIDTH;
    h = MEASURE_TITLE_HEIGHT;
  }
  else {
    w = MEASURE_TITLE_NML_WIDTH;
    h = MEASURE_TITLE_NML_HEIGHT;
    x += MEASURE_TITLE_POS_X_NML_OFFSET;
    y += MEASURE_TITLE_POS_Y_NML_OFFSET;
  }

  M5.Lcd.drawJpg(jpg, sizeof(jpg), x, y, w, h);
}

void
DispFrame::init(){
  m_Data.setup();
}


void
DispFrame::display_frame_fixed(){
  M5.Lcd.startWrite();
  M5.Lcd.setWindow(0, 0, 320, 240);

  M5.Lcd.drawJpg(title_jpg, sizeof(title_jpg), 20, 5, 280, 40);

  display_measure(MEASUARED_VALUE_A);
  display_measure(MEASUARED_VALUE_B);
  display_measure(MEASUARED_VALUE_C);

  M5.Lcd.drawJpg(measured_value_title, sizeof(measured_value_title),
                  110, 50, 80, 30);
  M5.Lcd.drawJpg(measured_range_title, sizeof(measured_range_title),
                  220, 50, 80, 30);

  disp_range(MEASUARED_VALUE_A);
  disp_range(MEASUARED_VALUE_B);
  disp_range(MEASUARED_VALUE_C);

  M5.Lcd.endWrite();
}

void DispFrame::disp_connecting() {
  M5.Lcd.clearDisplay(m_bg_color);
#if 0
  M5.Lcd.setTextSize(4);
  M5.Lcd.drawString("CONNECTING", 40, 100);
  M5.Lcd.setTextSize(1);
#else
  M5.Lcd.drawJpg(connecting, sizeof(connecting), 85, 105, 150, 30);
#endif
}

void DispFrame::display_frame(bool withMeasured) {
  M5.Lcd.clearDisplay(m_bg_color);
  display_frame_fixed();
  _btnDrawer.setText("SelMeasure", "Edit", "Save");
  _btnDrawer.draw(true);
  if(withMeasured) {
    M5.Lcd.startWrite();
    re_disp_measured(MEASUARED_VALUE_A);
    re_disp_measured(MEASUARED_VALUE_B);
    re_disp_measured(MEASUARED_VALUE_C);
    M5.Lcd.endWrite();
  }
}

void DispFrame::re_disp_measured(SELECT_IDX idx) {
  uint16_t y;
  const char *unit = " mm";
  DATA_IDX dataIdx = DATA_IDX_A;

  switch(idx) {
    case MEASUARED_VALUE_A:
      y = MEASURE_A_POS_Y;
      dataIdx = DATA_IDX_A;
      break;
    case MEASUARED_VALUE_B:
      y = MEASURE_B_POS_Y;
      dataIdx = DATA_IDX_B;
      break;
    case MEASUARED_VALUE_C:
      y = MEASURE_C_POS_Y;
      dataIdx = DATA_IDX_C;
      break;
    default:
      return;
  }

  int measured = m_Data.getMeasuredValue(dataIdx);
  if(measured < 0) {
    Serial.println("still not set measured");
    return;
  }
  String str = String(measured);

  int data_len = str.length();

  int space_len = 5 - data_len;
  String disp_str = "";
  for(int i=0; i<space_len; i++) {
    disp_str += " ";
  }
  disp_str += str;
  disp_str += unit;

  M5.Lcd.setTextSize(2);

  M5.Lcd.fillRect(MEASURE_VALUE_POS_X, y, MEASURE_VALUE_WIDTH, MEASURE_VALUE_HEIGHT, m_bg_color);
  M5.Lcd.drawString(disp_str, MEASURE_VALUE_POS_X, y + (MEASURE_VALUE_HEIGHT-M5.Lcd.fontHeight())/2);

  M5.Lcd.setTextSize(1);
}

void DispFrame::display_err() {
  uint16_t org_color = m_bg_color;
  m_bg_color = RED;
  display_frame(true);
  M5.Speaker.beep();
  delay(1000);
  m_bg_color = org_color;
  display_frame(true);
}

void DispFrame::select_measure_next() {
  switch(m_focus_idx) {
    case MEASUARED_VALUE_A:
      m_focus_idx = MEASUARED_VALUE_B;
      break;
    case MEASUARED_VALUE_B:
      m_focus_idx = MEASUARED_VALUE_C;
      break;
    default:
    case MEASUARED_VALUE_C:
      m_focus_idx = MEASUARED_VALUE_A;
      break;
  }

  M5.Lcd.startWrite();
  M5.Lcd.setWindow(0, 0, 320, 240);
  M5.Lcd.fillRect(MEASURE_TITLE_POS_X, MEASURE_A_POS_Y, MEASURE_TITLE_WIDTH, MEASURE_C_POS_Y + MEASURE_TITLE_HEIGHT - MEASURE_A_POS_Y, m_bg_color);
  display_measure(MEASUARED_VALUE_A);
  display_measure(MEASUARED_VALUE_B);
  display_measure(MEASUARED_VALUE_C);
  M5.Lcd.endWrite();
}

void DispFrame::disp_measured(char* data) {
  uint16_t y;
  const char *prefix = "L ";
  const char *unit = " mm";
  DATA_IDX dataIdx = DATA_IDX_A;

  switch(m_focus_idx) {
    case MEASUARED_VALUE_A:
      y = MEASURE_A_POS_Y;
      dataIdx = DATA_IDX_A;
      break;
    case MEASUARED_VALUE_B:
      y = MEASURE_B_POS_Y;
      dataIdx = DATA_IDX_B;
      break;
    case MEASUARED_VALUE_C:
      y = MEASURE_C_POS_Y;
      dataIdx = DATA_IDX_C;
      break;
    default:
      return;
  }
  std::string str = data;
  int pos1 = str.find(prefix);
  if(pos1 == std::string::npos) {
    Serial.println("no data L ");
    return;
  }
  pos1 += strlen(prefix);
  int pos2 = str.rfind(unit);
  if(pos2 == std::string::npos) {
    Serial.println("no data  mm");
    return;
  }
  int data_len = pos2 - pos1;
  String measured_str = str.substr(pos1, data_len).c_str();
  if(m_Data.setMeasuredValue(dataIdx, measured_str) < 0) {
    // エラー表示に切り替える
    display_err();
    return;
  }

  int space_len = 5 - data_len;
  String disp_str = "";
  for(int i=0; i<space_len; i++) {
    disp_str += " ";
  }
  disp_str += measured_str;
  disp_str += unit;

  M5.Lcd.setTextSize(2);

  M5.Lcd.fillRect(MEASURE_VALUE_POS_X, y, MEASURE_VALUE_WIDTH, MEASURE_VALUE_HEIGHT, m_bg_color);
  M5.Lcd.drawString(disp_str, MEASURE_VALUE_POS_X, y + (MEASURE_VALUE_HEIGHT-M5.Lcd.fontHeight())/2);

  M5.Lcd.setTextSize(1);
}

void DispFrame::disp_range(SELECT_IDX idx) {
  uint16_t x = MEASURE_RANGE_POS_X;
  uint16_t y;
  uint16_t w = MEASURE_RANGE_WIDTH;
  uint16_t h = MEASURE_RANGE_HEIGHT;
  const char *divStr = " - ";
  const char *unitStr = " mm";
  String min_str, max_str;
  DATA_IDX dataIdx = DATA_IDX_A;
  int range_min = 0, range_max = 0;

  M5.Lcd.setTextSize(1);

  switch(idx) {
    case MEASUARED_VALUE_A:
      y = MEASURE_A_POS_Y;
      dataIdx = DATA_IDX_A;
      break;
    case MEASUARED_VALUE_B:
      y = MEASURE_B_POS_Y;
      dataIdx = DATA_IDX_B;
      break;
    case MEASUARED_VALUE_C:
      y = MEASURE_C_POS_Y;
      dataIdx = DATA_IDX_C;
      break;
    default:
      return;
  }
  m_Data.getRangeData(dataIdx, &range_min, &range_max);
  min_str = String(range_min);
  max_str = String(range_max);

  M5.Lcd.fillRect(x, y, w, h, m_bg_color);
  y = y + (h-M5.Lcd.fontHeight())/2;
  M5.Lcd.drawRightString(min_str, x + MEASURE_RANGE_VAL_WIDTH, y, 1);
  x += MEASURE_RANGE_VAL_WIDTH;
  M5.Lcd.drawCentreString(divStr, x + MEASURE_RANGE_DIV_WIDTH/2, y, 1);
  x += MEASURE_RANGE_DIV_WIDTH;
  M5.Lcd.drawRightString(max_str, x + MEASURE_RANGE_VAL_WIDTH, y, 1);
  x += MEASURE_RANGE_VAL_WIDTH;
  M5.Lcd.drawCentreString(unitStr, x + MEASURE_RANGE_UNIT_WIDTH/2, y, 1);
}

int DispFrame::disp_range(String min, String max) {
  DATA_IDX dataIdx;

  switch(m_focus_idx) {
    case MEASUARED_VALUE_A:
      dataIdx = DATA_IDX_A;
      break;
    case MEASUARED_VALUE_B:
      dataIdx = DATA_IDX_B;
      break;
    case MEASUARED_VALUE_C:
      dataIdx = DATA_IDX_C;
      break;
    default:
      return -1;
  }
  if(m_Data.setRangeData(dataIdx, min, max) < 0) {
    // データセット失敗
    M5.Speaker.beep();
    return -1;
  }

  disp_range(m_focus_idx);
  return 0;
}

void DispFrame::saveData() {
  m_Data.save();
}

DispFrame dispFrame;