/*
 * digit_display.h
 *
 *  Created on: 3 lis 2020
 *      Author: theoar
 */

#ifndef INC_APP_SEGEMNT_DISPLAY_DIGIT_DISPLAY_H_
#define INC_APP_SEGEMNT_DISPLAY_DIGIT_DISPLAY_H_

#include <inttypes.h>

#include "digit.h"
#include "digit_display_defs.h"

namespace segment_display
{
  template<int count>
    class BasicDigitDisplay
    {
      private:
	BasicDigit *digits[count];

      public:
	BasicDigitDisplay()
	{
	  for(uint8_t x = 0; x < count; ++x)
	    this->digits[x] = nullptr;
	}

	bool setDigitAt(uint8_t pos, BasicDigit *digit)
	{
	  if(pos < count)
	  {
	    this->digits[pos] = digit;
	    if(digit!=nullptr)
	      digit->clear();
	    return true;
	  }
	  else
	  {
	    return false;
	  }
	}

	void clear()
	{
	  for(uint8_t x = 0; x < count; ++x)
	    if(this->digits[x] != nullptr)
	      this->digits[x]->clear();
	}

	void display(uint32_t number)
	{
	  for(uint32_t x = 0; x < count; ++x)
	  {
	    if(this->digits[x] != nullptr)
	      this->digits[x]->display(number % 10, false);
	    number /= 10;
	  }
	}

	void display(uint32_t number, uint8_t dotIndex)
	{
	  for(uint32_t x = 0; x < count; ++x)
	  {
	    if(this->digits[x] != nullptr)
	    {
	      if(x == dotIndex)
		this->digits[x]->display(number % 10, true);
	      else
		this->digits[x]->display(number % 10, false);
	      number /= 10;
	    }
	  }
	}

	void display(const char *str)
	{
	  for(uint32_t x = count; (x > 0) && (*str!=0); --x)
	  {
	    if(this->digits[x-1] != nullptr)
	    {
	      this->digits[x-1]->display(*str);
	      str++;
	    }
	  }
	}


    };
}

#endif /* INC_APP_SEGEMNT_DISPLAY_DIGIT_DISPLAY_H_ */
