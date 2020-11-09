/*
 * gpio_pin.cpp
 *
 *  Created on: 31 paź 2020
 *      Author: theoar
 */

#include "peripherials/gpio/gpio_pin.h"

namespace gpio
{
  Pin::Pin(GpioTypes::GpioNr gpioNo, uint32_t pin, uint32_t mode, uint32_t outputType, uint32_t pull, uint32_t speed)
  {
    this->configure(gpioNo, pin, mode, outputType, pull, speed);
  }

  void Pin::configure(GpioTypes::GpioNr gpioNo, uint32_t pin, uint32_t mode, uint32_t outputType, uint32_t pull, uint32_t speed)
  {
    LL_GPIO_InitTypeDef config;
    config.Mode = mode;
    config.OutputType = outputType;
    config.Pull = pull;
    config.Pin = pin;
    config.Speed = speed;

    LL_GPIO_Init(Gpio::getGpio(gpioNo), &config);

    this->pinNo = pin;
    this->gpioNo = gpioNo;
  }

  void Pin::setMode(uint32_t mode)
  {
    LL_GPIO_SetPinMode(Gpio::getGpio(this->gpioNo), this->pinNo, mode);
  }
  void Pin::setOutputType(uint32_t oType)
  {
    LL_GPIO_SetPinOutputType(Gpio::getGpio(this->gpioNo), this->pinNo, oType);
  }
  void Pin::setPull(uint32_t pull)
  {
    LL_GPIO_SetPinPull(Gpio::getGpio(this->gpioNo), this->pinNo, pull);
  }
  void Pin::setSpeed(uint32_t speed)
  {
    LL_GPIO_SetPinSpeed(Gpio::getGpio(this->gpioNo), this->pinNo, speed);
  }

  void Pin::init(GpioTypes::GpioNr gpioNo, uint32_t pin, uint32_t mode, uint32_t outputType, uint32_t pull, uint32_t speed)
  {
    Pin p;
    p.configure(gpioNo, pin, mode, outputType, pull, speed);
  }

  void Pin::setMode(GpioTypes::GpioNr gpioNo, uint32_t pinNo, uint32_t mode)
  {
    LL_GPIO_SetPinMode(Gpio::getGpio(gpioNo), pinNo, mode);
  }
  void Pin::setOutputType(GpioTypes::GpioNr gpioNo, uint32_t pinNo, uint32_t oType)
  {
    LL_GPIO_SetPinOutputType(Gpio::getGpio(gpioNo), pinNo, oType);
  }
  void Pin::setPull(GpioTypes::GpioNr gpioNo, uint32_t pinNo, uint32_t pull)
  {
    LL_GPIO_SetPinPull(Gpio::getGpio(gpioNo), pinNo, pull);
  }
  void Pin::setSpeed(GpioTypes::GpioNr gpioNo, uint32_t pinNo, uint32_t speed)
  {
    LL_GPIO_SetPinSpeed(Gpio::getGpio(gpioNo), pinNo, speed);
  }

  void Pin::toggle()
  {
    LL_GPIO_TogglePin(Gpio::getGpio(this->gpioNo), this->pinNo);
  }

  void Pin::set()
  {
    LL_GPIO_SetOutputPin(Gpio::getGpio(this->gpioNo), this->pinNo);
  }

  void Pin::reset()
  {
    LL_GPIO_ResetOutputPin(Gpio::getGpio(this->gpioNo), this->pinNo);
  }

  void Pin::state(bool state)
  {
    if (state)
      this->set();
    else
      this->reset();
  }

  bool Pin::state()
  {
    return LL_GPIO_IsInputPinSet(Gpio::getGpio(this->gpioNo), this->pinNo)!=0;
  }

  void Pin::toggle(GpioTypes::GpioNr gpioNo, uint32_t pinNo)
  {
    LL_GPIO_TogglePin(Gpio::getGpio(gpioNo), pinNo);
  }

  void Pin::set(GpioTypes::GpioNr gpioNo, uint32_t pinNo)
  {
    LL_GPIO_SetOutputPin(Gpio::getGpio(gpioNo), pinNo);
  }

  void Pin::reset(GpioTypes::GpioNr gpioNo, uint32_t pinNo)
  {
    LL_GPIO_ResetOutputPin(Gpio::getGpio(gpioNo), pinNo);
  }

  void Pin::state(GpioTypes::GpioNr gpioNo, uint32_t pinNo, bool state)
  {
    if (state)
      Pin::set(gpioNo, pinNo);
    else
      Pin::reset(gpioNo, pinNo);
  }

  bool Pin::state(GpioTypes::GpioNr gpioNo, uint32_t pinNo)
  {
    return LL_GPIO_IsInputPinSet(Gpio::getGpio(gpioNo), pinNo)!=0;
  }

}

