/*
 * gpio_pin.h
 *
 *  Created on: 31 paź 2020
 *      Author: theoar
 */

#ifndef INC_PERIPHERIALS_GPIO_GPIO_PIN_H_
#define INC_PERIPHERIALS_GPIO_GPIO_PIN_H_

#include <inttypes.h>

#include "gpio_defs.h"
#include "gpio.h"

namespace gpio
{
  class Pin
  {
    private:
      uint32_t pinNo;
      GpioTypes::GpioNr gpioNo;
    public:
      Pin() = default;
      Pin(GpioTypes::GpioNr gpioNo, uint32_t pin, uint32_t mode, uint32_t outputType, uint32_t pull, uint32_t speed);
      void configure(GpioTypes::GpioNr gpioNo, uint32_t pin, uint32_t mode, uint32_t outputType, uint32_t pull, uint32_t speed);
      void setMode(uint32_t mode);
      void setOutputType(uint32_t oType);
      void setPull(uint32_t pull);
      void setSpeed(uint32_t speed);
      void toggle();
      void set();
      void reset();
      void state(bool state);
      bool state();

      static void init(GpioTypes::GpioNr gpioNo, uint32_t pin, uint32_t mode, uint32_t outputType, uint32_t pull, uint32_t speed);
      static void setMode(GpioTypes::GpioNr gpioNo, uint32_t pinNo, uint32_t mode);
      static void setOutputType(GpioTypes::GpioNr gpioNo, uint32_t pinNo, uint32_t oType);
      static void setPull(GpioTypes::GpioNr gpioNo, uint32_t pinNo, uint32_t pull);
      static void setSpeed(GpioTypes::GpioNr gpioNo, uint32_t pinNo, uint32_t speed);
      static void toggle(GpioTypes::GpioNr gpioNo, uint32_t pinNo);
      static void set(GpioTypes::GpioNr gpioNo, uint32_t pinNo);
      static void reset(GpioTypes::GpioNr gpioNo, uint32_t pinNo);
      static void state(GpioTypes::GpioNr gpioNo, uint32_t pinNo, bool state);
      static bool state(GpioTypes::GpioNr gpioNo, uint32_t pinNo);
  };
}

#endif /* INC_PERIPHERIALS_GPIO_GPIO_PIN_H_ */
