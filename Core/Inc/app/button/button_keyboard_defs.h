/*
 * button_keyboard_defs.h
 *
 *  Created on: 2 gru 2020
 *      Author: theoar
 */

#ifndef INC_APP_BUTTON_BUTTON_KEYBOARD_DEFS_H_
#define INC_APP_BUTTON_BUTTON_KEYBOARD_DEFS_H_

namespace button
{
  class ButtonKeyboardDefs
  {
    public:
      static constexpr char ENTER = 0x10;
      static constexpr char ESC = 0x11;
      static constexpr char UP = 0x12;
      static constexpr char DOWN = 0x13;
      static constexpr char LEFT = 0x14;
      static constexpr char RIGHT = 0x15;

      enum class EventTypes
      {
	Clicked, Released, Shortcut
      };

      struct EventSt
      {
	  union
	  {
	      char key;

	      struct PairSt
	      {
		  char key1;
		  char key2;
	      } shortcut;

	  } type;

	  ButtonKeyboardDefs::EventTypes evType;

	  bool isKey(char key)
	  {
	    if(this->evType != ButtonKeyboardDefs::EventTypes::Shortcut)
	      return this->type.key == key;
	    else
	      return 0;
	  }

	  bool isKey(char key1, char key2)
	  {
	    return (this->type.shortcut.key1 == key1
		&& this->type.shortcut.key2 == key2)
		|| (this->type.shortcut.key2 == key1
		    && this->type.shortcut.key2 == key1);
	  }

	  char key()
	  {
	    if(this->evType != ButtonKeyboardDefs::EventTypes::Shortcut)
	      return this->type.key;
	    else
	      return 0;
	  }
      };

  };
}

#endif /* INC_APP_BUTTON_BUTTON_KEYBOARD_DEFS_H_ */
