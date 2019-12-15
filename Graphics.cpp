#include <Arduino.h>
#include "Docs.h"
#include "Clock.h"
#include "Buttons.h"
#include "Config.h"

#include "Graphics.h"

// LARGE file containing all the graphics logic for the "lite" skin.  But see also LCD, XC4630d, Font5x7 & Docs
Graphics graphics;

#define CASE_BLUE BLUE
#define CASE_BACKGROUND GREY
#define BTN_SHADOW 0x2104 // vs 0x4208
#define DATA_LED_ON RED
#define DATA_LED_OFF 0x3800
#define CTRL_LED_ON YELLOW
#define CTRL_LED_OFF 0x39E0

// Packed string lists. NUL delimits items, double NUL terminates list
// Index:                                     0        1         2        3         4         5         6           7         8           9             10           11           12            13     14     15
// Character \x10 + the index in menu and docs expands to the token, eg \x1B -> "Chords". In docs tokens are highlighted
#define _S(s) s "\0"
const char btnLabelsAndTokens[] PROGMEM = _S("CLR")_S("DISP")_S("SET")_S("READ")_S("STOR")_S("STRT")_S("STOP")  _S("BitN")_S("EEPROM")_S("Built-in")_S("Auto-run")_S("Chords")_S("Opcodes")_S("P")_S("Q")_S("R");
const char groupLabelsAndTokens[] PROGMEM = _S("INP")_S("ADDR")_S("MEM")_S("RUN")  _S("Register")_S("Flags");

const char nameLabel[] PROGMEM = _S("KENBAK")_S("lite");

void Graphics::Init()
{
  lcd.init();
  dataBtnOffsetX = (lcd._width - 8*(btnSize + btnGapData) - TotalColumnGap(7, true) + btnGapData)/2;
  dataBtnOffsetY = lcd._height/2 - btnSize*1.25;
  ctrlBtnOffsetX = (lcd._width - 8*(btnSize + btnGapData) - TotalColumnGap(7, false) + btnGapData)/2;
  ctrlBtnOffsetY = lcd._height - btnSize*3.25;

  btnWhiteLoLight = HSVtoRGB(0, 0, 90);
  btnBlackHiLight = HSVtoRGB(0, 0, 10);  // closer to black starts to look green

  Paint();
}

