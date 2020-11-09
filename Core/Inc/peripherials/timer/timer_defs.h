/*
 * timer_defs.h
 *
 *  Created on: 31 paź 2020
 *      Author: theoar
 */

#ifndef INC_PERIPHERIALS_TIMER_TIMER_DEFS_H_
#define INC_PERIPHERIALS_TIMER_TIMER_DEFS_H_

#include "proc.h"

namespace timer
{

  class TimerDefs
  {
    public:
      enum class TimerNr
      {
	Tim1, Tim2, Tim3, Tim4
      };

      enum class TimerType
      {
	Advanced, GeneralPuprose
      };

      static TimerType getTimerType(TimerNr nr)
      {
	switch(nr)
	{
	  case TimerNr::Tim1: return TimerType::Advanced; break;
	  case TimerNr::Tim2: return TimerType::GeneralPuprose; break;
	  case TimerNr::Tim3: return TimerType::GeneralPuprose; break;
	  case TimerNr::Tim4: return TimerType::GeneralPuprose; break;
	}
      }

      static TIM_TypeDef * getTimer(TimerNr nr)
      {
	switch(nr)
	{
	  case TimerNr::Tim1: return TIM1;
	  case TimerNr::Tim2: return TIM2;
	  case TimerNr::Tim3: return TIM3;
	  case TimerNr::Tim4: return TIM4;
	  default: return nullptr;
	}
      }

  };
}



#endif /* INC_PERIPHERIALS_TIMER_TIMER_DEFS_H_ */
