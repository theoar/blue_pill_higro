/*
 * timer_events.h
 *
 *  Created on: 30 paź 2020
 *      Author: theoar
 */

#ifndef INC_PERIPHERIALS_TIMER_TIMER_EVENTS_H_
#define INC_PERIPHERIALS_TIMER_TIMER_EVENTS_H_

#include <inttypes.h>

#include "timer_defs.h"

namespace timer
{

  class SysTickEvent
  {
    private:
      void (*sysTickevent)(void) = nullptr;
    public:
      void setEvent( void (*sysTickevent)(void))
      {
	this->sysTickevent = sysTickevent;
      }

      void callEvent()
      {
	if(sysTickevent!=nullptr)
	  this->sysTickevent();
      }
  };

  class IGeneralEvent
  {
    public:
      virtual void tickEvent(uint32_t status) = 0;
  };

  class ICompareChannelEvent
  {
    public:
      virtual void compareEvent(uint32_t status) = 0;
  };

  class IInputCompareEvent
  {
    public:
      virtual void captureEvent(uint32_t status) = 0;
  };

  class GeneralEvent
  {
    protected:
      IGeneralEvent *tickEvent = nullptr;
    public:
      void writeTickEvent(IGeneralEvent * tickEvent)
      {
	this->tickEvent = tickEvent;
      }
      void callTickEvent(uint32_t status)
      {
	if (this->tickEvent != nullptr)
	  this->tickEvent->tickEvent(status);
      }
  };

  class CompareChannelEvent
  {
    protected:
      ICompareChannelEvent *compareEvent = nullptr;
    public:
      void writeCompareEvent(ICompareChannelEvent *compareEvent)
      {
	this->compareEvent = compareEvent;
      }
      void callCompareEvent(uint32_t status)
      {
	if (this->compareEvent != nullptr)
	  this->compareEvent->compareEvent(status);
      }
  };

  class InputCaptureChannelEvent
  {
    protected:
      IInputCompareEvent *captureEvent = nullptr;
    public:
      void writeCaptureEvent(IInputCompareEvent *captureEvent)
      {
	this->captureEvent = captureEvent;
      }

      void callCaptureEvent(uint32_t status)
      {
	if (this->captureEvent != nullptr)
	  this->captureEvent->captureEvent (status);

      }
  };

  class GeneralPurposeTimerEvents : public GeneralEvent,
      public CompareChannelEvent,
      public InputCaptureChannelEvent
  {

  };


  class TimerEvents
  {
    public:
      static GeneralPurposeTimerEvents * getGeneralPurposeTimerEvents(TimerDefs::TimerNr nr);
      static SysTickEvent *getSysTickEvent();
  };

}

#endif /* INC_PERIPHERIALS_TIMER_TIMER_EVENTS_H_ */