void Graphics::Paint()
{
  int x;
  int y;
  int oct = 2;
  int hex = 8;
  char label[12];
  int scale = 1;
  int screwRad = 3;

  // the big blue lozenge shape
  int loz_dY = 2;
  int lozIndent = 2;  // from sides
  int lozY = 8; // from top/bottom
  int lozH = lcd._height - 2*lozY;
  int lozOffs = lozH/2/loz_dY + lozIndent;
  int lozThick = 2;
  lcd.fillColour(lcd.beginFill(0, 0, lcd._width, lcd._height), CASE_BLUE);
  int inflection = Lozenge(0, lozY, lcd._width, lozH, lozOffs, -1, loz_dY, lozThick, BLUE, BLACK, CASE_BACKGROUND, lozIndent);
  lcd.fillByte(lcd.beginFill(0, inflection - lozThick, lozIndent + 2*lozThick, 2*lozThick), 0x00);
  lcd.fillByte(lcd.beginFill(lcd._width - lozIndent - 2*lozThick, inflection - lozThick, lozIndent + 2*lozThick, 2*lozThick), 0x00);

  // name
  Font5x7::drawText((lcd._width - strlen(LoadStrN(nameLabel, 0, label))*(FONT5x7_WIDTH+1)*2) - lozOffs - lozThick - 3*screwRad - screwRad/2, lcd._height - lozY - (FONT5x7_HEIGHT+1+lozThick)*2 - screwRad - lozThick, LoadStrN(nameLabel, 0, label), BLACK, CASE_BACKGROUND, 2);
  Font5x7::drawText((lcd._width - strlen(LoadStrN(nameLabel, 1, label))*(FONT5x7_WIDTH+1)*2), lcd._height - lozY - (FONT5x7_HEIGHT+1)*2 - lozThick, LoadStrN(nameLabel, 1, label), WHITE, CASE_BLUE, 2);

  // credit, faded
  Font5x7::drawText(0, lcd._height - FONT5x7_HEIGHT, "MEW", HSVtoRGB(HUE_Blue, 200, VAL_Normal), CASE_BLUE, 1);

  // screws
  y = lozY + lozThick + screwRad;
  for (int screwRow = 0; screwRow < 2; screwRow++)
  {
    if (screwRow == 1) // bottom row
      y = lcd._height - lozY - lozThick - 3*screwRad;
    DrawCircle(lozOffs + 2*screwRad, y, screwRad, BTN_SHADOW, CASE_BACKGROUND, BLACK, true);
    DrawCircle(lcd._width/2 - screwRad, y, screwRad, BTN_SHADOW, CASE_BACKGROUND, BLACK, true);
    DrawCircle(lcd._width - lozOffs - 4*screwRad, y, screwRad, BTN_SHADOW, CASE_BACKGROUND, BLACK, true);
  }
    
  int lozGap = 4;
  lozH = FONT5x7_HEIGHT + lozGap;
  for (int btn = 0; btn < 8; btn++)
  {
    DrawDataBtn(btn, eUp);
    
    DrawDataLED(btn, false);
    
    x = dataBtnOffsetX + btn*(btnSize + btnGapData) + TotalColumnGap(btn, true);
    y = dataBtnOffsetY;

    // bit-N
    label[0] = '7' - btn;
    label[1] = 0;
    Font5x7::drawText(x + (btnSize - (FONT5x7_WIDTH+1)*scale)/2, y - btnLEDOffsetY - btnSize, label, BLACK, CASE_BACKGROUND, scale, true);
    
    x += (btnSize - 6)/2;
    
    // octal
    if (oct == 4 || btn == 0)
    {
      int span = (btn == 0)?2:3;
      int gaps = TotalColumnGap(btn + span - 1, true) - TotalColumnGap(btn, true);
      Lozenge(x - btnSize/2 + lozGap/2, y - btnLine1OffsetY - lozGap/2, span*(btnSize + btnGapData) - btnGapData + gaps, lozH, lozH/2, -1, +1, 1, CASE_BACKGROUND, BLACK, CASE_BACKGROUND);
    }
    label[0] = '0' + oct;
    oct >>= 1;
    if (!oct) 
      oct = 4;
    Font5x7::drawText(x, y - btnLine1OffsetY, label, BLACK, CASE_BACKGROUND, 1);
    
    // hex
    if (hex == 8)
    {
      int span = 4;
      int gaps = TotalColumnGap(btn + span - 1, true) - TotalColumnGap(btn, true);
      Lozenge(x - btnSize/2 + lozGap/2, y - btnLine2OffsetY - lozGap/2, span*(btnSize + btnGapData) - btnGapData + gaps, lozH, lozH/2, -1, +1, 1, CASE_BACKGROUND, BLACK, CASE_BACKGROUND);
    }
    label[0] = '0' + hex;
    hex >>= 1;
    if (!hex) 
      hex = 8;
    Font5x7::drawText(x, y - btnLine2OffsetY, label, BLACK, CASE_BACKGROUND, 1);
  }

  for (int btn = 0; btn < 7; btn++)
  {
    DrawCtrlBtn(btn, eUp);
    if (btn % 2 == 0)
      DrawCtrlLED(btn/2, false, 0xFF);

    x = ctrlBtnOffsetX + btn*(btnSize + btnGapCtrl) + TotalColumnGap(btn, false);
    y = ctrlBtnOffsetY;
    
    // group label
    if (btn % 2 == 0)
    {
      // make group 0 a bit wider than a btn so the line shows
      int wide = btnSize + btnGapCtrl/2;
      int leftX = x - btnGapCtrl/4;
      int textY = y - btnLine1OffsetY;
      if (btn)
      {
        wide = 2*btnSize + btnGapCtrl;
        leftX -= btnSize + btnGapCtrl - btnGapCtrl/4;
      }
      // draw the group lines
      lcd.fillByte(lcd.beginFill(leftX, textY + FONT5x7_HEIGHT/2, wide, 1), 0);
      int textX = leftX + (wide - (FONT5x7_WIDTH+1)*strlen(LoadStrN(groupLabelsAndTokens, btn/2, label)))/2;
      // gap before the text (except for the I in INP)
      if (btn)
        lcd.fillColour(lcd.beginFill(textX-1, textY, 1, FONT5x7_HEIGHT), CASE_BACKGROUND);
      Font5x7::drawText(textX, textY, label, BLACK, CASE_BACKGROUND, 1);
    }
    // btn labels
    int strWidth = (FONT5x7_WIDTH+1)*strlen(LoadStrN(btnLabelsAndTokens, btn, label)) - 1;  // compiler messes up calc without this intermediate?
    Font5x7::drawText(x + (btnSize - strWidth)/2, y - btnLine2OffsetY, label, BLACK, CASE_BACKGROUND, 1);
  }
}

