#include "Keyboard.h"

uint16_t Keyboard::fontColor[2]   = {0xFFFF, 0xFFFF};
uint16_t Keyboard::backColor[2]   = {0x630C, 0x421F};
uint16_t Keyboard::frameColor[2]  = {0x0208, 0xFFFF};
uint16_t Keyboard::textboxFontColor = 0x0000;
uint16_t Keyboard::textboxBackColor = 0xFFFF;
uint8_t Keyboard::keyHeight = 14;

uint8_t Keyboard::maxlength = 5;

#define COLUMNCOUNT 11
#define KEYWIDTH 29

static const char BS   = 0x08;
static const char DEL  = 0x7f;
static const char LEFT = 0x11;
static const char RIGH = 0x13;

static const PROGMEM char _chartbl[COLUMNCOUNT] =
{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', BS };

void Keyboard::setString(const String& value1, const String& value2) {
  _string_1 = value1;
  _string_2 = value2;
  _cursorPos = _string_1.length();
}

void Keyboard::setStringByState(const String& value) {
  if(_state == EDIT_1) {
    _string_1 = value;
    _cursorPos = _string_1.length();
  }
  else if(_state == EDIT_2) {
    _string_2 = value;
    _cursorPos = _string_2.length();
  }
}

String Keyboard::getStringByState() {
  if(_state == EDIT_1) {
    return _string_1;
  }
  else if(_state == EDIT_2) {
    return _string_2;
  }
  return "";
}

void Keyboard::clearString() { 
  _string_1 = _string_2 = "";
  _cursorPos = 0;
}

void Keyboard::setup(const String& value1, const String& value2) {
  _btnHeight = ButtonDrawer::height;
  _btnDrawer.setText("","","");

  setString(value1, value2);
  _col = 0;
  _state = INIT;
}

bool Keyboard::loop() {
  uint32_t _msec = millis();

  _keyCode = 0;
  if (_state == INIT) {
    start();
    return true;
  } 

  M5.update();

  int8_t oldCol = _col;

  // move right
  if (M5.BtnA.wasReleased()) {
    _col++;
  }
  // select
  else if (M5.BtnB.wasReleased()) {
    pressKey();
  }
  else if (M5.BtnC.wasReleased()) {
    if(_state == EDIT_2) {
      return false;
    }
    else {
      _state = EDIT_2;
      _cursorPos = _string_2.length();
      _col = 0;
      draw();
      return true;
    }
  }
  if (oldCol != _col) {
    _col = (_col + COLUMNCOUNT) % COLUMNCOUNT;
    drawColumn(_col);
    drawColumn(oldCol);
  }
  // draw blink cursor.
  M5.Lcd.drawFastVLine( _cursorX
                      , getY(-1) + (keyHeight - M5.Lcd.fontHeight()) / 2
                      , M5.Lcd.fontHeight()
                      , (_msec / 150) % 2 ? textboxBackColor : textboxFontColor);

  updateButton();
  _btnDrawer.draw();
  return true;
}
void Keyboard::close() { 
  int y = getY(-1);
  M5.Lcd.fillRect(0, y, M5.Lcd.width(), M5.Lcd.height() - y, m_bg_color);
  clearString();
}

int Keyboard::getX(int col) {
  return col * KEYWIDTH;
}
int Keyboard::getY(int row) {
  return M5.Lcd.height() - _btnHeight - (1 - row) * keyHeight;
}

void Keyboard::updateButton() {
  switch(_state) {
    default:
    case INIT:
      break;
    case EDIT_1:
      _btnDrawer.setText("MoveCusor", "Select", "Next");
      break;
    case EDIT_2:
      _btnDrawer.setText("MoveCusor", "Select", "Finish");
      break;
  }
}

void Keyboard::pressKey() {
  pressKey(_chartbl[_col]);
}
void Keyboard::pressKey(char keycode) {
  _keyCode = keycode;
  String _tmp_str = getStringByState();

  switch (_keyCode) {
    case BS:
      if (0 < _cursorPos) {
        _tmp_str = _tmp_str.substring(0, _cursorPos-1) + _tmp_str.substring(_cursorPos);
      }
      break;
    default:
      if (_tmp_str.length() < maxlength) {
        _tmp_str = _tmp_str.substring(0, _cursorPos) + _keyCode + _tmp_str.substring(_cursorPos);
      }
      break;
  }
  setStringByState(_tmp_str);
  drawTextbox();
}

void Keyboard::draw() {
  drawKeyboard();
  drawTextbox();
  _btnDrawer.draw(true);
}

void Keyboard::drawTextbox() {
  int y = getY(-1);
  int ty = y + (keyHeight - M5.Lcd.fontHeight()) / 2;
  int oldX = M5.Lcd.getCursorX();
  int oldY = M5.Lcd.getCursorY();
  M5.Lcd.setTextColor(textboxFontColor);
  M5.Lcd.drawFastHLine(0, y, M5.Lcd.width(), frameColor[0]);
  M5.Lcd.fillRect(0, y + 1, M5.Lcd.width(), keyHeight - 1, textboxBackColor);
  M5.Lcd.setCursor(1, ty);
  _cursorX = 0;
  String _tmp_str = getStringByState();

  for (int i = 0; i < _tmp_str.length(); ++i) {
    M5.Lcd.print(_tmp_str[i]);
    if (_cursorPos == i + 1) _cursorX = M5.Lcd.getCursorX() - 1;
  }
  M5.Lcd.setCursor(oldX, oldY);
}

void Keyboard::drawKeyboard(int h) {
  if (h < 0) h = keyHeight;
  int y = M5.Lcd.height() - _btnHeight - h;
  for (int c = 0; c < COLUMNCOUNT; ++c) {
    int x = getX(c);
    drawColumn(c, x, y, h);
    M5.Lcd.drawFastVLine(x, y, h, frameColor[0]);
  }
  M5.Lcd.drawFastVLine(getX(COLUMNCOUNT), y, h, frameColor[0]);
}

void Keyboard::drawColumn(int col) {
  drawColumn(col, getX(col), getY(0), keyHeight);
}

void Keyboard::drawColumn(int col, int x, int y, int h) {
  M5.Lcd.fillRect(x+1, y, KEYWIDTH-1, h, backColor[0]);
  int kh = h;
  char tbl[2] = {_chartbl[col],0};
  int fh = M5.Lcd.fontHeight();
  uint16_t color = fontColor[_col == col ? 1 : 0];
  char* str = tbl;
  char code = _chartbl[col];

  if (_col == col) {
    M5.Lcd.fillRect(x+1, y+1, KEYWIDTH - 1, kh - 1, backColor[1]);
  }

  switch (code) {
    case BS:
      str = (char*)PROGMEM "BS";
      break;
  }
  int fy = min(y + (kh - fh + 1) / 2, M5.Lcd.height() - _btnHeight - fh);
  M5.Lcd.setTextColor(color);
  M5.Lcd.drawCentreString(str, x + 16, fy, 1);

  M5.Lcd.drawFastHLine(x, y, KEYWIDTH, frameColor[0]);
}

bool Keyboard::start() {
  _state = EDIT_1;
  draw();
  return true;
}
