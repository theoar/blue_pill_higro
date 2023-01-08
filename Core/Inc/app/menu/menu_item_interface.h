/*
 * menu_item_interface.h
 *
 *  Created on: 6 gru 2020
 *      Author: theoar
 */

#ifndef INC_APP_MENU_MENU_ITEM_INTERFACE_H_
#define INC_APP_MENU_MENU_ITEM_INTERFACE_H_

#include <functional>

#include "app/segemnt_display/display_interface.h"
#include "app/daemon_ctrl/daemon_interface.h"

#include "peripherials/rtc/rtc.h"

#include "app/soft_timer/soft_timer.h"
#include "app/soft_timer/blink_timer.h"

//#include <stdio.h>
#include <cstring>
#include <limits>


using namespace segment_display;
using namespace daemon_ctrl;
using namespace rtc;

namespace menu
{
  class MenuItemInterface
  {
    public:
      virtual void draw(SegmentDisplayInterface *display) = 0;

      virtual void actionUp()  = 0;
      virtual void actionDown()  = 0;
      virtual void actionEnter()  = 0;

      virtual bool focused() = 0;

      virtual void setFocus() = 0;
      virtual void clearFocus() = 0;
  };

  template<typename VarType>
   class MenuItemGenericReadWriteFunctional : public MenuItemInterface
   {
     public:
       typedef VarType VarTypeE;

       using ValueGet = std::function<VarTypeE (bool *)>;
       using ValueChanged = std::function<void (VarTypeE)>;
       using ValueFormat = std::function<void (char *, VarTypeE)>;
     private:
        ValueGet valueGet;
        ValueChanged valueChagned;
        ValueFormat valueFormat;

 	VarType currentData;

 	VarType min;
 	VarType max;
 	VarType step;

 	BlinkTimer blinkTimer;

 	bool editable{true};
 	bool dataPropet{false};

 	bool isInEdit = false;

 	char error[16];
 	char prefix[8];
 	char sufix[8];

 	bool disablePrefixInEdit = true;
 	bool disableSufixInEdit = true;
 	bool autoPrefixSufixInEdit = false;
 	bool enableValueInNonEditMode = true;

 	void strncpy(char * dst, const char *src, unsigned size)
 	{
 	  if(src!=nullptr)
 	    std::strncpy(dst, src, size);
 	  else
 	    memset(dst, 0, size);
 	}


       public:
 	MenuItemGenericReadWriteFunctional(const MenuItemGenericReadWriteFunctional &other)
 	{
 	  this->valueFormat = other.valueFormat;
 	  this->valueChagned = other.valueChagned;
 	  this->currentData = other.currentData;

 	  this->blinkTimer = other.blinkTimer;

 	  this->isInEdit = false;

 	  memcpy(this->prefix, other.prefix, sizeof(this->prefix));
 	  memcpy(this->sufix, other.sufix, sizeof(this->sufix));
 	  memcpy(this->error, other.error, sizeof(this->error));
 	}

 	MenuItemGenericReadWriteFunctional()
 	{
 	  this->setStep(1);
 	  this->prefix[0] = 0;
 	  this->sufix[0] = 0;
 	  this->error[0] = 0;

 	  this->setRange(std::numeric_limits<VarTypeE>::min(), std::numeric_limits<VarTypeE>::max());
 	}

 	MenuItemGenericReadWriteFunctional(ValueChanged valueChanged, ValueGet valueGet, ValueFormat valueFormat) : MenuItemGenericReadWriteFunctional()
 	{
 	  this->valueGet = valueGet;
 	  this->valueChagned = valueChanged;
 	  this->valueFormat = valueFormat;
 	}

 	template<typename T>
 	auto & setFormatValue(T fnc)
 	{
 	  this->valueFormat = fnc;
 	  return *this;
 	}

 	template<typename T>
 	auto & setValueChanged(T valueChanged)
 	{
 	  this->valueChagned = valueChanged;
	  return *this;
 	}

 	template<typename T>
 	auto & setValueGet(T valueGet)
 	{
 	  this->valueGet = valueGet;
	  return *this;
 	}

 	auto & setPrefix(const char *prefix)
 	{
 	  this->strncpy(this->prefix, prefix, sizeof(this->prefix));
 	  return *this;
 	}

 	auto & setError(const char *error)
 	{
 	  this->strncpy(this->error, error, sizeof(this->error));
 	  return *this;
 	}

