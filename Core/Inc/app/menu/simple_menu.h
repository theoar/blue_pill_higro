/*
 * simple_menu.h
 *
 *  Created on: 13 gru 2020
 *      Author: theoar
 */

#ifndef INC_APP_MENU_SIMPLE_MENU_H_
#define INC_APP_MENU_SIMPLE_MENU_H_

#include "menu_item_interface.h"

#include "app/soft_timer/soft_timer.h"
#include "app/daemon_ctrl/daemon_interface.h"
#include "app/button/button_keyboard.h"
#include "app/segemnt_display/multiplex_digit_display.h"

#include <inttypes.h>

using namespace daemon_ctrl;
using namespace button;
using namespace segment_display;

namespace menu
{
  template<uint32_t size>
    class SimpleMenu : public IDaemon
    {
      private:
	ButtonKeyboardInterface *keyboard = nullptr;
	SegmentDisplayInterface *display = nullptr;
	MenuItemInterface *items[size];
	bool inScrollMenu[size];

	uint32_t itemsCount{0};

	bool atLeasOneInScroll = false;

	SoftTimer idleTimer;
	SoftTimer switchTimer;
	static constexpr uint32_t IdleTimeout = 10000;
	static constexpr uint32_t SwitchTime = 5000;

	uint32_t index = 0;

	enum class MachineSt
	{
	  EditMode, DisplayMode
	} machineSt = MachineSt::DisplayMode, previousMachineSt = MachineSt::DisplayMode;

	void setMachineState(MachineSt newState)
	{
	  this->previousMachineSt = this->machineSt;
	  this->machineSt = newState;
	}

      public:
	SimpleMenu()
	{
	  for (auto &x : this->items)
	    x = nullptr;

	  this->idleTimer.stop();
	  this->switchTimer.stop();

	  this->setMachineState(MachineSt::DisplayMode);
	  this->switchTimer.start(this->SwitchTime);
	}

	SimpleMenu(SegmentDisplayInterface *display, ButtonKeyboardInterface *keyboard) : SimpleMenu()
	{
	  this->setDisplay(display);
	  this->setKeyboard(keyboard);
	}

	bool addItem(MenuItemInterface *item, bool inScrollMenu = false)
	{
	  if(this->getItemsCount()<size)
	  {
	    this->items[this->getItemsCount()] = item;
	    this->inScrollMenu[this->getItemsCount()] = inScrollMenu;
	    this->atLeasOneInScroll |= inScrollMenu;
	    this->itemsCount++;

	    return true;
	  }
	  return false;
	}

	void setDisplay(SegmentDisplayInterface *display)
	{
	  this->display = display;
	}

	void setKeyboard(ButtonKeyboardInterface *keyboard)
	{
	  this->keyboard = keyboard;
	}

	uint32_t getItemsCount() const
	{
	  return this->itemsCount;
	}

	bool enteredIntoEditMode() const
	{
	  return this->machineSt==MachineSt::EditMode && this->previousMachineSt==MachineSt::DisplayMode;
	}

	bool enteredIntoDisplayMode() const
	{
	  return this->machineSt==MachineSt::DisplayMode && this->previousMachineSt==MachineSt::EditMode;
	}

	//IDaemon----------------------------------------------
	void handler()
	{
	  bool userAction = !this->keyboard->isEmpy();

	  if(userAction)
	  {
	    this->setMachineState(MachineSt::EditMode);
	    this->idleTimer.start(this->IdleTimeout);
	  }
	  else
	  {
	    if(this->idleTimer.check())
	    {
	      this->idleTimer.stop();
	      this->setMachineState(MachineSt::DisplayMode);

	      this->items[this->index]->clearFocus();

	      this->switchTimer.start(this->SwitchTime);
	    }
	  }


	  switch(this->machineSt)
	  {
	    case MachineSt::DisplayMode:
	    {
	      if(this->switchTimer.checkAndRestart())
	      {
		if(this->atLeasOneInScroll)
		{
		  this->index++;
		  this->index %= this->itemsCount;
		  while(this->inScrollMenu[this->index]==false)
		  {
		    this->index++;
		    this->index %= this->itemsCount;
		  }
		}

	      }
	    }
	    break;

	    case MachineSt::EditMode:
	    {
	      auto &item = this->items[this->index];

	      if(userAction)
	      {
		auto ev = this->keyboard->fetch();

		if(ev.evType == ButtonKeyboardDefs::EventTypes::Clicked)
		{
		  if(ev.isKey(ButtonKeyboardDefs::UP))
		  {
		    if(item->focused())
		      item->actionUp();
		    else
		    {
		      this->index++;
		      this->index %= this->itemsCount;
		    }
		  }

		  if(ev.isKey(ButtonKeyboardDefs::DOWN))
		  {
		    if(item->focused())
		      item->actionDown();
		    else
		    {
		      if( this->index == 0)
			 this->index = this->itemsCount;

		      this->index--;
		    }
		  }

		  if(ev.isKey(ButtonKeyboardDefs::ENTER))
		    item->actionEnter();
		}
	      }
	    }
	    break;
	  }

	  this->items[this->index]->draw(this->display);
	}
	//-----------------------------------------------------


    };
}

#endif /* INC_APP_MENU_SIMPLE_MENU_H_ */
