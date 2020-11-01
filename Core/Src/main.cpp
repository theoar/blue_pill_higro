/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include "app/soft_timer/soft_timer.h"
#include "app/higrometer/higrometer.h"
#include "app/daemon_ctrl/daemon_ctrl.h"

#include "peripherials/timer/timer_events.h"
#include "peripherials/rcc/rcc.h"
#include "peripherials/gpio/gpio.h"
#include "peripherials/gpio/gpio_pin.h"
#include "peripherials/gpio/gpio_defs.h"

#include "devices/dht11/dht11.h"

#include "system_clock_config.h"

using namespace timer;
using namespace rcc;
using namespace gpio;
using namespace devices;
using namespace higrometer;
using namespace daemon_ctrl;

int main(void)
{
  systemClockConfig();

  Rcc::init(72000000);
  Gpio::init();

  Gpio::init(GpioTypes::GpioNr::A);
  Gpio::init(GpioTypes::GpioNr::B);
  Gpio::init(GpioTypes::GpioNr::C);

  TimerEvents::getSysTickEvent()->setEvent(&GlobalTimer::increment);

  SoftTimer blinkTimer;
  blinkTimer.start(1000);
  Pin led(GpioTypes::GpioNr::C, LL_GPIO_PIN_13, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_MODE_OUTPUT_50MHz);

  Dht11 dht11;
  dht11.init(GpioTypes::GpioNr::B, LL_GPIO_PIN_6, TimerDefs::TimerNr::Tim4, LL_TIM_CHANNEL_CH1);

  Higrometer higrometer;
  higrometer.init(&dht11, 2000);

  Daemon deamon;
  deamon.addAndStartProcess(&dht11);
  deamon.addAndStartProcess(&higrometer);

  volatile uint32_t h = 0;
  volatile uint32_t t = 0;

  while(1)
  {
    if(blinkTimer.check())
    {
      led.toggle();
      blinkTimer.restart();
    }

    if(higrometer.isConnected())
    {
      h = higrometer.getHumidity();
      t = higrometer.getTemperature();
    }

    deamon.handler();
  }

}

