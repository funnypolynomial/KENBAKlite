#include <Arduino.h>
#include "Docs.h"

// Documentation texts in PROGMEM

// text contains embedded chars like \x1n which are replaced with the corresponding nth token from btnLabelsAndTokens or groupLabelsAndTokens
// '\n'->newline, '\r'->newline with horz line, '\t'->next multiple of 7 columns, ',', ':' & '=' get trailiing space, '=' gets leading space
// a '\xFF' highlights the next word
// "word " -> "wor<d+0x80>" would save 150 bytes, plus code

// Chord btn presses etc
static const char kChords[] PROGMEM =
  "\x1B:\n"
  "Press + hold btn1 until \x0E appears then press btn2\n"
  "\x16+\x10:blank display\n"
  "\x16+\x17:load \x19 program N\n"
  "N=0:Counter 1:Cylon 2:CountClock 3:BCDClock 4:BinClock 5:Blinken 6:Sieve 7:ClockSet\n"
  "\x10+\x14:clear memory\n"
  "\x17+\x13:read from \x18 slot N\n"
  "\x17+\x14:write to \x18 slot N\n"
  "\x16+\x13/\x14:sys read/write,index in address,value in display\n"
  "\x17+\x16:set CPU speed to N\n"
  "N=delay of 2^N ms\n"
  "\x17+\x11:write memory to Serial\n"
  "\x17+\x12:set memory from Serial\n"
  "N=0:4800 1:9600 2:19k2 3:38k4 4:4800 5:9600 6:19k2 7:38k4\n"
  "\x16+\x15:single step,\x16 \x0E is not cleared\n\n"
  "\x1A program etc set via the \x0F menu";

// terse op-code descriptions in two pages
static const char kOpCodes1[] PROGMEM =
  "\x1C are octal \x1D\x1E\x1F\n"
  "Misc\r"
  "\x1D\t\t\x1E\t\t\x1F\n"
  "0,1=Halt\tN/A\t\t0\n"
  "2,3=NOP\n"
  "0360=System Info/Extension\n"
  "\n"
  "Shift + Rotate\r"
  "\x1D\t\t\x1E\t\t\x1F\n"
  "0=Rt Shift\n"
  "1=Rt Rot\t\t\t1\n"
  "2=Lt Shift\n"
  "3=Lt Rot\n"
  "\x1E is bin mnn,m=0/1=A/B,amount is nn,00 is 4\n"
  "\n"
  "Bit Test + Set\r"
  "\x1D\t\t\x1E\t\t\x1F\n"
  "0=Set  0\n"
  "1=Set  1\n"
  "2=Skip 0\t\t\t2\n"
  "3=Skip 1\n"
  "\x1E is bit number";

static const char kOpCodes2[] PROGMEM =
  "\x1C are octal \x1D\x1E\x1F\n"
  "Jumps\r"
  "\x1D\t\t\x1E\t\t\x1F\n"
  "0=A\t\t4=Direct\t3 !\xBD 0\n"  // add high bit to '=' char to avoid extra spaces
  "1=B\t\t5=Indirect\t4 \xBD\xBD 0\n"
  "2=X\t\t6=Dir+Mark\t5 <\xBD 0\n"
  "3=Uncond\t7=Ind+Mark\t6 >\xBD 0\n"
  "\t\t\t\t7  > 0\n"
  "\n"
  "Or,And,LNeg\r"
  "\x1D\t\t\x1E\t\t\x1F\n"
  "3\t\t0=Or\t\t3=Const\n"
  "\t\t1=NOP\t4=Mem\n"
  "\t\t2=And\t5=Ind\n"
  "\t\t3=LNeg\t6=Idx\n"
  "\t\t\t\t7=Ind/Idx\n"
  "\n"
  "Add,Sub,Load,Store\r"
  "\x1D\t\t\x1E\t\t\x1F\n"
  "0=A\t\t0=Add\t3=Const\n"
  "1=B\t\t1=Sub\t4=Mem\n"
  "2=X\t\t2=Load\t5=Ind\n"
  "\t\t3=Store\t6=Idx\n"
  "\t\t\t\t7=Ind/Idx"  ;

// the reserved locations in memory
static const char kMemory[] PROGMEM =
  "Memory map:\n"
  "Octal\r"
  "000=\x14 A\n"
  "001=\x14 B\n"
  "002=\x14 X\n"
  "003=\x14 P\n"
  "\n"
  "200=\xFFOutput\n"
  "201=\x15 A\n"
  "202=\x15 B\n"
  "203=\x15 X\n"
  "\n"
  "377=\xFFInput";

static const char kSysInfo[] PROGMEM =
  "SysInfo/SYSX\n"
  "0360 with Index in A,Value in B\n"
  "Write/read=hi bit set/clear\n"
  "Index\r"
  "000-007:RTC Sec,Min,Hr,Day,Date,Mon,Yr,Ctrl\n"
  "010:Flags\n"
  "011:\x18 map\n"
  "012-016:User\n"
  "017:\x1A\n"
  "020:Ctrl LEDs,4 hi bits=RUN intensity\n"
  "021:Random\n"
  "022:Delay ms\n"
  "024-030:\x18,RAM offsets,\x18 size,overlay,page\n"
  "177:Check";

static const char kPrograms[] PROGMEM =
  "\x19 programs\n"
  "\x16+\x17 loads program N,clocks display 12-hour,no AM/PM\n"
  "\r"
  "0:Count=increments counter\n"
  "1:Pattern=back-and-forth\n"
  "2:Count Clock=Hr is P-HHHHHH,sum of H's +6 if P\n"
  "Min is B-MMMMM,sum is 10's,+5 if left M blinks\n"
  "B blinks\n"
  "3:BCD Clock=Hr is ---Hhhhh,min is B-MMMmmmm,BCD\n"
  "4:Binary Clock=Hr is ctrl LEDs,min is Bmmmmmm\n"
  "5:Bas Blinken Lights=random pattern\n"
  "6:Sieve of Eratosthenes=primes < 256,binary\n"
  "7:Set clock,hour in A,minutes in B,BCD,24-hour format\n"
  "\n"
  "RTC present:\xFF\x01";

const char* Doc_Chords()
{
  return kChords;
}

const char* Doc_OpCodes(bool firstPage)
{
  return firstPage?kOpCodes1:kOpCodes2;
}

const char* Doc_Memory()
{
  return kMemory;
}

const char* Doc_SysInfo()
{
  return kSysInfo;
}

const char* Doc_Programs()
{
  return kPrograms;
}
