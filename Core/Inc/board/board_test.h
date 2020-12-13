/*
 * board_test.h
 *
 *  Created on: 23 lis 2020
 *      Author: theoar
 */

#ifndef INC_BOARD_BOARD_TEST_H_
#define INC_BOARD_BOARD_TEST_H_

#include "app/daemon_ctrl/daemon_ctrl.h"
#include "app/daemon_ctrl/daemon_interface.h"

#include "peripherials/rcc/rcc.h"
#include "peripherials/gpio/gpio.h"
#include "peripherials/gpio/gpio_pin.h"

#include "peripherials/timer/timer_events.h"

#include "app/segemnt_display/multiplex_digit_display.h"
#include "app/segemnt_display/digit.h"

#include "system_clock_config.h"


using namespace daemon_ctrl;
using namespace rcc;
using namespace gpio;
using namespace timer;
using namespace segment_display;

namespace board
{
  class BoardTest : public IDaemon
  {
    public:
      MultiplexDigitDisplay<4> display;
      MultiplexScrollDigitDisplay<4> display1;
      Pin digit1Power;
      Pin digit2Power;
      Pin digit3Power;
      Pin digit4Power;
      BasicDigit digit;

    public:
      BoardTest(Daemon *daemon)
      {
	systemClockConfig();
	Rcc::init(72000000);

	Gpio::init();
	Gpio::init(GpioTypes::GpioNr::A);
	Gpio::init(GpioTypes::GpioNr::B);
	Gpio::init(GpioTypes::GpioNr::C);

	TimerEvents::getSysTickEvent()->setEvent(&GlobalTimer::increment);

	this->digit.setCommonAnode();
	this->digit.configure(DigitDisplayDefs::Segments::A, LL_GPIO_PIN_0, GpioTypes::GpioNr::A);
	this->digit.configure(DigitDisplayDefs::Segments::B, LL_GPIO_PIN_1, GpioTypes::GpioNr::A);
	this->digit.configure(DigitDisplayDefs::Segments::C, LL_GPIO_PIN_2, GpioTypes::GpioNr::A);
	this->digit.configure(DigitDisplayDefs::Segments::D, LL_GPIO_PIN_3, GpioTypes::GpioNr::A);
	this->digit.configure(DigitDisplayDefs::Segments::E, LL_GPIO_PIN_4, GpioTypes::GpioNr::A);
	this->digit.configure(DigitDisplayDefs::Segments::F, LL_GPIO_PIN_5, GpioTypes::GpioNr::A);
	this->digit.configure(DigitDisplayDefs::Segments::G, LL_GPIO_PIN_6, GpioTypes::GpioNr::A);
	this->digit.configure(DigitDisplayDefs::Segments::DOTP, LL_GPIO_PIN_7, GpioTypes::GpioNr::A);

	this->digit1Power.configure(GpioTypes::GpioNr::B, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_SPEED_FREQ_HIGH);
	this->digit2Power.configure(GpioTypes::GpioNr::B, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_SPEED_FREQ_HIGH);
	this->digit3Power.configure(GpioTypes::GpioNr::B, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_SPEED_FREQ_HIGH);
	this->digit4Power.configure(GpioTypes::GpioNr::B, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_SPEED_FREQ_HIGH);

//	this->display.setPowerPinAt(0, &this->digit1Power);
//	this->display.setPowerPinAt(1, &this->digit2Power);
//	this->display.setPowerPinAt(2, &this->digit3Power);
//	this->display.setPowerPinAt(3, &this->digit4Power);
//
//	this->display.setDigitConfig(&this->digit);

	this->display1.setPowerPinAt(3, &this->digit1Power);
	this->display1.setPowerPinAt(2, &this->digit2Power);
	this->display1.setPowerPinAt(1, &this->digit3Power);
	this->display1.setPowerPinAt(0, &this->digit4Power);

	this->display1.setDigitConfig(&this->digit);

	daemon->addAndStartProcess(this);
	//daemon->addAndStartProcess(&this->display);

	daemon->addAndStartProcess(&this->display1);
      }

      virtual void handler(void) {};

  };
}



#endif /* INC_BOARD_BOARD_TEST_H_ */