 	auto & setSufix(const char * sufix)
 	{
 	  this->strncpy(this->sufix, sufix, sizeof(this->sufix));
 	  return *this;
 	}

 	auto & setRange(VarType min, VarType max)
 	{
 	  this->min = min;
 	  this->max = max;
 	  return *this;
 	}

 	auto & setStep(VarType step)
 	{
 	  this->step = step;
 	  return *this;
 	}

 	auto & setPrefixVisibleDuringEdit(bool state)
 	{
 	  this->disablePrefixInEdit = !state;
 	  return *this;
 	}

 	auto & setSufixVisibleDuringEdit(bool state)
 	{
 	  this->disableSufixInEdit = !state;
 	  return *this;
 	}

 	auto & setValueVisableDuringNonEditMode(bool state)
 	{
 	  this->enableValueInNonEditMode = state;
 	  return *this;
 	}

 	auto & setAutoSufixPrefixVisibility(bool state)
 	{
 	  this->autoPrefixSufixInEdit = state;
 	  return *this;
 	}

 	auto & setEditable(bool allowEdit)
 	{
 	  this->editable = allowEdit;
 	  return *this;
 	}

 	void draw(SegmentDisplayInterface *display)
 	{
 	  enum class DisplayE
 	  {
 	      Stable,
 	      Blink,
 	      Edit,
 	      Error
 	  } displayE = DisplayE::Error;


 	  char buff[64] = {0};
 	  char format[64] = {0};


 	  VarType value;

 	  if(this->isInEdit)
 	  {
 	    if(this->blinkTimer.checkCurrentAndRestart() == 1)
 	      displayE = DisplayE::Blink;
 	    else
 	      displayE = DisplayE::Edit;

 	    value = this->currentData;
 	  }
 	  else
 	  {
 	    this->dataPropet = true;
 	    value = this->valueGet(&this->dataPropet);

 	    if(this->dataPropet)
 	      displayE = DisplayE::Stable;
 	    else
 	      displayE = DisplayE::Error;
 	  }

 	  switch (displayE)
 	  {
 	    case DisplayE::Error:
 	    {
 	      display->display(this->error);
 	    }
 	    break;

 	    case DisplayE::Stable:
 	    {
 	      if(this->enableValueInNonEditMode)
 	      {
 		this->valueFormat(format, value);
 		sprintf(buff, "%s%s%s", this->prefix, format, this->sufix);
 	      }
 	      else
 	      {
 		sprintf(buff, "%s", this->prefix);
 	      }

 	      display->display(buff);
 	    }
 	    break;

 	    case DisplayE::Blink:
 	    case DisplayE::Edit:
 	    {
 	      char emptyString = '\0';
 	      const char *prefix = this->prefix;
 	      const char *sufix = this->sufix;

 	      if(this->autoPrefixSufixInEdit)
 	      {
 		auto lenPrefix = display->strlen(prefix);
 		auto lenSufix = display->strlen(sufix);
 		auto digitsCount = display->digitsCount();

 		if((lenPrefix+lenSufix) >= digitsCount )
 		{
 		  prefix = &emptyString;
 		  sufix = &emptyString;
 		}
 	      }
 	      else
 	      {
 		if(this->disablePrefixInEdit)
 		  prefix = &emptyString;

 		if(this->disableSufixInEdit)
 		  sufix = &emptyString;
 	      }

 	      this->valueFormat(format, value);
 	      auto len = display->strlen(format);

 	      if(displayE==DisplayE::Blink)
 	      {
		for(unsigned x = 0; x<len; x++)
		  format[x] = ' ';
 	      }

 	      sprintf(buff, "%s%s%s", prefix, format, sufix);

 	      display->display(buff);

 	    }
 	    break;
 	  }

 	}

 	void setFocus()
 	{
 	  if(!this->isInEdit)
 	  {
 	    if(this->editable)
 	    {
 	      this->isInEdit = true;
 	      this->blinkTimer.start(500,500);
 	      this->dataPropet = true;
 	      this->currentData = this->valueGet(&this->dataPropet);
 	    }
 	  }
 	}

 	void clearFocus()
 	{
 	  if(this->isInEdit)
 	  {
 	    this->isInEdit = false;
 	    //(this->provider->*this->setter)(this->currentData);
 	    this->blinkTimer.stop();
 	  }
 	}

 	bool focused()
 	{
 	  return this->isInEdit;
 	}