word Graphics::HitTest()
{
  int tx, ty;
  int bx1, by1,  bx2, by2;
  if (lcd.getTouch(tx, ty))
  {
    if (MenuClicked(tx, ty))
    {
      return 0;
    }
    by1 = dataBtnOffsetY;
    by2 = by1 + btnSize;
    for (int btn = Buttons::eBit0; btn <= Buttons::eBit7; btn++)
    {
      bx1 = dataBtnOffsetX + btn*(btnSize + btnGapData) + TotalColumnGap(btn, true);
      bx2 = bx1 + btnSize;
      if (bx1 <= tx && tx <= bx2 && by1 <= ty && ty <= by2)
        return 0x80 >> btn;
    }
    
    by1 = ctrlBtnOffsetY;
    by2 = by1 + btnSize;
    for (int btn = Buttons::eInputClear; btn <= Buttons::eRunStop; btn++)
    {
      bx1 = ctrlBtnOffsetX + (btn - Buttons::eInputClear)*(btnSize + btnGapCtrl)  + TotalColumnGap(btn - Buttons::eInputClear, false);
      bx2 = bx1 + btnSize;
      if (bx1 <= tx && tx <= bx2 && by1 <= ty && ty <= by2)
        return 0x0100 << (btn - Buttons::eInputClear);
    }
  }
  return 0;
}


void Graphics::DrawDataLEDs(byte leds)
{
  for (int led = 0; led < 8; led++)
    DrawDataLED(led, leds & (0x80 >> led));
  lastDataLEDs = leds;
}

void Graphics::DrawControlLEDs(byte leds, byte runBrightness)
{
  for (int led = 0; led < 4; led++)
    DrawCtrlLED(led, leds & (0x01 << led), runBrightness);
  lastCtrlLEDs = leds;
  lastRunBrightness = runBrightness;
}

void Graphics::DrawBtn(int Btn, tBtnState state)
{
  if (Btn <= Buttons::eBit7)
    DrawDataBtn(Buttons::eBit7 - Btn, state);
  else
    DrawCtrlBtn(Btn - Buttons::eInputClear, state);
}

void Graphics::DrawDataBtn(int btn, tBtnState state)
{
  DrawBtn(dataBtnOffsetX + btn*(btnSize + btnGapData) + TotalColumnGap(btn, true), dataBtnOffsetY, btn > 3, state);
}

void Graphics::DrawCtrlBtn(int btn, tBtnState state)
{
  DrawBtn(ctrlBtnOffsetX + btn*(btnSize + btnGapCtrl) + TotalColumnGap(btn, false), ctrlBtnOffsetY, btn % 2, state);
}

void Graphics::DrawBtn(int x, int y, bool white, tBtnState state, const char* label)
{
  // label stuff is a hack for clock setting!
  if (state == eFlash)
  {
    DrawBtn(x, y, !white, eDown);
    delay(25);
    DrawBtn(x, y, white, eDown);
    return;
  }
  word face = white?WHITE:BLACK;
  // inefficient filling the whole thing...
  lcd.fillColour(lcd.beginFill(x, y, btnSize+3, btnSize+3), CASE_BACKGROUND);
  if (state == eUp)
  {
    // tall drop shadow
    lcd.fillColour(lcd.beginFill(x+3, y+3, btnSize, btnSize), BTN_SHADOW);
  }
  else
  {
    // short drop shadow
    lcd.fillColour(lcd.beginFill(x+2, y+2, btnSize, btnSize), BTN_SHADOW);
  }
  if (state != eUp)
  {
    // dip it
    x++;
    y++;
  }
   
  // button face
  lcd.fillColour(lcd.beginFill(x, y, btnSize, btnSize), face);
  
  if (label)
  {
    Font5x7::drawText(x + (btnSize - FONT5x7_WIDTH)/2, y + (btnSize - FONT5x7_HEIGHT)/2, label, ~face, face);
  }
  
  // hi/lo lights
  int inset = 0;
  int thickness = 2;
  if (white)
  {
    lcd.fillColour(lcd.beginFill(x + thickness, y + btnSize - inset - thickness, btnSize - 2*inset - thickness, thickness), btnWhiteLoLight);
    lcd.fillColour(lcd.beginFill(x + btnSize - inset - thickness, y + thickness, thickness, btnSize - 2*inset - 2*thickness), btnWhiteLoLight);
  }
  else
  {
    lcd.fillColour(lcd.beginFill(x + inset, y + inset, btnSize - 2*inset - thickness, thickness), btnBlackHiLight);
    lcd.fillColour(lcd.beginFill(x + inset, y + inset, thickness, btnSize - 2*inset - thickness), btnBlackHiLight);
  }
  
  if (state == eLocked)
    Font5x7::drawText(x + (btnSize - FONT5x7_WIDTH*2)/2, y + (btnSize - FONT5x7_HEIGHT*2)/2, "\x0E", ~face, face, 2);
}

