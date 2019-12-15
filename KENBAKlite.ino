// ==================================================================
// ==================================================================
//      K E N B A K u i n o (lite)
//      Mark Wilson 2011
//  Software emulation of a KENBAK-1.
//  Released under Creative Commons Attribution, CC BY
//  (kiwimew at gmail dot com)
//  Sep 2011: Initial version.
//  Jan 2012: Changes to compile under v1.0 if the IDE (Arduino.h, changes to Wire)
//  May 2014: Corrected control switch order in schematic, 
//            typos in documents (no code changes)
//  Jun 2015: Changes to compile under v1.6.4 if the IDE, (#define for deprecated prog_uchar)  
//  Dec 2018: Corrected SDA/SCL pins, clarified that RTC is a DS1307 module/breakout (i.e. with XTAL)
//            GitHub. Serial read/write memory
//  May 2019: Added Auto-run program at start-up
//            Added Clock consts for no RTC or RTC with no SRAM
//  Jun 2019: Added system extensions to read/write EEPROM
//  -------------------------------------------------------
//  Sep 2019: Experimental Graphical version --> KENBAKlite
//            LEDs and buttons are displayed on the LCD shield along with a facsimile of the KENBAK front panel.
//            Buttons are emulated by clicks on the LCD's touch screen.
//            To implement chords, buttons held down lock so they can combine with the next button press.
//            To make room for the additional code, all the built-in programs are stored as raw PROGMEM bytes, 
//            the asm versions are #ifdef'd out
//            There's a menu with access to the equivalents of the power-on chords
//            The menu also gives access to some terse documentation pages
//           
// ==================================================================

// WTF.  We need to include these in the "main sketch" if we want to include them in "tabs"?
#include <Arduino.h>
// Libraries:
#include <SoftwareI2C.h>

#include "Graphics.h"
#include "PINS.h"
#include "Config.h"
#include "Clock.h"
#include "LEDS.h"
#include "Buttons.h"
#include "CPU.h"
#include "MCP.h"
#include "Memory.h"

ExtendedCPU cpu = ExtendedCPU();

void setup() 
{
  Serial.begin(38400);
  SERIALISE_ON(false);
  graphics.Init();
  // to check/adjust the touch calibration, XC4630_TOUCHX0 etc in XC4630d.h
  //graphics.TouchTest();
  clock.Init();
  buttons.Init();
  config.Init();
  leds.Init();
  cpu.Init();
  memory.Init();
  mcp.Init();
  graphics.ShowMenu();
}

void loop() 
{
  mcp.Loop();
}
