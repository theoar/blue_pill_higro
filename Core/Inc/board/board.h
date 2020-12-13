/*
 * board.h
 *
 *  Created on: 9 lis 2020
 *      Author: theoar
 */

#ifndef INC_BOARD_BOARD_H_
#define INC_BOARD_BOARD_H_

#include "app/segemnt_display/digit.h"
#include "app/segemnt_display/multiplex_digit_display.h"

#include "app/rotary_dip_switch/rotary_dip_switch.h"
#include "app/higrometer/higrometer.h"

#include "app/binary_regulator/binary_regulator.h"

#include "app/button/button_keyboard.h"

#include "app/daemon_ctrl/daemon_ctrl.h"

#include "devices/dht11/dht11.h"

#include "peripherials/gpio/gpio_pin.h"


using namespace segment_display;
using namespace rotary_dip_switch;
using namespace devices;
using namespace higrometer;
using namespace daemon_ctrl;
using namespace gpio;
using namespace button;
using namespace binary_regulator;


namespace board
{
  class Board
  {
    private:
      void initRcc();
      void initGpio();
      void initBackupDomain();
      void initDisplay(Daemon *daemon);
      void initGlobalTimer();
      void initKeyboard(Daemon *daemon);
      void initDhtDriver(Daemon *daemon);
      void initHigrometer(Daemon *daemon);
      void initRegulator();
      void initBlinkLed();
      void initRelay();

      MultiplexScrollDigitDisplay<4> display;
      Pin digit1Power;
      Pin digit2Power;
      Pin digit3Power;
      Pin digit4Power;
      BasicDigit digit;

      ButtonKeyboardSimple<3> keyboard;
      Button enter;
      Button up;
      Button down;
      Pin enterPin;
      Pin upPin;
      Pin downPin;

      Pin led;

      Pin relayPin;

      Dht11 dht11;
      Higrometer higrometer;

    public:
      Board() = default;
      Board(Daemon *daemon);
      void init(Daemon *daemon);

      MultiplexScrollDigitDisplay<4> & getDisplay();
      Dht11 & getDhtDriver();
      Higrometer & getHigrometer();
      ButtonKeyboardSimple<3> &getKeyboard();
      Pin & getLed();
      Pin & getRelayPin();
      void setRelayState(bool enabled);
  };
}


#endif /* INC_BOARD_BOARD_H_ */
