/*
 * board.h
 *
 *  Created on: 9 lis 2020
 *      Author: theoar
 */

#ifndef INC_BOARD_BOARD_H_
#define INC_BOARD_BOARD_H_

#include "app/segemnt_display/digit.h"
#include "app/segemnt_display/digit_display.h"

#include "app/rotary_dip_switch/rotary_dip_switch.h"
#include "app/higrometer/higrometer.h"

#include "app/binary_regulator/binary_regulator.h"

#include "devices/dht11/dht11.h"

#include "peripherials/gpio/gpio_pin.h"

#include "app/daemon_ctrl/daemon_ctrl.h"


using namespace segment_display;
using namespace rotary_dip_switch;
using namespace devices;
using namespace higrometer;
using namespace daemon_ctrl;
using namespace gpio;
using namespace binary_regulator;

namespace board
{
  class Board
  {
    private:
      void initRcc();
      void initGpio();
      void initDisplay();
      void initGlobalTimer();
      void initRotaryDipSwitch(Daemon *daemon);
      void initDhtDriver(Daemon *daemon);
      void initHigrometer(Daemon *daemon);
      void initRegulator();
      void initBlinkLed();
      void initRelay();

      BasicDigit firstDigit;
      BasicDigit secondDigit;
      BasicDigitDisplay<2> display;

      RotaryDipSwitch<16> rds;
      Pin hex1;
      Pin hex2;
      Pin hex4;
      Pin hex8;

      Pin led;

      Pin relayPin;

      Dht11 dht11;
      Higrometer higrometer;

    public:
      Board() = default;
      Board(Daemon *daemon);
      void init(Daemon *daemon);

      BasicDigitDisplay<2> & getDisplay();
      RotaryDipSwitch<16> & getRotaryDipSwitch();
      Dht11 & getDhtDriver();
      Higrometer & getHigrometer();
      Pin & getLed();
      Pin & getRelayPin();
      void setRelayState(bool enabled);

  };
}


#endif /* INC_BOARD_BOARD_H_ */
