/*
 * button.h
 *
 *  Created on: 30 lis 2020
 *      Author: theoar
 */

#ifndef INC_APP_BUTTON_BUTTON_H_
#define INC_APP_BUTTON_BUTTON_H_

#include "app/daemon_ctrl/daemon_interface.h"
#include "app/soft_timer/soft_timer.h"
#include "peripherials/gpio/gpio_pin.h"

using namespace daemon_ctrl;
using namespace gpio;

namespace button
{
  class Button : public IDaemon
  {
    public:
      enum class State
      {
	None,
	Clicked,
	Released
      };

    private:
      enum class MachineSt
      {
	Idle,
	Debounce,
	Clicked,
      };

      MachineSt machineSt = MachineSt::Idle;

      Pin * pin = nullptr;

      SoftTimer timer;
      static constexpr uint32_t DebounceMs = 50;
      static constexpr uint32_t RepateMs = 300;
      static constexpr uint32_t RepateTickMs = 25;

      uint32_t repateMs = RepateMs;

      bool clicked = false;
      bool holded = false;
      bool released = false;
      bool repeated = false;

      bool clickedState = false;
    public:
      Button() = default;
      void configure(Pin *pin, bool cklickedState);

      void handler();

      bool isClicked();
      bool isHolded();
      bool isReleased();
      bool isRepeated();

  };
}



#endif /* INC_APP_BUTTON_BUTTON_H_ */
