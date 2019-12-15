#include <Arduino.h>
#include "SoftwareI2C.h"
#include <EEPROM.h>
#include "PINS.h"
#include "Clock.h"
#include "Config.h"
#include "Memory.h"

// Use softwire for I2C to RTC on selected pins
SoftwareI2C softWire;

Clock::Clock()
{
}

void Clock::Init()
{
  if (RTC_I2C_ADDR)
  {
    delay(100);
    softWire.begin(PIN_RTC_SDA, PIN_RTC_SCL);
  }
}

byte Clock::BCD2Dec(byte BCD)
{
  return (BCD/16*10) + (BCD & 0x0F);
}

byte Clock::Dec2BCD(byte Dec)
{
  return (Dec/10*16) + (Dec % 10);
}

// 0:CLK_SECONDS
// 1:CLK_MINUTES
// 2:CLK_HOURS
// 3:CLK_DAY
// 4:CLK_DATE
// 5:CLK_MONTH
// 6:CLK_YEAR
// 7:CLK_CONTROL

byte Clock::ReadByte(byte Index)
{
  // general "read"
  if (Index <= Config::eClockControl)
  {
    // reading a clock register
    if (RTC_I2C_ADDR)
      return ReadRTCByte(Index);
    else// no RTC, return 1/1/00, 12:00:00 day=1
      if (Index == Config::eClockHours24)
        return 0x12;
      else
        return (0b00111000 >> Index) & 0x01; 
  }
  else  // reading config/user byte from RTC's SRAM, or EEPROM
  {
    if (RTC_I2C_ADDR && RTC_USER_SRAM_OFFSET)  // RTC's SRAM
      return ReadRTCByte(Index - Config::eControlFlags + RTC_USER_SRAM_OFFSET);
    else
    {
      int idx = memory.GetEEPROMTopIdx() + Index - Config::eControlFlags + 1;
      return EEPROM.read(idx);
    }      
  }
}

void Clock::WriteByte(byte Index, byte Value)
{
  // general "write"
  if (Index <= Config::eClockControl)
  {
    // writing a clock register
    if (RTC_I2C_ADDR)
      return WriteRTCByte(Index, Value);
    // else no RTC, ignore
  }
  else  // writing config/user byte to RTC's SRAM, or EEPROM
  {
    if (RTC_I2C_ADDR && RTC_USER_SRAM_OFFSET)  // RTC's SRAM
      return WriteRTCByte(Index - Config::eControlFlags + RTC_USER_SRAM_OFFSET, Value);
    else
    {
      int idx = memory.GetEEPROMTopIdx() + Index - Config::eControlFlags + 1;
      return EEPROM.write(idx, Value);
    }
  }
}

// clock specific
byte Clock::ReadRTCByte(byte Index)
{
  if (Index == Config::eClockControl && RTC_CONTROL_OFFSET == 0x00) 
    return 0x00;
  Index = (Index == Config::eClockControl)?RTC_CONTROL_OFFSET:Index;  // adjust control register
  softWire.beginTransmission(RTC_I2C_ADDR);
  softWire.write(Index);
  softWire.endTransmission();
 
  softWire.requestFrom(RTC_I2C_ADDR, 1);
  return softWire.read();
}

void Clock::WriteRTCByte(byte Index, byte Value)
{
  if (Index == Config::eClockControl && RTC_CONTROL_OFFSET == 0x00) 
    return;
  Index = (Index == Config::eClockControl)?RTC_CONTROL_OFFSET:Index;  // adjust control register
  softWire.beginTransmission(RTC_I2C_ADDR);
  softWire.write(Index);
  softWire.write(Value);
  softWire.endTransmission();
}

Clock clock = Clock();
