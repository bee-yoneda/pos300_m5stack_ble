#ifndef DISPFRAME_H
#define DISPFRAME_H

#include <M5Stack.h>

struct Frame {
private:
  int m_x_pos = 0;
  int m_y_pos = 0;
  int m_width = 100;
  int m_height = 100;

  uint16_t m_frame_color = 0x8F71; // LightGreen
  uint16_t m_bg_color = 0x8F71; // LightGreen
  uint16_t m_text_color = BLACK;
  String m_text;

public:
  void setPos(int x, int y, int width, int height) {
    m_x_pos = x;
    m_y_pos = y;
    m_width = width;
    m_height = height;
  }
  int getXpos() {return m_x_pos;}
  int getYpos() {return m_y_pos;}
  int getWidth() {return m_width;}
  int getHeight() {return m_height;}

  void setText(char* text) {
    m_text = text;
  }
  String getText() {
    return m_text;
  }
  void setFrameColor(uint16_t color) {
    m_frame_color = color;
  }
  uint16_t getFrameColor() {
    return m_frame_color;
  }
  void setBgColor(uint16_t color) {
    m_bg_color = color;
  }
  uint16_t getBgColor() {
    return m_bg_color;
  }
  void setTextColor(uint16_t color) {
    m_text_color = color;
  }
  uint16_t getTextColor() {
    return m_text_color;
  }
};

typedef enum {
  TITLE = 0,
  MEASUARE_A,
  MEASUARE_B,
  MEASUARE_C,
  MEASUARED_VALUE_TITLE,
  MEASUARED_RANGE_TITLE,
  MEASUARED_VALUE_A,
  MEASUARED_VALUE_B,
  MEASUARED_VALUE_C,
  MEASUARED_RANGE_A,
  MEASUARED_RANGE_B,
  MEASUARED_RANGE_C,
  FRAME_MAX
} FRAME_IDX;

class DispFrame {
private:
  Frame m_Frame[FRAME_MAX];

  uint16_t m_bg_color = BLACK;

  void display_frame_fixed();
  void display_frame_measured();

public:
  void display_frame();

};

#endif // DISPFRAME