void Graphics::DrawDataLED(int led, bool on)
{
  DrawLED(dataBtnOffsetX + led*(btnSize + btnGapData) + (btnSize - ledSize)/2 + TotalColumnGap(led, true), dataBtnOffsetY - btnLEDOffsetY, on?DATA_LED_ON:DATA_LED_OFF, on);
}

void Graphics::DrawCtrlLED(int led, bool on, byte runBrightness)
{
  if (led == 3 && on && runBrightness != 0xFF)
    DrawLED(ctrlBtnOffsetX + 2*led*(btnSize + btnGapCtrl) + (btnSize - ledSize)/2 + TotalColumnGap(led, false), ctrlBtnOffsetY - btnLEDOffsetY, HSVtoRGB(HUE_Yellow, SAT_Highlight, runBrightness), on);
  else
    DrawLED(ctrlBtnOffsetX + 2*led*(btnSize + btnGapCtrl) + (btnSize - ledSize)/2 + TotalColumnGap(led, false), ctrlBtnOffsetY - btnLEDOffsetY, on?CTRL_LED_ON:CTRL_LED_OFF, on);
}

void Graphics::DrawLED(int x, int y, word colour, bool on)
{
  DrawCircle(x, y, ledRadius, colour, CASE_BACKGROUND, on?colour:BLACK);
}

int Graphics::Lozenge(int x, int y, int w, int h, int inset, int dX, int dY, int thick, word out, word line, word in, int indent)
{
  /*                _____________
     draws a shape /             \
                   \_____________/ 
     top left at x,y, total w & h. inset is from topleft to _
     angled line dec dX every dY                           /
     colours are inside, line & outside
     returns y at inflection
  */
  int inflection = 0;
  lcd.beginFill(x, y, w, h);
  for (int row = 0; row < h; row++)
  {
    if (row < thick || row >= (h - thick))
    {
      // top and bottom
      lcd.fillColour(inset + indent, out);
      lcd.fillColour(w - 2*(inset + indent), line);
      lcd.fillColour(inset + indent, out);
    }
    else
    {
      // middle
      lcd.fillColour(inset + indent, out);
      lcd.fillColour(thick, line);
      lcd.fillColour(w - 2*(inset + thick + indent), in);
      lcd.fillColour(thick, line);
      lcd.fillColour(inset + indent, out);
    }
    if (row % dY == 0)
      inset += dX;
    if (inset < indent)
    {
      inflection = y + row;
      inset = indent - inset;
      dX = -dX;
    }
  }
  return inflection;
}

void Graphics::AddCircleData(int x, int y, int r)
{
  // update raster data with pixel info
  if (x <= r && y <= r)
  {
    // only care about top-left quad, plus central row/col
    if (x < circleRasterOffset[y])
      circleRasterOffset[y] = x;
    
    circleRasterLength[y]++;
  }
}

bool Graphics::BuildCircleData(int radius)
{
  // use Bresenham to determine pixels on circle, but convert to raster
  if (radius > ledRadius)
    return false;  // data table is fixed size
  for (int i = 0; i <= radius; i++)
  {
    circleRasterOffset[i] = 2*radius;
    circleRasterLength[i] = 0;
  }
  int r = radius;
  int x = -r;
  int y = 0;
  int err = 2 - 2*r;
  int cenX = r;
  int cenY = r;
  
  do 
  {
    AddCircleData(cenX - x, cenY + y, radius);
    AddCircleData(cenX - y, cenY - x, radius);
    AddCircleData(cenX + x, cenY - y, radius);
    AddCircleData(cenX + y, cenY + x, radius);
    r = err;
    if (r <= y) 
      err += (++y)*2 + 1;
    if (r > x || err > y)
      err += (++x)*2 + 1;
  } while (x < 0);
  
  circleRasterRadius = radius;
  return true;
}

