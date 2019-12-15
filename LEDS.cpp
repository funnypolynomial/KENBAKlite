#include <Arduino.h>
#include "PINS.h"
#include "MCP.h"
#include "Graphics.h"
#include "LEDS.h"

void LEDs::Init()
{
  m_LastData = 0;
  m_LastControl = 0;
}

void LEDs::Display(byte Data, byte Control)
{
  // update the data and control LEDs
  if (Data != m_LastData)
  {
    graphics.DrawDataLEDs(Data);
    m_LastData = Data;
  }
  
  if (Control != m_LastControl)
  {
    byte runBrightness = 0xFF - (Control & 0xF0);
    graphics.DrawControlLEDs(Control, runBrightness);
    m_LastControl = Control;
  }
}

LEDs leds = LEDs();

