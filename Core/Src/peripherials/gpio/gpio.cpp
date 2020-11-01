/*
 * gpio.cpp
 *
 *  Created on: 31 paź 2020
 *      Author: theoar
 */

#include "peripherials/gpio/gpio.h"
#include "peripherials/gpio/gpio_defs.h"
#include "peripherials/rcc/rcc.h"

using namespace rcc;

namespace gpio
{
  void Gpio::init()
  {
    LL_GPIO_AF_Remap_SWJ_NOJTAG();
  }

  void Gpio::init(GpioTypes::GpioNr type)
  {
    switch(type)
    {
      case GpioTypes::GpioNr::A: Rcc::peripheralClockEnable(PeripheralClock::GPIOA_PER); break;
      case GpioTypes::GpioNr::B: Rcc::peripheralClockEnable(PeripheralClock::GPIOB_PER); break;
      case GpioTypes::GpioNr::C: Rcc::peripheralClockEnable(PeripheralClock::GPIOC_PER); break;
    }
  }

  GPIO_TypeDef *Gpio::getGpio(GpioTypes::GpioNr type)
  {
    switch(type)
    {
      case GpioTypes::GpioNr::A: return GPIOA;
      case GpioTypes::GpioNr::B: return GPIOB;
      case GpioTypes::GpioNr::C: return GPIOC;
      default: return nullptr;
    }
  }
}