void Graphics::DrawCircle(int x, int y, int r, word fg, word bg, word line, bool diagonal)
{
  if (r != circleRasterRadius)
  {
    // compute rasterization on demand
    if (!BuildCircleData(r))
      return;
  }
  int Size = 2*r + 1;
  lcd.beginFill(x, y, Size, Size);
  for (int row = 0; row < Size; row++)
  {
    int dataRow = row;
    if (dataRow > r)
      dataRow = Size - dataRow - 1;
      
    int ofs = circleRasterOffset[dataRow];
    int len = circleRasterLength[dataRow];
    // fill to left edge
    lcd.fillColour(ofs, bg);
    // left edge
    lcd.fillColour(len, line);
    if (row == 0 || row == Size - 1)
    {
      lcd.fillColour(len - 1, line);   // top and bottom lines, no inside
    }
    else
    {
      // inside & right edge
      if (diagonal && ofs < dataRow)
      {
        // add a diagonal line (screw slot)
        int totalInside = 2*(r - ofs - len) + 1;
        int Step = dataRow - ofs;
        if (row > r)
          Step = totalInside - Step + 1;
        lcd.fillColour(Step - 1, fg);
        lcd.fillColour(1, line);
        lcd.fillColour(totalInside - Step, fg);
      }
      else
      {
        lcd.fillColour(2*(r - ofs - len) + 1, fg);
      }
      lcd.fillColour(len, line);
    }
    // fill to right side
    lcd.fillColour(ofs, bg);
  }
}

byte Graphics::TotalColumnGap(int btn, bool data)
{
  // encoded counts of column gaps, 2 bits each, accumulating from the left
  word encoded = data?0b1010100101010000:0b1111111010010100;
  int gap = data?6:3;
  int count = (encoded >> 2*btn) & 0x03;
  return gap*count;
}

// simple menu system
void Graphics::ShowMenu(bool normal)
{
  if (normal)
    Font5x7::drawText(2, 2, "\x0F", WHITE, CASE_BLUE, 2); // \x0F is a "hamburger" char
  else
    Font5x7::drawText(2, 2, "\x0F", CASE_BLUE, WHITE, 2);
}

int Graphics::DrawMenu(int x, int y, const char* items, int highlight, word fg, word bg, int& lineHeight, int& maxWidth)
{
  char itemStr[12];
  const char* ptr = items;
  int line = 0;
  int itemLen;
  lineHeight = (FONT5x7_HEIGHT + 1) + 4;
  maxWidth = 0;
  while (pgm_read_byte_near(ptr))
  {
    strcpy_P(itemStr, ptr);
    itemLen = strlen(itemStr);
    char ch = itemStr[0];
    if (itemLen >= 1 && '\x10' <= ch && ch <= '\x1F')
    {
      // prefix w/ token
      char tempStr[12];
      LoadStrN(btnLabelsAndTokens, ch - '\x10', tempStr);
      strcat(tempStr, itemStr + 1);
      strcpy(itemStr, tempStr);
    }
    int strWidth = strlen(itemStr)*(FONT5x7_WIDTH + 1) + 1;
    if (highlight == line)
    {
      lcd.fillColour(lcd.beginFill(x, y + line*lineHeight, strWidth, lineHeight), fg);
      Font5x7::drawText(x + 1, y + line*lineHeight + 2, itemStr, bg, fg, 1, false, true);
    }
    else
    {
      lcd.fillColour(lcd.beginFill(x, y + line*lineHeight, strWidth, lineHeight), bg);
      Font5x7::drawText(x + 1, y + line*lineHeight + 2, itemStr, fg, bg, 1, false, true);
    }
    ptr += itemLen + 1;
    maxWidth = max(maxWidth, strWidth);
    line++;
  }
  return line;
}

