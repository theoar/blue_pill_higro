/*
 * digit.h
 *
 *  Created on: 2 lis 2020
 *      Author: theoar
 */

#ifndef INC_APP_SEGEMNT_DISPLAY_DIGIT_H_
#define INC_APP_SEGEMNT_DISPLAY_DIGIT_H_

#include <app/segemnt_display/digit_display_defs.h>
#include "peripherials/gpio/gpio_pin.h"


using namespace gpio;

namespace segment_display
{
  class BasicDigit
  {
    protected:
      Pin pinConfig[DigitDisplayDefs::segmentCount()];
      bool commonAnode = true;
      bool dotConfigured = false;

    public:
      void configure(DigitDisplayDefs::Segments segment, Pin pin)
      {
	this->pinConfig[DigitDisplayDefs::segmentIndex(segment)] = pin;
      }

      void configure(DigitDisplayDefs::Segments segment, uint32_t pin, GpioTypes::GpioNr gpio)
      {
	if(this->commonAnode)
	{
	  Pin pinObj { gpio, pin, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz };
	  this->configure(segment, pinObj);

	}
	else
	{
	  Pin pinObj { gpio, pin, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz };
	  this->configure(segment, pinObj);
	}
      }

      virtual void clear()
      {
	for(uint8_t i = 0; i < DigitDisplayDefs::segmentCount(); ++i)
	{
	  if(this->commonAnode)
	    this->pinConfig[i].set();
	  else
	    this->pinConfig[i].reset();
	}

	if(this->dotConfigured)
	{
	  if(this->commonAnode)
	    this->pinConfig[DigitDisplayDefs::segmentIndex(DigitDisplayDefs::Segments::DOTP)].set();
	  else
	    this->pinConfig[DigitDisplayDefs::segmentIndex(DigitDisplayDefs::Segments::DOTP)].reset();
	}
      }

      virtual void display(uint8_t digit)
      {
	uint8_t mask = DigitDisplayDefs::digit2SegmentMask(digit);

	if(this->commonAnode)
	  mask = ~mask;

	for(uint8_t i = 0; i < DigitDisplayDefs::segmentCount(); ++i)
	{
	  if(mask & (1 << i))
	    this->pinConfig[i].set();
	  else
	    this->pinConfig[i].reset();
	}
      }

      virtual void display(char digit)
      {
	uint8_t mask = DigitDisplayDefs::char2SegmentMask(digit);

	if(this->commonAnode)
	  mask = ~mask;

	for(uint8_t i = 0; i < DigitDisplayDefs::segmentCount(); ++i)
	{
	  if(mask & (1 << i))
	    this->pinConfig[i].set();
	  else
	    this->pinConfig[i].reset();
	}
      }

      virtual void display(uint8_t digit, bool dot)
      {
	this->display(digit);

	if(this->dotConfigured)
	{
	  if(dot)
	  {
	    if(this->commonAnode)
	      this->pinConfig[DigitDisplayDefs::segmentIndex(DigitDisplayDefs::Segments::DOTP)].reset();
	    else
	      this->pinConfig[DigitDisplayDefs::segmentIndex(DigitDisplayDefs::Segments::DOTP)].set();
	  }
	  else
	  {
	    if(this->commonAnode)
	      this->pinConfig[DigitDisplayDefs::segmentIndex(DigitDisplayDefs::Segments::DOTP)].set();
	    else
	      this->pinConfig[DigitDisplayDefs::segmentIndex(DigitDisplayDefs::Segments::DOTP)].reset();
	  }
	}
      }

      void setCommonAnode()
      {
	this->commonAnode = true;
      }

      void setCommonCathode()
      {
	this->commonAnode = false;
      }

      void segmentAt(DigitDisplayDefs::Segments segment, bool state)
      {
	if(state)
	{
	  if(this->commonAnode)
	    this->pinConfig[DigitDisplayDefs::segmentIndex(segment)].reset();
	  else
	    this->pinConfig[DigitDisplayDefs::segmentIndex(segment)].set();
	}
	else
	{
	  if(this->commonAnode)
	    this->pinConfig[DigitDisplayDefs::segmentIndex(segment)].set();
	  else
	    this->pinConfig[DigitDisplayDefs::segmentIndex(segment)].reset();
	}
      }
  };

}

#endif /* INC_APP_SEGEMNT_DISPLAY_DIGIT_H_ */
