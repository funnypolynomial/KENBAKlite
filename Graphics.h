#pragma once
#include "XC4630d.h"
#include "HSV.h"
#include "LCD.h"
#include "Font5x7.h"

class Graphics
{
  public:
    enum tBtnState {eUp, eDown, eLocked, eFlash};
    void Init();
    void Paint();
    word HitTest();
    void DrawDataLEDs(byte leds);
    void DrawControlLEDs(byte leds, byte runBrightness);
    void DrawBtn(int Btn, tBtnState state);
    void ShowMenu(bool normal = true);

    void TouchTest();
  private:
    void AddCircleData(int x, int y, int r);
    bool BuildCircleData(int r);
    void DrawCircle(int x, int y, int r, word fg, word bg, word line, bool diagonal = false);
    int  Lozenge(int x, int y, int w, int h, int inset, int dX, int dY, int thick, word out, word line, word in, int indent = 0);
    void DrawDataBtn(int btn, tBtnState state);
    void DrawCtrlBtn(int btn, tBtnState state);
    void DrawBtn(int x, int y, bool white, tBtnState state, const char* label = NULL);
    void DrawDataLED(int led, bool on);
    void DrawCtrlLED(int led, bool on, byte runBrightness);
    void DrawLED(int x, int y, word colour, bool on);
    byte TotalColumnGap(int btn, bool data);
    int DrawMenu(int x, int y, const char* items, int highlight, word fg, word bg, int& lineHeight, int& maxWidth);
    int DoMenu(const char* items);
    bool MenuClicked(int tx, int ty);
    void RenderText(const char* pText, const char* pTokens, bool highlightTokens = true, char* pToken0 = NULL);
    char* LoadStrN(const char* list, int N, char* buff);
    void WaitForUp();
    void WaitForDown(int& tx, int& ty);
    void CfgClock();

    int dataBtnOffsetX;
    int dataBtnOffsetY;
    int ctrlBtnOffsetX;
    int ctrlBtnOffsetY;
    word btnWhiteLoLight;
    word btnBlackHiLight;
    byte lastDataLEDs;
    byte lastCtrlLEDs;
    byte lastRunBrightness;
    static const byte btnGapData = 6;
    static const byte btnGapCtrl = btnGapData+5;
    static const byte btnSize = 20;
    static const byte ledRadius = 7;
    static const byte ledSize = 2*ledRadius + 1;
    static const byte btnLEDOffsetY = 43;
    static const byte btnLine1OffsetY = 24;
    static const byte btnLine2OffsetY = 12;
    // cached circle raster data
    byte circleRasterRadius = 0;
    byte circleRasterOffset[ledRadius + 1];
    byte circleRasterLength[ledRadius + 1];
};


extern Graphics graphics;
