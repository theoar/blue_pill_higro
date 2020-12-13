/*
 * button.cpp
 *
 *  Created on: 30 lis 2020
 *      Author: theoar
 */

#include "app/button/button.h"

namespace button
{
  void Button::configure(Pin *pin, bool cklickedState)
  {
    this->pin = pin;
    this->machineSt = MachineSt::Idle;
    this->clicked = false;
    this->holded = false;
    this->clickedState = cklickedState;
    this->timer.stop();
  }

  void Button::handler()
  {
    switch(this->machineSt)
    {
      case MachineSt::Idle:
      {
	if(this->pin->state()==this->clickedState)
	{
	  this->timer.start(this->DebounceMs);
	  this->machineSt = MachineSt::Debounce;
	}
      }
      break;

      case MachineSt::Debounce:
      {
	if(this->timer.check())
	{
	  if(this->pin->state()==this->clickedState)
	  {
	    this->clicked = true;
	    this->repateMs = Button::RepateMs;
	    this->timer.start(this->repateMs);
	    this->machineSt = MachineSt::Clicked;

	  }
	  else
	  {
	    this->timer.stop();
	    this->machineSt = MachineSt::Idle;
	  }
	}
      }
      break;

      case MachineSt::Clicked:
      {
	if(this->pin->state()!=this->clickedState)
	{
	  this->machineSt = MachineSt::Idle;
	  this->holded = false;
	  this->released = true;
	  this->timer.stop();
	}
	else
	{
	  if(this->timer.check())
	  {
	    this->holded = true;
	    this->clicked = true;
	    this->repeated = true;

	    this->timer.start(this->repateMs);
	    this->repateMs-=Button::RepateTickMs;

	    if(this->repateMs==0)
	      this->repateMs = Button::RepateTickMs;
	  }
	}
      }
      break;
      default:;
    }
  }

  bool Button::isClicked()
  {
    bool ret = this->clicked;
    this->clicked = false;
    return ret;
  }

  bool Button::isHolded()
  {
    return this->holded;
  }

  bool Button::isReleased()
  {
    bool ret = this->released;
    this->released = false;
    return ret;
  }

  bool Button::isRepeated()
  {
    bool ret = this->repeated;
    this->repeated = false;
    return ret;
  }
}

