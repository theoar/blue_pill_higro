/*
 * multiplex_digit_display.h
 *
 *  Created on: 23 lis 2020
 *      Author: theoar
 */

#ifndef INC_APP_SEGEMNT_DISPLAY_MULTIPLEX_DIGIT_DISPLAY_H_
#define INC_APP_SEGEMNT_DISPLAY_MULTIPLEX_DIGIT_DISPLAY_H_

#include <string.h>
#include <stdio.h>

#include "digit.h"

#include "display_interface.h"

#include "app/daemon_ctrl/daemon_interface.h"
#include "app/soft_timer/soft_timer.h"
#include "peripherials/gpio/gpio_pin.h"

using namespace daemon_ctrl;
using namespace gpio;

namespace segment_display
{
  template<uint8_t count>
    class MultiplexDigitDisplay : public IDaemon, public SegmentDisplayInterface
    {
      private:
	SoftTimer timer;
	uint32_t switchTime = 2;

	BasicDigit *digit = nullptr;
	Pin *powerPin[count];

	char text[count];
	bool dot[count];

	uint8_t currentPos = 0;

	bool commonAnode = true;

      public:
	MultiplexDigitDisplay()
	{
	  for (uint8_t x = 0; x < count; ++x)
	    this->powerPin[x] = nullptr;

	  this->timer.start(this->switchTime);
	}

	bool setDigitConfig(BasicDigit *digit)
	{
	  if(digit != nullptr)
	  {
	    this->digit = digit;
	    return true;
	  }

	  return false;
	}

	bool setPowerPinAt(uint8_t pos, Pin *powerPin)
	{
	  if(pos < count)
	  {
	    if(powerPin != nullptr)
	    {
	      this->powerPin[pos] = powerPin;
	      return true;
	    }
	  }
	  return false;
	}

	void display(uint32_t number) override
	{
	  for (uint32_t x = 0; x < count; ++x)
	  {
	    this->text[x] = (static_cast<char>(number % 10)) + '0';
	    number /= 10;
	  }
	}

	void display(const char *str) override
	{
	  uint8_t index = count;

	  while (*str != 0 && index != 0)
	  {
	    if(*str != '.')
	    {
	      this->dot[index - 1] = *(str + 1) == '.';
	      this->text[index - 1] = *str;
	      index--;
	    }
	    str++;
	  }

	  while (index != 0)
	  {
	    this->text[index - 1] = ' ';
	    this->dot[index - 1] = 0;
	    index--;
	  }
	}

	void clear() override
	{
	  this->digit->clear();
	}

	virtual void handler(void)
	{
	  if(this->timer.checkAndRestart())
	  {
	    this->powerPin[this->currentPos]->reset();

	    this->currentPos++;
	    this->currentPos %= count;

	    this->digit->display(this->text[this->currentPos]);
	    this->digit->dot(this->dot[this->currentPos]);

	    this->powerPin[this->currentPos]->set();
	  }
	}
    };
  template<uint8_t count>
      class MultiplexScrollDigitDisplay : public IDaemon, public SegmentDisplayInterface
      {
        private:
  	SoftTimer timer;
  	uint32_t switchTime = 2;

  	BasicDigit *digit = nullptr;
  	Pin *powerPin[count];

  	static constexpr char DotMask = 1<<7;

  	static constexpr uint32_t TextSize = 32;
  	char rawText[32];
  	char text[TextSize];

  	uint32_t offset = 0;
  	uint32_t curentSize = 0;

  	uint8_t currentPos = 0;

  	SoftTimer scrollTimer;
  	uint32_t scrollTime = 300;

  	bool commonAnode = true;

        public:
  	MultiplexScrollDigitDisplay()
  	{
  	  for (uint8_t x = 0; x < count; ++x)
  	    this->powerPin[x] = nullptr;

  	  this->timer.start(this->switchTime);

  	  memset(this->rawText, 0, sizeof(this->rawText));
  	  memset(this->text, 0, sizeof(this->text));
  	}

  	bool setDigitConfig(BasicDigit *digit)
  	{
  	  if(digit != nullptr)
  	  {
  	    this->digit = digit;
  	    return true;
  	  }

  	  return false;
  	}

  	bool setPowerPinAt(uint8_t pos, Pin *powerPin)
  	{
  	  if(pos < count)
  	  {
  	    if(powerPin != nullptr)
  	    {
  	      this->powerPin[pos] = powerPin;
  	      return true;
  	    }
  	  }
  	  return false;
  	}

  	void display(uint32_t number) override
  	{
  	  sprintf(this->text, "%lu", number);
  	}

  	void display(const char *str) override
  	{
	  uint8_t index = 0;

	  if(strcmp(this->rawText, str) != 0)
	  {
	    strcpy(this->rawText, str);

	    while (*str != 0 && index != TextSize)
	    {
	      if(*str != '.')
	      {
		this->text[index] = *str;
		if(*(str + 1) == '.')
		  this->text[index] |= DotMask;
		else
		  this->text[index] &= ~DotMask;

		index++;
	      }
	      str++;
	    }

	    if(index>count)
	    {
	      for(uint32_t x = 0; x<count-1; ++x)
		this->text[index++] = ' ';

	      this->scrollTimer.start(this->scrollTime);
	    }
	    else
	    {
	      while (index != count)
		this->text[index++] = ' ';

	      this->scrollTimer.stop();
	    }

	    //this->currentPos = 0;
	    this->offset = 0;
	    this->curentSize = index;
	  }
  	}

  	void clear() override
  	{
  	  memset(this->text, ' ', TextSize);
  	  memset(this->rawText, ' ', TextSize);
  	  this->curentSize = count;
  	}

  	unsigned digitsCount() const override
  	{
  	  return count;
  	}

  	unsigned strlen(const char *str) const override
  	{
  	  unsigned len{0};
  	  while(*str!='\0')
  	  {
  	    if(*str!='.')
  	      len++;

  	    str++;
  	  }

  	  return len;
  	}

  	virtual void handler(void)
  	{
  	  if(this->scrollTimer.checkAndRestart())
  	  {
  	    this->offset++;
  	    if(this->offset>=this->curentSize)
  	      this->offset = 0;
  	  }

  	  if(this->timer.checkAndRestart())
  	  {
	    this->powerPin[this->currentPos]->reset();

	    this->currentPos++;
	    this->currentPos %= count;

	    char c;
	    c = this->text[(this->currentPos + this->offset) % this->curentSize];
	    this->digit->display((char)(c & ~DotMask));
	    this->digit->dot((c & DotMask));

  	    this->powerPin[this->currentPos]->set();
  	  }
  	}
      };
}

#endif /* INC_APP_SEGEMNT_DISPLAY_MULTIPLEX_DIGIT_DISPLAY_H_ */
