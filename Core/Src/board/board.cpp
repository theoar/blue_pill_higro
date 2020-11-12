/*
 * board.cpp
 *
 *  Created on: 9 lis 2020
 *      Author: theoar
 */

#ifndef SRC_BOARD_BOARD_CPP_
#define SRC_BOARD_BOARD_CPP_

#include "../system_clock_config.h"

#include "peripherials/rcc/rcc.h"

#include "board/board.h"

using namespace rcc;

namespace board
{
  Board::Board(Daemon *daemon)
  {
    this->init(daemon);
  }

  void Board::init(Daemon *daemon)
  {
    this->initRcc();
    this->initGlobalTimer();
    this->initGpio();
    this->initDisplay();
    this->initRotaryDipSwitch(daemon);
    this->initDhtDriver(daemon);
    this->initHigrometer(daemon);
    this->initBlinkLed();
    this->initRelay();
  }

  void Board::initRcc()
  {
    systemClockConfig();
    Rcc::init(72000000);
  }

  void Board::initGpio()
  {
    Gpio::init();
    Gpio::init(GpioTypes::GpioNr::A);
    Gpio::init(GpioTypes::GpioNr::B);
    Gpio::init(GpioTypes::GpioNr::C);
  }

  void Board::initDisplay()
  {
    this->firstDigit.setCommonAnode();
    this->firstDigit.configure(DigitDisplayDefs::Segments::A, LL_GPIO_PIN_8, GpioTypes::GpioNr::B);
    this->firstDigit.configure(DigitDisplayDefs::Segments::B, LL_GPIO_PIN_9, GpioTypes::GpioNr::B);
    this->firstDigit.configure(DigitDisplayDefs::Segments::F, LL_GPIO_PIN_7, GpioTypes::GpioNr::B);
    this->firstDigit.configure(DigitDisplayDefs::Segments::G, LL_GPIO_PIN_5, GpioTypes::GpioNr::B);
    this->firstDigit.configure(DigitDisplayDefs::Segments::C, LL_GPIO_PIN_4, GpioTypes::GpioNr::B);
    this->firstDigit.configure(DigitDisplayDefs::Segments::D, LL_GPIO_PIN_3, GpioTypes::GpioNr::B);
    this->firstDigit.configure(DigitDisplayDefs::Segments::E, LL_GPIO_PIN_15, GpioTypes::GpioNr::A);
    this->firstDigit.clear();


    this->secondDigit.setCommonAnode();
    this->secondDigit.configure(DigitDisplayDefs::Segments::G, LL_GPIO_PIN_12, GpioTypes::GpioNr::A);
    this->secondDigit.configure(DigitDisplayDefs::Segments::B, LL_GPIO_PIN_11, GpioTypes::GpioNr::A);
    this->secondDigit.configure(DigitDisplayDefs::Segments::C, LL_GPIO_PIN_10, GpioTypes::GpioNr::A);
    this->secondDigit.configure(DigitDisplayDefs::Segments::D, LL_GPIO_PIN_8, GpioTypes::GpioNr::A);
    this->secondDigit.configure(DigitDisplayDefs::Segments::A, LL_GPIO_PIN_15, GpioTypes::GpioNr::B);
    this->secondDigit.configure(DigitDisplayDefs::Segments::E, LL_GPIO_PIN_14, GpioTypes::GpioNr::B);
    this->secondDigit.configure(DigitDisplayDefs::Segments::F, LL_GPIO_PIN_13, GpioTypes::GpioNr::B);
    this->secondDigit.clear();

    this->display.setDigitAt(0, &this->firstDigit);
    this->display.setDigitAt(1, &this->secondDigit);
  }

  void Board::initGlobalTimer()
  {
    TimerEvents::getSysTickEvent()->setEvent(&GlobalTimer::increment);
  }

  void Board::initRotaryDipSwitch(Daemon *daemon)
  {
    this->hex1.configure(GpioTypes::GpioNr::C, LL_GPIO_PIN_15, LL_GPIO_MODE_INPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz);
    this->hex2.configure(GpioTypes::GpioNr::C, LL_GPIO_PIN_14, LL_GPIO_MODE_INPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz);
    this->hex4.configure(GpioTypes::GpioNr::A, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz);
    this->hex8.configure(GpioTypes::GpioNr::A, LL_GPIO_PIN_2, LL_GPIO_MODE_INPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz);

    rds.pinAt(0, &hex1);
    rds.pinAt(1, &hex2);
    rds.pinAt(2, &hex4);
    rds.pinAt(3, &hex8);

    daemon->addAndStartProcess(&this->rds);
  }

  void Board::initDhtDriver(Daemon *daemon)
  {
    this->dht11.init(GpioTypes::GpioNr::B, LL_GPIO_PIN_6, TimerDefs::TimerNr::Tim4, LL_TIM_CHANNEL_CH1);
    daemon->addAndStartProcess(&this->dht11);
  }

  void Board::initHigrometer(Daemon *daemon)
  {
    this->higrometer.init(&dht11, 2000);
    daemon->addAndStartProcess(&this->higrometer);
  }

  void Board::initBlinkLed()
  {
    this->led.configure(GpioTypes::GpioNr::C, LL_GPIO_PIN_7 ,LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz);
  }

  void Board::initRelay()
  {
    this->relayPin.configure(GpioTypes::GpioNr::A, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz);
  }

  BasicDigitDisplay<2> & Board::getDisplay()
  {
    return this->display;
  }

  RotaryDipSwitch<16> & Board::getRotaryDipSwitch()
  {
    return this->rds;
  }

  Dht11 & Board::getDhtDriver()
  {
    return this->dht11;
  }

  Higrometer & Board::getHigrometer()
  {
    return this->higrometer;
  }

  Pin & Board::getLed()
  {
    return this->led;
  }

  Pin & Board::getRelayPin()
  {
    return this->relayPin;
  }

  void Board::setRelayState(bool enabled)
  {
    if(enabled)
      this->relayPin.reset();
    else
      this->relayPin.set();
  }

}



#endif /* SRC_BOARD_BOARD_CPP_ */
