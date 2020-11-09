/*
 * rotary_dip_switch.h
 *
 *  Created on: Nov 8, 2020
 *      Author: theoar
 */

#ifndef SRC_APP_ROTARY_DIP_SWITCH_ROTARY_DIP_SWITCH_H_
#define SRC_APP_ROTARY_DIP_SWITCH_ROTARY_DIP_SWITCH_H_

#include "peripherials/gpio/gpio_pin.h"
#include "app/soft_timer/soft_timer.h"
#include "app/daemon_ctrl/daemon_interface.h"

using namespace gpio;
using namespace daemon_ctrl;

namespace rotary_dip_switch
{
  static constexpr int myPow(int x, int no, int val)
  {
    if(val == 1)
      return 1;

    if(x >= val)
      return no - 1;
    else
      return myPow(x * 2, ++no, val);
  }

  static const constexpr int myPow(int val)
  {
    if(val == 1)
      return 1;
    else
      return myPow(1, 1, val);
  }

  template<int pos>
    class RotaryDipSwitch : public IDaemon
    {
      private:
	const int pinNo = myPow(pos);
	uint8_t currentNumber = 0;
	uint8_t tempNumber = 0;
	uint32_t stabilizationMs = 500;
	bool hasChanged = true;
	SoftTimer timer;
	Pin *pins[myPow(pos)];

      public:
	RotaryDipSwitch()
	{
	  this->timer.stop();
	}

	void pinAt(int index, Pin *pin)
	{
	  if(index<this->pinNo)
	    this->pins[index] = pin;
	}

	uint8_t number()
	{
	  uint8_t num = 0;
	  for(uint8_t x = 0; x<this->pinNo; ++x)
	    num |= !this->pins[x]->state() ? (1<<x) : (0);

	  return num;
	}

	uint8_t stableNumber()
	{
	  return this->currentNumber;
	}

	virtual void handler(void)
	{
	  uint8_t num = this->number();

	  if(num!=this->tempNumber)
	  {
	    this->tempNumber = num;
	    this->timer.start(this->stabilizationMs);
	  }
	  else
	  {
	    if(this->timer.check())
	    {
	      if(this->currentNumber!=this->tempNumber)
		this->hasChanged = true;

	      this->currentNumber = this->tempNumber;
	    }
	  }
	}

	bool changed()
	{
	  bool ret = this->hasChanged;
	  this->hasChanged = false;
	  return ret;
	}
    };
}



#endif /* SRC_APP_ROTARY_DIP_SWITCH_ROTARY_DIP_SWITCH_H_ */
