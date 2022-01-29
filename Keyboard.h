#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <M5Stack.h>
#include "ButtonDrawer.h"

class Keyboard
{
public:
  static uint16_t fontColor[2];
  static uint16_t backColor[2];
  static uint16_t frameColor[2];
  static uint16_t textboxFontColor;
  static uint16_t textboxBackColor;
  static uint8_t keyHeight;

  static uint8_t maxlength;

  void setup(const String& value1 = "", const String& value2 = "");
  bool loop();
  void close();

  void draw();
  void clearString();
  String getString_1() {return _string_1; }
  String getString_2() {return _string_2; }
  void setString(const String& value1 = "", const String& value2 = "");
  String getStringByState();
  void setStringByState(const String& value = "");
private:

  enum eState
  {
    INIT = 0,
    EDIT_1,
    EDIT_2
  };
  eState _state;
  int8_t _col = 0;
  int8_t _cursorPos = 0;
  int16_t _cursorX = 0;
  char _keyCode;
  String _string_1;
  String _string_2;
  ButtonDrawer _btnDrawer;
  uint8_t _btnHeight;
  uint16_t m_bg_color = BLACK;

  int getX(int col);
  int getY(int row);
  void updateButton();
  void pressKey();
  void pressKey(char keycode);
  void drawTextbox();
  void drawKeyboard(int h = -1);
  void drawColumn(int col);
  void drawColumn(int col, int x, int y, int h);
  bool start();
};

#endif
