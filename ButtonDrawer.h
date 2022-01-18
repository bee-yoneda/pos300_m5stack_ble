#ifndef _BUTTONDRAWER_H_
#define _BUTTONDRAWER_H_

#include <M5Stack.h>

class ButtonDrawer {
public:
  static uint16_t frameColor[2];
  static uint16_t backColor[2];
  static uint16_t fontColor[2];
  static int16_t width;
  static int16_t height;
  ButtonDrawer(){};
  ButtonDrawer(const String& btnA, const String& btnB, const String& btnC)
   : _strings{btnA,btnB,btnC}
  {
  };
  void setText(const String& btnA, const String& btnB, const String& btnC);
  void setText(uint8_t idx, const String& str);
  void draw(bool force = false);
  void draw(uint8_t idx, bool pressed);

private:
  String _strings[3];
  bool _mod[3];
  void drawButton(int x, bool pressed, const String& title) const;
};

#endif

