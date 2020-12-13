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
	ButtonKeyboardSimple<3> *keyboard = nullptr;
	MultiplexScrollDigitDisplay<4> *display = nullptr;
	MenuItemInterface *items[size];
	SoftTimer idleTimer;
	SoftTimer switchTimer;
	static constexpr uint32_t IdleTimeout = 5000;
	static constexpr uint32_t SwitchTime = 2000;

	uint32_t index = 0;

	enum class MachineSt
	{
	  EditMode, DisplayMode
	} machineSt = MachineSt::DisplayMode;
      public:
	SimpleMenu()
	{
	  for (auto &x : this->items)
	    x = nullptr;

	  this->idleTimer.stop();
	  this->switchTimer.stop();

	  this->machineSt = MachineSt::DisplayMode;
	  this->switchTimer.start(this->SwitchTime);
	}

	void addItemAt(MenuItemInterface *item, uint32_t pos)
	{
	  if(index<size)
	    this->items[pos] = item;
	}

	void setDisplay(MultiplexScrollDigitDisplay<4> *display)
	{
	  this->display = display;
	}

	void setKeyboard(ButtonKeyboardSimple<3> *keyboard)
	{
	  this->keyboard = keyboard;
	}

	//IDaemon----------------------------------------------
	void handler()
	{
	  bool userAction = !this->keyboard->isEmpy();

	  if(userAction)
	  {
	    this->machineSt = MachineSt::EditMode;
	    this->idleTimer.start(this->IdleTimeout);
	  }
	  else
	  {
	    if(this->idleTimer.check())
	    {
	      this->idleTimer.stop();
	      this->machineSt = MachineSt::DisplayMode;

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
		this->index++;
		this->index %= size;
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
		      this->index %= size;
		    }
		  }

		  if(ev.isKey(ButtonKeyboardDefs::DOWN))
		  {
		    if(item->focused())
		      item->actionDown();
		    else
		    {
		      if( this->index == 0)
			 this->index =  size;

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
