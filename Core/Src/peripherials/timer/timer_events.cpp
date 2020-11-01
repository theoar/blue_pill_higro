/*
 * timer_events.cpp
 *
 *  Created on: 31 paź 2020
 *      Author: theoar
 */

#include "peripherials/timer/timer_events.h"

#include "proc.h"


namespace timer
{

  SysTickEvent sysTickEvents;
  GeneralPurposeTimerEvents timer2Events;
  GeneralPurposeTimerEvents timer3Events;
  GeneralPurposeTimerEvents timer4Events;

  GeneralPurposeTimerEvents *TimerEvents::getGeneralPurposeTimerEvents(TimerDefs::TimerNr nr)
  {
    switch(nr)
    {
      case TimerDefs::TimerNr::Tim2: return &timer2Events;
      case TimerDefs::TimerNr::Tim3: return &timer3Events;
      case TimerDefs::TimerNr::Tim4: return &timer4Events;
      default: return nullptr;
    }
  }

  SysTickEvent *TimerEvents::getSysTickEvent()
  {
    return &sysTickEvents;
  }

  extern "C" void TIM2_IRQHandler()
  {
    uint32_t status = TIM2->SR;
    TIM2->SR = 0;

    if(status & (TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF))
    {
      timer2Events.callCompareEvent(status);
      timer2Events.callCaptureEvent(status);
    }

    if(status & TIM_SR_UIF)
      timer2Events.callTickEvent(status);

    NVIC_ClearPendingIRQ(TIM2_IRQn);
  }

  extern "C" void TIM3_IRQHandler()
  {
    uint32_t status = TIM3->SR;
    TIM3->SR = 0;

    if(status & (TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF))
    {
      timer3Events.callCompareEvent(status);
      timer3Events.callCaptureEvent(status);
    }

    if(status & TIM_SR_UIF)
      timer3Events.callTickEvent(status);

    NVIC_ClearPendingIRQ(TIM3_IRQn);
  }

  extern "C" void TIM4_IRQHandler()
  {
    uint32_t status = TIM4->SR;
    TIM4->SR = 0;

    if(status & (TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF))
    {
      timer4Events.callCompareEvent(status);
      timer4Events.callCaptureEvent(status);
    }

    if(status & TIM_SR_UIF)
      timer4Events.callTickEvent(status);

    NVIC_ClearPendingIRQ(TIM4_IRQn);
  }

  extern "C" void SysTick_Handler()
  {
    sysTickEvents.callEvent();
    NVIC_ClearPendingIRQ(SysTick_IRQn);
  }
}

