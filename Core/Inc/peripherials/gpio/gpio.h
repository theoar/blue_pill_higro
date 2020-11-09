/*
 * gpio.h
 *
 *  Created on: 31 paź 2020
 *      Author: theoar
 */

#ifndef INC_PERIPHERIALS_GPIO_GPIO_H_
#define INC_PERIPHERIALS_GPIO_GPIO_H_

#include "proc.h"

#include "gpio_defs.h"

namespace gpio
{
  class Gpio
  {
    public:
      static void init();
      static void init(GpioTypes::GpioNr type);
      static GPIO_TypeDef *getGpio(GpioTypes::GpioNr type);
  };
}

#endif /* INC_PERIPHERIALS_GPIO_GPIO_H_ */
