#include <Arduino.h>
#include "PINS.h"
#include "Graphics.h"
#include "Buttons.h"

void Buttons::Init()
{
  // set no prev state
  m_wPrevState = 0xFFFF;
  m_wPrevReading = 0xFFFF;
  m_iTransitionTimeMS = m_iHeldTimeMS = millis();
  m_wHeld = 0;
}

word Buttons::ShiftIn()
{
  return graphics.HitTest();
}

#if 0
// Time-based debouncing, ifdef'd out because I'm not sure of the impact on execution
bool Buttons::GetButtons(word& State, word& NewPressed, bool deBounce)
{
  // get the current raw state and any that have changed to down
  unsigned long nowMS = millis();
  word ThisState = ShiftIn();
  bool deBounced = !deBounce;
  if (deBounce)
  {
      if (ThisState != m_wPrevReading)
      {
        // state change, reset the timer
        m_wPrevReading = ThisState;
        m_iTransitionTimeMS = nowMS;
      }
      else if ((nowMS - m_iTransitionTimeMS) >= 20UL)
      {
          // held for long enough
          deBounced = true;
      }
  }
  
  if (ThisState != m_wPrevState && deBounced)
  {
    NewPressed = ThisState & (~m_wPrevState);  // only those that have *changed* from OFF to ON, i.e. DOWN

    m_wPrevState = ThisState;
    State = ThisState;
    return true;
  }
  return false;
}
#else
bool Buttons::GetButtons(word& State, word& NewPressed, bool Wait)
{
  // get the current raw state and any that have changed to down
  word ThisState = ShiftIn();
  
  if (Wait)
  {
    // simple de-bounce, if requested
    delay(20);
    word DebouncedState = ShiftIn();
    if (DebouncedState != ThisState)
      return false;
  }
  
  if (ThisState != m_wPrevState)
  {
    word OldState = m_wPrevState;
    NewPressed = ThisState & (~m_wPrevState);  // only those that have *changed* from OFF to ON, i.e. DOWN

    m_wPrevState = ThisState;
    State = ThisState;
    // Extra logic to implement chord with the touch screen, a btn can be locked down
    // Lock is one-shot, EXCEPT STOP locked and STRT pressed (single-step)
    int Btn;
    int HeldBtn;
    if (GetButtonDown(NewPressed, Btn))
    {
      // down
      graphics.DrawBtn(Btn, Graphics::eDown);
      State |= m_wHeld;
      if (GetButtonDown(m_wHeld, HeldBtn) && (Btn != eRunStart || HeldBtn != eRunStop))
      {
        graphics.DrawBtn(HeldBtn, Graphics::eUp);
        m_wHeld = 0;
      }
      m_iHeldTimeMS = millis();
    }
    else if (GetButtonDown(OldState, Btn))
    {
      // up
      if (m_wHeld != OldState)
      {
        graphics.DrawBtn(Btn, Graphics::eUp);
        if (GetButtonDown(m_wHeld, HeldBtn) && (Btn != eRunStart || HeldBtn != eRunStop))
        {
          graphics.DrawBtn(HeldBtn, Graphics::eUp);
          m_wHeld = 0;
        }
      }
    }
    return true;
  }
  else
  {
    if (m_wPrevState && !m_wHeld)
    {
      if ((millis() - m_iHeldTimeMS) > 1500L)
      {
        // btn held long enough to lock
        m_wHeld = m_wPrevState;
        int Btn;
        if (GetButtonDown(m_wHeld, Btn))
        {
          graphics.DrawBtn(Btn, Graphics::eFlash);
          graphics.DrawBtn(Btn, Graphics::eLocked);
        }
      }
    }
  }
  return false;
}
#endif

bool Buttons::IsPressed(word BtnState, int Btn)
{
  // is the Btn down (in the given state)
  return bitRead(BtnState, Btn);
}

bool Buttons::GetButtonDown(word BtnState, int& Btn)
{
  // return the first Btn down in the state
  for (Btn = eBit0; Btn <= eUnused; Btn++)
  {
    if (IsPressed(BtnState, Btn))
      return true;
  }
  return false;
}


Buttons buttons = Buttons();