int Graphics::DoMenu(const char* items)
{
  int x = 1, y = 1;
  int selection = -1;
  int tx, ty;
  WaitForUp();

  int lineHeight, maxWidth;
  int lines = DrawMenu(x, y, items, -1, WHITE, CASE_BLUE, lineHeight, maxWidth);
  
  WaitForDown(tx, ty);
  int row = (ty - y)/lineHeight;
  if (tx <= (x + maxWidth) && row < lines)
  {
    DrawMenu(x, y, items, row, WHITE, CASE_BLUE, lineHeight, maxWidth);
    selection = row;
    WaitForUp();
  }
  DrawMenu(x, y, items, -1, CASE_BLUE, CASE_BLUE, lineHeight, maxWidth);
  return selection;
}

const char topMenu[] PROGMEM = _S("KENBAK-1")
                               _S("Emulator");
                               
const char kenbakMenu[] PROGMEM = _S("\x1C""1")  // ="Opcodes1" 
                                  _S("\x1C""2")  // ="Opcodes2" 
                                  _S("Memory");

const char emulatorMenu[] PROGMEM = _S("Programs")
                                    _S("SysInfo")
                                    _S("\x1B")    // ="Chords"
                                    _S("Config");

const char autoMenu[] PROGMEM = _S("\x19") // ="Built-in"
                                _S("\x18") // ="EEPROM"
                                _S("None");
                                
const char slotMenu[] PROGMEM = _S("#0")
                                _S("#1")
                                _S("#2")
                                _S("#3")
                                _S("#4")
                                _S("#5")
                                _S("#6")
                                _S("#7");
                                
const char configMenu[] PROGMEM = _S("\x1A")  // ="Auto-run"
                                  _S("Reset")
                                  _S("Clock");
                                  
const char confirmMenu[] PROGMEM = _S("Yes")
                                   _S("No");
                                
bool Graphics::MenuClicked(int tx, int ty)
{
  if (tx <= btnSize && ty <= btnSize)
  {
    word BtnState = 0;
    bool repaint = false;
    ShowMenu(false);
    int menu1 = DoMenu(topMenu);
    if (menu1 == 0) // KENBAK
    {
      int menu2 = DoMenu(kenbakMenu);
      if (menu2 == 0) // OpCodes1
        RenderText(Doc_OpCodes(true), btnLabelsAndTokens);
      else if (menu2 == 1)  // OpCodes2
          RenderText(Doc_OpCodes(false), btnLabelsAndTokens);
      else if (menu2 == 2)  // Memory
        RenderText(Doc_Memory(), groupLabelsAndTokens);
      repaint = menu2 >= 0;
    }
    else if (menu1 == 1) // Emulator
    {
      int menu1 = DoMenu(emulatorMenu);
      if (menu1 == 0) // Programs
      {
        char RTCStr[5];
        LoadStrN(confirmMenu, Clock::RTC_I2C_ADDR?0:1, RTCStr); // load RTC token
        RenderText(Doc_Programs(), btnLabelsAndTokens, true, RTCStr);
        repaint = true;
      }
      else if (menu1 == 1)  // SysInfo
      {
        RenderText(Doc_SysInfo(), btnLabelsAndTokens, false);
        repaint = true;
      }
      else if (menu1 == 2)  // Chords
      {
        RenderText(Doc_Chords(), btnLabelsAndTokens);
        repaint = true;
      }
      else if (menu1 == 3)  // Config
      {
        menu1 = DoMenu(configMenu);
        if (menu1 == 0)
        {
          int menu2 = DoMenu(autoMenu);
          if (menu2 == 0 || menu2 == 1)
          {
              int menu3 = DoMenu(slotMenu);
              if (0 <= menu3 && menu3 <= 7)
              {
                // set auto-run
                bitWrite(BtnState, (menu1 == 0)?Buttons::eRunStop:Buttons::eMemoryRead, 1);
                bitWrite(BtnState, Buttons::eBit0 + menu3, 1);
              }
          }
          else if (menu2 == 2)
          {
            // clear auto-run
            bitWrite(BtnState,Buttons::eRunStop, 1);
          }
        }
        else if (menu1 == 1)
        {
          if (DoMenu(confirmMenu) == 0)
          {
            // reset cfg
            bitWrite(BtnState,Buttons::eRunStop, 1);
            bitWrite(BtnState,Buttons::eInputClear, 1);
          }
        }
        else if (menu1 == 2)
        {
          CfgClock();
          repaint = true;
        }
      }
    }
    if (repaint)
    {
      Paint();
      DrawDataLEDs(lastDataLEDs);
      DrawControlLEDs(lastCtrlLEDs, lastRunBrightness);
      buttons.m_wHeld = 0x00;
      for (int btn = 0; btn < 7; btn++)
        DrawCtrlBtn(btn, eUp);
    }
    if (BtnState)
      config.ApplyStartupConfig(BtnState);
    WaitForUp();
    ShowMenu();
    return true;
  }
  return false;
}

