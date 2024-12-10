//--------------------------------------------------------------------------------
// Title: Main - [ main ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111110-000002,cmt:
// CN01:Ball
// CN02:Line
// CN03:L-Touch
// CN04:R-Touch
// CN05:C-Touch
// CN06:
// CN07:
// CN08:
// CN09:
// CN10:
//--------------------------------------------------------------------------------
#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
//--------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------
void user_sub_1(void);
void user_sub_2(void);
void user_sub_3(void);
void user_main(void);
//--------------------------------------------------------------------------------
// Program Name : pid-modoki.C
//--------------------------------------------------------------------------------
void user_sub_1(void)
{
  gV[VAR_A] = gAD[CN2];
  gV[VAR_A] = gV[VAR_A] - 360;
  gV[VAR_A] = gV[VAR_A] / 1;
  if (gV[VAR_A] > 50) {
    gV[VAR_A] = 50;
  } else if (gV[VAR_A] < - 50) {
    gV[VAR_A] = - 50;
  }
  gV[VAR_W] = gV[VAR_H] + gV[VAR_A];
  gV[VAR_X] = gV[VAR_I] - gV[VAR_A];
  gV[VAR_Y] = gV[VAR_J] + gV[VAR_A];
  gV[VAR_Z] = gV[VAR_K] - gV[VAR_A];
  if (gV[VAR_W] > 100) {
    gV[VAR_W] = 100;
  } else if (gV[VAR_W] < - 100) {
    gV[VAR_W] = - 100;
  }
  if (gV[VAR_X] > 100) {
    gV[VAR_X] = 100;
  } else if (gV[VAR_X] < - 100) {
    gV[VAR_X] = - 100;
  }
  if (gV[VAR_Y] > 100) {
    gV[VAR_Y] = 100;
  } else if (gV[VAR_Y] < - 100) {
    gV[VAR_Y] = - 100;
  }
  if (gV[VAR_Z] > 100) {
    gV[VAR_Z] = 100;
  } else if (gV[VAR_Z] < - 100) {
    gV[VAR_Z] = - 100;
  }
}
//--------------------------------------------------------------------------------
// Program Name : right.C
//--------------------------------------------------------------------------------
void user_sub_2(void)
{
  gV[VAR_H] = 100;
  gV[VAR_I] = 100;
  gV[VAR_J] = 0;
  gV[VAR_K] = 0;
  gPwm[0] = gV[VAR_W] < 0 ? (gV[VAR_W] * -1) | 0x80 : gV[VAR_W];
  gPwm[1] = gV[VAR_X] < 0 ? (gV[VAR_X] * -1) | 0x80 : gV[VAR_X];
  gPwm[2] = gV[VAR_Y] < 0 ? (gV[VAR_Y] * -1) | 0x80 : gV[VAR_Y];
  gPwm[3] = gV[VAR_Z] < 0 ? (gV[VAR_Z] * -1) | 0x80 : gV[VAR_Z];
  pwm_out();
}
//--------------------------------------------------------------------------------
// Program Name : stop.C
//--------------------------------------------------------------------------------
void user_sub_3(void)
{
  gV[VAR_H] = 0;
  gV[VAR_I] = 0;
  gV[VAR_J] = 0;
  gV[VAR_K] = 0;
  gPwm[0] = gV[VAR_W] < 0 ? (gV[VAR_W] * -1) | 0x80 : gV[VAR_W];
  gPwm[1] = gV[VAR_X] < 0 ? (gV[VAR_X] * -1) | 0x80 : gV[VAR_X];
  gPwm[2] = gV[VAR_Y] < 0 ? (gV[VAR_Y] * -1) | 0x80 : gV[VAR_Y];
  gPwm[3] = gV[VAR_Z] < 0 ? (gV[VAR_Z] * -1) | 0x80 : gV[VAR_Z];
  pwm_out();
}
//--------------------------------------------------------------------------------
// Program Name : main.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  while (TRUE) {
    user_sub_1();
    if (gAD[CN1] < 51) {
      user_sub_2();
    } else if (gAD[CN1] < 92) {
      user_sub_2();
    } else {
      user_sub_3();
    }
  }
}
//--------------------------------------------------------------------------------
