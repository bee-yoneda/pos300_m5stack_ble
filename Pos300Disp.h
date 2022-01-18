#ifndef POS300_DISP_H
#define POS300_DISP_H

#include <M5Stack.h>
#include <BlePos300.h>
#include <DispFrame.h>
#include <Keyboard.h>

typedef enum {
  DISP_STATUS_NOT_CONNECT = 0,
  DISP_STATUS_CONNECTING,
  DISP_STATUS_MEASURED,
  DISP_STATUS_EDIT,
} DISP_STATUS;


#endif /* POS300_DISP_H */