#include "DispFrame.h"
#include <jpeg.h>

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

DispFrame::DispFrame(){

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
    M5.Lcd.drawJpg(connecting, sizeof(connecting), 85, 105, 150, 30);
}

void DispFrame::display_frame() {
  M5.Lcd.clearDisplay(m_bg_color);
  display_frame_fixed();
}

void DispFrame::select_measure_next() {
  if(m_focus_idx >= MEASUARED_VALUE_C) {
    m_focus_idx = MEASUARED_VALUE_A;
  }
  else {
    m_focus_idx++;
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

  switch(m_focus_idx) {
    case MEASUARED_VALUE_A:
      y = MEASURE_A_POS_Y;
      break;
    case MEASUARED_VALUE_B:
      y = MEASURE_B_POS_Y;
      break;
    case MEASUARED_VALUE_C:
      y = MEASURE_C_POS_Y;
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
  int space_len = pos2 - pos1;
  space_len = 5 - space_len;
  String measured_str = "";
  for(int i=0; i<space_len; i++) {
    measured_str += " ";
  }
  measured_str += str.substr(pos1).c_str();

  M5.Lcd.setTextSize(2);

  M5.Lcd.fillRect(MEASURE_VALUE_POS_X, y, MEASURE_VALUE_WIDTH, MEASURE_VALUE_HEIGHT, m_bg_color);
  M5.Lcd.drawString(measured_str, MEASURE_VALUE_POS_X, y + (MEASURE_VALUE_HEIGHT-M5.Lcd.fontHeight())/2);

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
  M5.Lcd.setTextSize(1);
  switch(idx) {
    case MEASUARED_VALUE_A:
      y = MEASURE_A_POS_Y;
      min_str = "1000";
      max_str = "10000";
      break;
    case MEASUARED_VALUE_B:
      y = MEASURE_B_POS_Y;
      min_str = "0";
      max_str = "10";
      break;
    case MEASUARED_VALUE_C:
      y = MEASURE_C_POS_Y;
      min_str = "300";
      max_str = "99999";
      break;
    default:
      return;
  }

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

DispFrame dispFrame;