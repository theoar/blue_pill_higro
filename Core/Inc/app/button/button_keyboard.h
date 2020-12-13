/*
 * button_keyboard.h
 *
 *  Created on: 1 gru 2020
 *      Author: theoar
 */

#ifndef INC_APP_BUTTON_BUTTON_KEYBOARD_H_
#define INC_APP_BUTTON_BUTTON_KEYBOARD_H_

#include "button.h"
#include "button_keyboard_defs.h"

#include "app/circular_buffer/circular buffer.h"

#include "app/soft_timer/soft_timer.h"

#include "app/daemon_ctrl/daemon_interface.h"

using namespace circular_buffer;
using namespace daemon_ctrl;

namespace button
{

  class ButtonKeyboardInterface : public IDaemon
  {
    protected:
	struct Key
	{
	    Button *btn = nullptr;
	    char key;
	};

    public:
      //ButtonKeyboardInterface-
      virtual bool isEmpy() = 0;
      virtual void addButton(Button *btn, char key) = 0;
      virtual void setButtonAt(Button *btn, char key, uint32_t ind) = 0;
      virtual ButtonKeyboardDefs::EventSt fetch() = 0;

      //IDaemon
      virtual void handler(void) = 0;
  };

  template<int size>
    class ButtonKeyboardSimple : public ButtonKeyboardInterface
    {
      protected:
	Key keys[size];
	CircularBuffer<16, ButtonKeyboardDefs::EventSt> queue;
	uint32_t index = 0;

      public:
	ButtonKeyboardSimple()
	{
	  for (uint32_t x = 0; x < size; ++x)
	    this->keys[x].btn = nullptr;
	}

	bool isEmpy()
	{
	  return this->queue.isEmpty();
	}

	ButtonKeyboardDefs::EventSt fetch()
	{
	  return this->queue.pop();
	}

	void addButton(Button *btn, char key)
	{
	  if(this->index < size)
	  {
	    this->keys[this->index] =
	      { btn, key };
	    this->index++;
	  }
	}

	void setButtonAt(Button *btn, char key, uint32_t ind)
	{
	  if(ind < size)
	    this->keys[this->index] =
	      { btn, key };
	}

	void handler()
	{
	  ButtonKeyboardDefs::EventSt ev;
	  for (uint32_t x = 0; x < size; ++x)
	  {
	    if(this->keys[x].btn->isClicked())
	    {
	      ev.evType = ButtonKeyboardDefs::EventTypes::Clicked;
	      ev.type.key = this->keys[x].key;
	      this->queue.add(ev);
	    }

	    if(this->keys[x].btn->isReleased())
	    {
	      ev.evType = ButtonKeyboardDefs::EventTypes::Released;
	      ev.type.key = this->keys[x].key;

	      this->queue.add(ev);
	    }
	  }
	}

    };


  template<int size, int shortCutSize = 0>
    class ButtonKeyboardAdvanced : public ButtonKeyboardSimple<size>
    {
      private:

	struct PairSt
	{
	    char key1 = 0;
	    char key2 = 0;
	    bool key1Ev = false;
	    bool key2Ev = false;
	    bool active = false;
	    SoftTimer timer;
	};

	static constexpr uint32_t shortCutSizeHoldMs = 100;

	PairSt shortCut[shortCutSize];

	int isShortcutKey(char key)
	{
	  int index = -1;
	  for(int x = 0; x<shortCutSize;x++)
	    if((this->shortCut[x].key1==key || this->shortCut[x].key2==key) && this->shortCut[x].active)
	      return x;

	  return index;
	}

      public:
	ButtonKeyboardAdvanced() : ButtonKeyboardSimple<size>()
	{
	  for(uint32_t x = 0; x<shortCutSize; ++x)
	    this->shortCut[x].active = false;
	}

	void addShortcut(char k1, char k2, uint32_t index)
	{
	  if(index<shortCutSize)
	  {
	    this->shortCut[index].key1 = k1;
	    this->shortCut[index].key2 = k2;
	    this->shortCut[index].key1Ev = false;
	    this->shortCut[index].key2Ev = false;
	    this->shortCut[index].active = true;
	    this->shortCut[index].timer.stop();
	  }
	}


	void removeShortcut(char k1, char k2, uint32_t index)
	{
	  if(index<shortCutSize)
	  {
	    this->shortCut[index].key1 = 0;
	    this->shortCut[index].key2 = 0;
	    this->shortCut[index].key1Ev = false;
	    this->shortCut[index].key2Ev = false;
	    this->shortCut[index].active = false;
	    this->shortCut[index].timer.stop();
	  }
	}
	virtual void handler(void)
	{
	  ButtonKeyboardDefs::EventSt ev;

	  for (uint32_t x = 0; x < size; ++x)
	  {
	    if(this->keys[x].btn != nullptr)
	    {
	      int sCutI = this->isShortcutKey(this->keys[x].key);

	      if(sCutI!=-1)
	      {
		if(this->shortCut[sCutI].timer.check())
		{
		  this->shortCut[sCutI].timer.stop();
		  if(this->shortCut[sCutI].key1Ev==true && this->shortCut[sCutI].key2Ev==true)
		  {
		    ev.evType = ButtonKeyboardDefs::EventTypes::Shortcut;
		    ev.type.shortcut.key1 = this->shortCut[sCutI].key1;
		    ev.type.shortcut.key2 = this->shortCut[sCutI].key2;

		    this->queue.add(ev);

		    this->shortCut[sCutI].key1Ev = false;
		    this->shortCut[sCutI].key2Ev = false;
		  }
		  else
		  {
		    if(this->shortCut[sCutI].key1Ev==true)
		    {
		      ev.evType = ButtonKeyboardDefs::EventTypes::Clicked;
		      ev.type.key = this->shortCut[sCutI].key1;

		      this->queue.add(ev);

		      this->shortCut[sCutI].key1Ev = false;
		    }

		    if(this->shortCut[sCutI].key2Ev==true)
		    {
		      ev.evType = ButtonKeyboardDefs::EventTypes::Clicked;
		      ev.type.key = this->shortCut[sCutI].key2;

		      this->queue.add(ev);

		      this->shortCut[sCutI].key2Ev = false;
		    }
		  }
		}
	      }

	      if(this->keys[x].btn->isClicked())
	      {
		if(sCutI!=-1 && !this->keys[x].btn->isRepeated())
		{
		  if(this->shortCut[sCutI].key1==this->keys[x].key)
		    this->shortCut[sCutI].key1Ev=true;

		  if(this->shortCut[sCutI].key2==this->keys[x].key)
		    this->shortCut[sCutI].key2Ev=true;

		  if(this->shortCut[sCutI].timer.isStopped())
		    this->shortCut[sCutI].timer.start(this->shortCutSizeHoldMs);
		}
		else
		{
		  ev.evType = ButtonKeyboardDefs::EventTypes::Clicked;
		  ev.type.key = this->keys[x].key;

		  this->queue.add(ev);
		}
	      }

	      if(this->keys[x].btn->isReleased())
	      {
		ev.evType = ButtonKeyboardDefs::EventTypes::Released;
		ev.type.key = this->keys[x].key;

		this->queue.add(ev);
	      }
	    }
	  }
	}

    };
}

#endif /* INC_APP_BUTTON_BUTTON_KEYBOARD_H_ */
