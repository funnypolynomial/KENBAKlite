#include <Arduino.h>
#include "HSV.h"

// convert Hue, Saturation & Value to Red, Green & Blue
// based on web.mit.edu/storborg/Public/hsvtorgb.c, refactored
word HSVtoRGB(byte h, byte s, byte v)
{
  byte region;
  unsigned short r, g, b, fpart;

  // one of 6 hue regions
  region = h / 43;
  // remainder within region, scaled 0-255
  fpart = (h - (region * 43)) * 6L;

  switch (region)
  {
    case 0:
      r = v;
      g = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8; // t
      b = (v * (255 - s)) >> 8; // p
      break;
    case 1:
      r = (v * (255 - ((s * fpart) >> 8))) >> 8; // q
      g = v;
      b = (v * (255 - s)) >> 8; // p
      break;
    case 2:
      r = (v * (255 - s)) >> 8; // p
      g = v;
      b = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;  // t
      break;
    case 3:
      r = (v * (255 - s)) >> 8; // p
      g = (v * (255 - ((s * fpart) >> 8))) >> 8; // q
      b = v;
      break;
    case 4:
      r = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;  // t
      g = (v * (255 - s)) >> 8; // p
      b = v;
      break;
    case 5:
      r = v;
      g = (v * (255 - s)) >> 8; // p
      b = (v * (255 - ((s * fpart) >> 8))) >> 8; // q
      break;
    default:
      return 0;
  }

  // RGB 565
  return ((b & 0xF8) >> 3) |
         ((g & 0xFC) << 3) |
         ((r & 0xF8) << 8);
}

