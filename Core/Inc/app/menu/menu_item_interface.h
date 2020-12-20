/*
 * menu_item_interface.h
 *
 *  Created on: 6 gru 2020
 *      Author: theoar
 */

#ifndef INC_APP_MENU_MENU_ITEM_INTERFACE_H_
#define INC_APP_MENU_MENU_ITEM_INTERFACE_H_

#include "app/segemnt_display/display_interface.h"
#include "app/daemon_ctrl/daemon_interface.h"

#include "peripherials/rtc/rtc.h"

#include "app/soft_timer/soft_timer.h"
#include "app/soft_timer/blink_timer.h"

#include <stdio.h>

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

  template<typename ProviderDataType, typename VarType>
  class MenuItemGenericReadWrite : public MenuItemInterface
  {
      typedef VarType (ProviderDataType::*GetterMethodType)(bool *);
      typedef void (ProviderDataType::*SetterMethodType)(VarType);
      typedef void (*VarTypeToStrFunction)(char*, VarType);
    private:
	ProviderDataType *provider;
	GetterMethodType getter;
	SetterMethodType setter;

	VarTypeToStrFunction toStrFnc = nullptr;

	VarType currentData;

	VarType min;
	VarType max;
	VarType step;

	BlinkTimer blinkTimer;

	bool dataPropet = false;

	bool isInEdit = false;

	char error[16];
	char prefix[8];
	char dataFormat[16];
	char sufix[8];
      public:
	MenuItemGenericReadWrite(const MenuItemGenericReadWrite &other)
	{
	  this->provider = other.provider;
	  this->getter = other.getter;
	  this->setter = other.setter;
	  this->currentData = other.currentData;

	  this->blinkTimer = other.blinkTimer;

	  this->dataPropet = other.dataPropet;

	  this->isInEdit = other.isInEdit;

	  memcpy(this->dataFormat, other.dataFormat, sizeof(this->dataFormat));
	  memcpy(this->prefix, other.prefix, sizeof(this->prefix));
	  memcpy(this->sufix, other.sufix, sizeof(this->sufix));
	}

	MenuItemGenericReadWrite()
	{
	  this->setStep(1);
	  this->dataFormat[0] = 0;
	  this->prefix[0] = 0;
	  this->sufix[0] = 0;
	  this->error[0] = 0;
	}

	MenuItemGenericReadWrite(ProviderDataType *provider, GetterMethodType getter, SetterMethodType setter) : MenuItemGenericReadWrite()
	{
	  this->setProvider(provider, getter, setter);
	}

	void setDataFormat(const char *format)
	{
	  strcpy(this->dataFormat, format);
	}

	void setDataFormat(VarTypeToStrFunction fnc)
	{
	  this->toStrFnc = fnc;
	}

	void setPrefix(const char *prefix)
	{
	  strcpy(this->prefix, prefix);
	}

	void setError(const char *error)
	{
	  strcpy(this->error, error);
	}

	void setSufix(const char * sufix)
	{
	  strcpy(this->sufix, sufix);
	}

	void setRange(VarType min, VarType max)
	{
	  this->min = min;
	  this->max = max;
	}

	void setStep(VarType step)
	{
	  this->step = step;
	}

	void setProvider(ProviderDataType *provider, GetterMethodType getter, SetterMethodType setter)
	{
	  this->provider = provider;
	  this->getter = getter;
	  this->setter = setter;
	  this->currentData = (this->provider->*this->getter)(&this->dataPropet);
	}

	void draw(SegmentDisplayInterface *display)
	{
	  char buff[64];
	  char format[64];

	  enum class DisplayE
	  {
	      Stable,
	      Blink,
	      Edit,
	      Error
	  } displayE = DisplayE::Error;

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
	    value = (this->provider->*this->getter)(&this->dataPropet);

	    if(this->dataPropet)
	      displayE = DisplayE::Stable;
	    else
	      displayE = DisplayE::Error;
	  }

	  switch (displayE)
	  {
	    case DisplayE::Blink:
	      display->display(this->prefix);
	    break;
	    case DisplayE::Error:
	      display->display(this->error);
	    break;

	    case DisplayE::Edit:
	      if(this->toStrFnc==nullptr)
	      {
		sprintf(format, "%s%s%s", this->prefix, this->dataFormat, this->sufix);
		sprintf(buff, format, value);
	      }
	      else
	      {
		this->toStrFnc(format, value);
		sprintf(buff, "%s%s%s", this->prefix, format, this->sufix);
	      }

	      display->display(buff);
	    break;

	    case DisplayE::Stable:
	      if(this->toStrFnc==nullptr)
	      {
		sprintf(format, "%s%s%s", this->prefix, this->dataFormat, this->sufix);
		sprintf(buff, format, value);
	      }
	      else
	      {
		this->toStrFnc(format, value);
		sprintf(buff, "%s%s%s", this->prefix, format, this->sufix);
	      }
	      display->display(buff);
	    break;
	  }

	}

	void setFocus()
	{
	  if(!this->isInEdit)
	  {
	    this->isInEdit = true;
	    this->currentData = (this->provider->*this->getter)(&this->dataPropet);
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
	    this->isInEdit = true;
	    this->blinkTimer.start(500,500);
	    this->currentData = (this->provider->*this->getter)(&this->dataPropet);
	  }
	  else
	  {
	    this->isInEdit = false;
	    (this->provider->*this->setter)(this->currentData);
	    this->blinkTimer.stop();
	  }
	}
    };

  template<typename ProviderDataType, typename VarType>
    class MenuItemGenericRead : public MenuItemInterface
    {
      typedef VarType ProviderDataType::*GetterMethodBoolType(bool *);
      typedef VarType ProviderDataType::*GetterMethodVoidType();
      private:
  	ProviderDataType *provider;
  	GetterMethodBoolType getterBool;
  	GetterMethodVoidType getterVoid;


  	char error[16];
  	char format[16];
        public:
  	MenuItemGenericRead()
        {
  	  this->format[0] = 0;
  	  this->error[0] = 0;
  	}

  	void setFormat(const char *format)
  	{
  	  strcpy(this->format, format);
  	}

  	void setError(const char *error)
  	{
  	  strcpy(this->error, error);
  	}

  	void setProviderFeedback(ProviderDataType *provider, GetterMethodBoolType getter)
  	{
  	  this->provider = provider;
  	  this->getterBool = getter;
  	  this->getterVoid = nullptr;
  	}

  	void setProvider(ProviderDataType *provider, GetterMethodVoidType getter)
  	{
  	  this->provider = provider;
  	  this->getterVoid = getter;
  	  this->getterBool = nullptr;
  	}


  	void draw(SegmentDisplayInterface *display)
  	{
  	  bool dataPropet = false;
  	  char buff[32];

  	  if(this->getterBool!=nullptr)
  	  {
	    sprintf(buff, this->format, (this->provider->*this->getterBool)(&dataPropet));
	    if(dataPropet)
	      display->display(buff);
	    else
	      display->display(this->error);
  	  }
  	  else
	  {
	    display->display(buff);
  	  }
  	}

  	void setFocus(){ return; }
  	void clearFocus(){}
  	bool focused() { return false; }
  	void actionUp() { return; }
  	void actionDown() { return; }
  	void actionEnter(){ return; }
      };

  template<typename ProviderDataType, typename VarType>
    class MenuItemFormattedRead : public MenuItemInterface
    {
      typedef VarType (ProviderDataType::*GetterMethodBoolType)(bool *);
      typedef VarType (ProviderDataType::*GetterMethodVoidType)();

      private:
  	ProviderDataType *provider;
  	GetterMethodBoolType getterBool = nullptr;
  	GetterMethodVoidType getterVoid = nullptr;

	typedef void (*PrintfFunctionType)(char*, VarType);
	PrintfFunctionType printfFunction = nullptr;

	char prefix[16];
	char sufix[16];

	char error[16];

        public:
	MenuItemFormattedRead()
        {
	  this->prefix[0] = 0;
	  this->sufix[0] = 0;
	  this->error[0] = 0;
        }

	void setPrefix(const char *prefix)
	{
	  strcpy(this->prefix, prefix);
	}

	void setSufix(const char *sufix)
	{
	  strcpy(this->sufix, sufix);
	}

	void setError(const char *error)
	{
	  strcpy(this->error, error);
	}

  	void setSprintfFunction(PrintfFunctionType printfFunction)
  	{
  	  this->printfFunction = printfFunction;
  	}

  	void setProviderFeedback(ProviderDataType *provider, GetterMethodBoolType getter)
  	{
  	  this->provider = provider;
  	  this->getterBool = getter;
  	  this->getterVoid = nullptr;
  	}

  	void setProvider(ProviderDataType *provider, GetterMethodVoidType getter)
  	{
  	  this->provider = provider;
  	  this->getterVoid = getter;
  	  this->getterBool = nullptr;
  	}

  	void draw(SegmentDisplayInterface *display)
  	{
  	  char buff[32];
  	  char format[32];

  	  bool result = false;

  	  if(this->getterBool!=nullptr)
  	  {
	    this->printfFunction(buff, (this->provider->*this->getterBool)(&result));

	    sprintf(format, "%s%s%s", this->prefix, buff, this->sufix);

	    if(result)
	      display->display(format);
	    else
	      display->display(this->error);
  	  }
  	  else
  	  {
	    this->printfFunction(buff, (this->provider->*this->getterVoid)());
	    sprintf(format, "%s%s%s", this->prefix, buff, this->sufix);
	    display->display(format);
	  }
  	}



  	void setFocus(){ return; }
  	void clearFocus(){}
  	bool focused() { return false; }
  	void actionUp() { return; }
  	void actionDown() { return; }
  	void actionEnter(){ return; }
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
