#ifndef DISPFRAME_H
#define DISPFRAME_H

#include <M5Stack.h>
#include "ButtonDrawer.h"

typedef enum {
  MEASUARED_VALUE_A = 0,
  MEASUARED_VALUE_B,
  MEASUARED_VALUE_C,
} SELECT_IDX;

typedef enum {
  MEASUARED_SELECT_AUTO = 0,
  MEASUARED_SELECT_MANUAL,
} SELECT_MODE;

#define MEASURE_TITLE_POS_X 20
#define MEASURE_A_POS_Y 90
#define MEASURE_B_POS_Y 130
#define MEASURE_C_POS_Y 170

#define MEASURE_TITLE_WIDTH 70
#define MEASURE_TITLE_NML_WIDTH 56
#define MEASURE_TITLE_HEIGHT 30
#define MEASURE_TITLE_NML_HEIGHT 24
#define MEASURE_TITLE_POS_X_NML_OFFSET (MEASURE_TITLE_WIDTH - MEASURE_TITLE_NML_WIDTH) / 2
#define MEASURE_TITLE_POS_Y_NML_OFFSET (MEASURE_TITLE_HEIGHT - MEASURE_TITLE_NML_HEIGHT) / 2

#define MEASURE_VALUE_POS_X 105
#define MEASURE_VALUE_WIDTH 96
#define MEASURE_VALUE_HEIGHT 30

#define MEASURE_RANGE_POS_X 212
#define MEASURE_RANGE_VAL_WIDTH 30
#define MEASURE_RANGE_DIV_WIDTH 18
#define MEASURE_RANGE_UNIT_WIDTH 18
#define MEASURE_RANGE_WIDTH MEASURE_RANGE_VAL_WIDTH * 2 + MEASURE_RANGE_DIV_WIDTH + MEASURE_RANGE_UNIT_WIDTH
#define MEASURE_RANGE_HEIGHT MEASURE_VALUE_HEIGHT

class DispFrame {
public:
  void init();
  void disp_connecting();
  void display_frame(bool withMeasured = false);
  void select_measure_next();
  void disp_measured(char* data, char* play_data);
  int disp_range(String min, String max);
  void saveData();
  void change_select_mode();
  SELECT_MODE get_select_mode();
  void disp_button();

private:
  uint16_t m_bg_color = BLACK;
  uint16_t m_fg_color = WHITE;

  SELECT_IDX m_focus_idx = MEASUARED_VALUE_A;
  SELECT_MODE m_select_mode = MEASUARED_SELECT_AUTO;

  ButtonDrawer _btnDrawer;

  void display_measure_title(uint8_t idx);
  void display_fixed();
  void re_display_measured(SELECT_IDX idx);
  void display_range(SELECT_IDX idx);
  void display_err();
};

extern DispFrame dispFrame;

#endif // DISPFRAME