void Graphics::TouchTest()
{
  // check the touch calibration, XC4630_TOUCHX0 etc in XC4630d.h
  int tx, ty;
  int px = -1;
  int py = -1;
  lcd.fillByte(lcd.beginFill(0, 0, lcd._width, lcd._height), 0x00);
  Font5x7::drawText(0, 0, "Touch test", WHITE, BLACK, 1);
  
  while (true)
  {
    if (lcd.getTouch(tx, ty))
    {
      for (int colour = 0; colour <= 1; colour++)
      {
        if (px != -1)
        {
          lcd.fillByte(lcd.beginFill(px, 0, 1, lcd._height), 0xFF*colour);
          lcd.fillByte(lcd.beginFill(0, py, lcd._width, 1), 0xFF*colour);
        }
        px = tx;
        py = ty;
      }
    }
  }
} 

//#define SERIALIZE_TEXT  // dump the doc to serial
void Graphics::RenderText(const char* pText, const char* pTokens, bool highlightTokens, char* pToken0)
{
  // '\x1n'->token n
  // '/n'->newline, '/r'->newline with horz line, '/t'->next multiple of 7 columns, ',', ':' & '=' get trailiing space, '=' gets leading space
  int margin = 2;
  int lineHeight = (FONT5x7_HEIGHT + 3);
  int cols = (lcd._width - 2*margin)/(FONT5x7_WIDTH + 1);
  int lines = (lcd._height - 2*margin)/lineHeight;
  int col = 0;
  int line = 0;
  int tabSize = 7;
  char wordStr[20];
  bool lineBreak = false;
  bool textEffect = true;
  word fore = BLACK;
  word effect = BLUE;
  lcd.fillByte(lcd.beginFill(0, 0, lcd._width, lcd._height), 0xFF);
  
  while (line < lines)
  {
    char ch = pgm_read_byte_near(pText++);
    char* pWord = wordStr;
    
    // load buffer:
    if ('\x10' <= ch && ch <= '\x1F')
    {
      // token
      LoadStrN(pTokens, ch - '\x10', wordStr);
      pWord += strlen(wordStr);
      textEffect = highlightTokens;
    }
    else if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
    {
      // word
      while (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
      {
        *(pWord++) = ch;
        ch = pgm_read_byte_near(pText++);
      }
      pText--;
    }
    else if (ch == '\n')
    {
      lineBreak = true;
    }
    else if (ch == '\r')  // newline with horizonal line
    {
      lineBreak = true;
      col++;
      lcd.fillColour(lcd.beginFill(margin + col*(FONT5x7_WIDTH + 1), margin + line*lineHeight + FONT5x7_HEIGHT/2, lcd._width - (margin + col*(FONT5x7_WIDTH + 1)) - margin, 1), fore);
#ifdef SERIALIZE_TEXT
for (int tab = col; tab < cols; tab++)
  Serial.print('-');
#endif
    }
    else if (ch == '\t')
    {
#ifdef SERIALIZE_TEXT
for (int tab = col; tab < (col/tabSize + 1)*tabSize; tab++)
  Serial.print(' ');
#endif
      col = (col/tabSize + 1)*tabSize;
    }
    else if (ch == 0)
    {
      break;
    }
    else if (ch == '\xFF')
    {
      textEffect = true;
      continue;
    }
    else if (ch == '\x01')
    {
      if (pToken0)
      {
        strcpy(wordStr, pToken0);
        pWord += strlen(wordStr);
      }
      else
        continue;
    }
    else
    {
      // just a char (but '=' -> ' = ', ':' -> ': ', ',' -> ', ')
      if (ch == '=')
        *(pWord++) = ' ';
      *(pWord++) = ch & 0x7F; // eg special '=' in "!="
      if ((ch == ':' || ch == '=' || ch == ','))
        *(pWord++) = ' ';
    }
    *pWord = 0;
    int len = strlen(wordStr);
    if (col + len > cols)
    {
      // word wrap
      lineBreak = true;
    }
    if (lineBreak)
    {
#ifdef SERIALIZE_TEXT
Serial.println();
#endif
      col = 0;
      line++;
    }
    Font5x7::drawText(margin + col*(FONT5x7_WIDTH + 1), margin + line*lineHeight, wordStr, textEffect?effect:fore, WHITE, 1, false, true);
#ifdef SERIALIZE_TEXT
Serial.print(wordStr);
#endif
    lineBreak = textEffect = false;
    col += len;
  }
  int tx, ty;
  WaitForDown(tx, ty);
}

char* Graphics::LoadStrN(const char* list, int N, char* buff)
{
  // load from NUL delimited string list in PROGMEM
  strcpy_P(buff, list);
  int strLen = strlen(buff); 
  while (N && strLen)
  {
    list += strLen + 1;
    strcpy_P(buff, list);
    strLen = strlen(buff); 
    N--;
  }
  return buff;
}

void Graphics::WaitForUp()
{
  // wait for touch up, with semi-debounce
  int tx, ty;
  while (lcd.getTouch(tx, ty))
    ;
  delay(10);
  while (lcd.getTouch(tx, ty))
    ;
}

void Graphics::WaitForDown(int& tx, int& ty)
{
  // wait for touch down, with semi-debounce
  int tx1, ty1;
  while (true)
  {
    while (!lcd.getTouch(tx1, ty1))
      ;
    delay(10);
    while (!lcd.getTouch(tx, ty))
      ;

    if (abs(tx-tx1) < 5 && abs(ty-ty1) < 5)
      break;
  }
}

void Graphics::CfgClock()
{
  int tx, ty;
  WaitForUp();
  lcd.fillColour(lcd.beginFill(0, 0, lcd._width, lcd._height), CASE_BACKGROUND);
  if (!Clock::RTC_I2C_ADDR)
  {
    Font5x7::drawText(10, 10, "No Clock!", RED, CASE_BACKGROUND, 2);    
    WaitForDown(tx, ty);
    return;
  }
  Font5x7::drawText(10, 10, "Set time:", BLACK, CASE_BACKGROUND);

  byte mins = clock.BCD2Dec(config.Read(Config::eClockMinutes));
  byte hour = clock.BCD2Dec(config.Read(Config::eClockHours24));
  char digit[2];
  digit[1] = 0;
  int x = 10;
  int y = 20;
  int dX = btnSize + 4;
  bool first = true;
  while (true)
  {
    int idx = 0;
    digit[0] = '0' + (hour / 10);
    DrawBtn(x + dX*idx++, y, true, eUp, digit);
    digit[0] = '0' + (hour % 10);
    DrawBtn(x + dX*idx++, y, true, eUp, digit);
    digit[0] = '0' + (mins / 10);
    DrawBtn(x + dX*idx++, y, true, eUp, digit);
    digit[0] = '0' + (mins % 10);
    DrawBtn(x + dX*idx++, y, true, eUp, digit);
    if (first)
    {
      idx++;  // gap
      DrawBtn(x + dX*idx++, y, false, eUp, "\x0D"); // tick
      DrawBtn(x + dX*idx++, y, false, eUp, "x");  // cross
      first = false;
    }
    WaitForDown(tx, ty);
    if ((y - 3) < ty && ty <= (y + btnSize + 3))
    {
      int btn = (tx - x)/ dX;
      if (0 <= btn && btn <= 3)
      {
        DrawBtn(x + btn*dX, y, true, eFlash);
        WaitForUp();
        if (btn <= 1)
          hour += (btn==0)?10:1;
        else
          mins += (btn==2)?10:1;
        hour %= 24;
        mins %= 60;
      }
      else if (btn == 5)
      {
        // set the time
        DrawBtn(x + btn*dX, y, false, eFlash);
        WaitForUp();
        config.Write(Config::eClockSeconds, 0);
        config.Write(Config::eClockMinutes, clock.Dec2BCD(mins));
        config.Write(Config::eClockHours24, clock.Dec2BCD(hour));
        break;        
      }
      else if (btn == 6)
      {
        DrawBtn(x + btn*dX, y, false, eFlash);
        WaitForUp();
        break;        
      }
    }
  }
}
