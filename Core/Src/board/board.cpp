/*
 * board.cpp
 *
 *  Created on: 9 lis 2020
 *      Author: theoar
 */

#ifndef SRC_BOARD_BOARD_CPP_
#define SRC_BOARD_BOARD_CPP_

#include "peripherials/rcc/rcc.h"
#include "peripherials/rtc/rtc.h"
#include "peripherials/bkp/bkp.h"

#include "board/board.h"
#include "system_clock_config.h"

using namespace rcc;
using namespace bkp;
using namespace rtc;

namespace board
{
  Board::Board(Daemon *daemon)
  {
    this->init(daemon);
  }

  void Board::init(Daemon *daemon)
  {
    this->initRcc();
    this->initBackupDomain();
    this->initGlobalTimer();
    this->initGpio();
    this->initKeyboard(daemon);
    this->initDisplay(daemon);
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

  void Board::initBackupDomain()
  {
    Rtc::init();
    BackupDomain::init();
  }

  void Board::initGpio()
  {
    Gpio::init();
    Gpio::init(GpioTypes::GpioNr::A);
    Gpio::init(GpioTypes::GpioNr::B);
    Gpio::init(GpioTypes::GpioNr::C);
  }

  void Board::initDisplay(Daemon *daemon)
  {
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

    this->display.setPowerPinAt(3, &this->digit1Power);
    this->display.setPowerPinAt(2, &this->digit2Power);
    this->display.setPowerPinAt(1, &this->digit3Power);
    this->display.setPowerPinAt(0, &this->digit4Power);

    this->display.setDigitConfig(&this->digit);

    daemon->addAndStartProcess(&this->display);
  }

  void Board::initKeyboard(Daemon *daemon)
  {
    this->enterPin = Pin(GpioTypes::GpioNr::B, LL_GPIO_PIN_11, LL_GPIO_MODE_INPUT, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_UP, LL_GPIO_SPEED_FREQ_HIGH);
    this->upPin = Pin(GpioTypes::GpioNr::B, LL_GPIO_PIN_10, LL_GPIO_MODE_INPUT, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_UP, LL_GPIO_SPEED_FREQ_HIGH);
    this->downPin = Pin(GpioTypes::GpioNr::B, LL_GPIO_PIN_1, LL_GPIO_MODE_INPUT, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_UP, LL_GPIO_SPEED_FREQ_HIGH);

    this->enter.configure(&this->enterPin, false);
    this->up.configure(&this->upPin, false);
    this->down.configure(&this->downPin, false);

    this->keyboard.addButton(&this->enter, ButtonKeyboardDefs::ENTER);
    this->keyboard.addButton(&this->up, ButtonKeyboardDefs::UP);
    this->keyboard.addButton(&this->down, ButtonKeyboardDefs::DOWN);

    daemon->addAndStartProcess(&this->enter);
    daemon->addAndStartProcess(&this->up);
    daemon->addAndStartProcess(&this->down);

    daemon->addAndStartProcess(&this->keyboard);
  }

  void Board::initGlobalTimer()
  {
    TimerEvents::getSysTickEvent()->setEvent(&GlobalTimer::increment);
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
    this->led.configure(GpioTypes::GpioNr::C, LL_GPIO_PIN_13 ,LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz);
  }

  void Board::initRelay()
  {
    this->relayPin.configure(GpioTypes::GpioNr::B, LL_GPIO_PIN_15, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz);
  }

  SegmentDisplayInterface & Board::getDisplay()
  {
    return this->display;
  }

  ButtonKeyboardInterface & Board::getKeyboard()
  {
    return this->keyboard;
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