 	void actionUp()
 	{
 	  if(this->isInEdit)
 	  {
 	    if(this->currentData+this->step>this->max)
 	      this->currentData=this->min;
 	    else
 	      this->currentData+=this->step;
 	  }
 	}

 	void actionDown()
 	{
 	  if(this->isInEdit)
 	  {
 	    if(this->currentData - this->step < this->min)
 	      this->currentData = this->max;
 	    else
 	      this->currentData -= this->step;
 	  }
 	}

 	void actionEnter()
 	{
 	  if(!this->isInEdit)
 	  {
 	    if(this->editable)
 	    {
 	      this->isInEdit = true;
 	      this->blinkTimer.start(500,500);
 	      this->dataPropet = true;
 	      this->currentData = this->valueGet(&this->dataPropet);
 	    }
 	  }
 	  else
 	  {
 	    this->isInEdit = false;
 	    this->valueChagned(this->currentData);
 	    this->blinkTimer.stop();
 	  }
 	}
     };

  class MenuItemTime: public MenuItemInterface
  {
    private:
      TimeStruct editTime;
      enum class EditPos
      {
	  Hours,
	  Minutes,
	  NotInEdit
      } editPos = EditPos::NotInEdit;

      BlinkTimer blinkTimer;
    public:
      virtual void draw(SegmentDisplayInterface *display)
      {
	char buff[32];
	bool drawBuff = false;

	switch(editPos)
	{
	  case EditPos::NotInEdit:
	  {
	    TimeStruct currentTime = Rtc::time();
	    if(currentTime.Second%2 == 0)
	      sprintf(buff, "%02d.%02d", currentTime.Hour, currentTime.Minute);
	    else
	      sprintf(buff, "%02d%02d", currentTime.Hour, currentTime.Minute);

	    drawBuff = true;
	  }
	  break;

	  case EditPos::Minutes:
	  {
	    if(this->blinkTimer.checkCurrentAndRestart()==0)
	      sprintf(buff, "%02d  ", this->editTime.Hour);
	    else
	      sprintf(buff, "%02d.%02d", this->editTime.Hour, this->editTime.Minute);

	    drawBuff = true;
	  }
	  break;

	  case EditPos::Hours:
	  {
	    if(this->blinkTimer.checkCurrentAndRestart()==0)
	      sprintf(buff, "  %02d", this->editTime.Minute);
	    else
	      sprintf(buff, "%02d.%02d", this->editTime.Hour, this->editTime.Minute);

	    drawBuff = true;
	  }
	  break;

	  default:;
	}

	if(drawBuff)
	  display->display(buff);
      }

      virtual void actionUp()
      {
	switch(this->editPos)
	{
	  case EditPos::Hours:
	    this->editTime.Hour++;
	    this->editTime.Hour %= 24;
	  break;
	  case EditPos::Minutes:
	    this->editTime.Minute++;
	    this->editTime.Minute %= 60;
	  break;
	  case EditPos::NotInEdit:
	  break;
	}
      }

      virtual void actionDown()
      {
	switch(this->editPos)
	{
	  case EditPos::Hours:
	    if(this->editTime.Hour==0)
	      this->editTime.Hour = 23;
	    else
	      this->editTime.Hour--;
	  break;
	  case EditPos::Minutes:
	    if(this->editTime.Minute==0)
	      this->editTime.Minute = 59;
	    else
	      this->editTime.Minute--;
	  break;
	  case EditPos::NotInEdit:
	  break;
	}
      }

      virtual void actionEnter()
      {
	switch(this->editPos)
	{
	  case EditPos::Hours:
	    this->editPos = EditPos::Minutes;
	  break;
	  case EditPos::Minutes:
	    this->editPos = EditPos::NotInEdit;
	    Rtc::setTime(this->editTime);
	    this->blinkTimer.stop();
	  break;
	  case EditPos::NotInEdit:
	    this->editPos = EditPos::Hours;
	    this->blinkTimer.start(500, 500);
	    this->editTime = Rtc::time();
	  break;
	}
      }

      virtual bool focused()
      {
	return this->editPos!=EditPos::NotInEdit;
      }


      virtual void setFocus()
      {
	this->editPos = EditPos::Hours;
	this->blinkTimer.start(500, 500);
	this->editTime = Rtc::time();
      }

      virtual void clearFocus()
      {
	this->editPos = EditPos::NotInEdit;
	this->blinkTimer.stop();
      }
  };
}



#endif /* INC_APP_MENU_MENU_ITEM_INTERFACE_H_ */
