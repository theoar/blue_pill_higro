#include <inttypes.h>

#include "proc.h"

#include "peripherials/rcc/rcc.h"
#include "peripherials/rcc/rcc_interface.h"
#include "peripherials/rcc/rcc_defs.h"

using namespace rcc;

uint32_t Rcc::systemClock = 0;

void Rcc::init(uint32_t coreClock)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  Rcc::setSystemClockValue(coreClock);
}

static volatile uint32_t* ClockRegMapAt(PeripheralClock name)
{
  switch (name)
  {
    case PeripheralClock::GPIOA_PER:
      return &RCC->APB2ENR;
    case PeripheralClock::GPIOB_PER:
      return &RCC->APB2ENR;
    case PeripheralClock::GPIOC_PER:
      return &RCC->APB2ENR;

    case PeripheralClock::TIM1_PER:
      return &RCC->APB2ENR;
    case PeripheralClock::TIM2_PER:
      return &RCC->APB1ENR;
    case PeripheralClock::TIM3_PER:
      return &RCC->APB1ENR;
    case PeripheralClock::TIM4_PER:
      return &RCC->APB1ENR;

    case PeripheralClock::ADC1_PER:
      return &RCC->APB2ENR;
    case PeripheralClock::ADC2_PER:
      return &RCC->APB2ENR;

    case PeripheralClock::RTC_PER:
      return &RCC->BDCR;

    case PeripheralClock::USART1_PER:
      return &RCC->APB2ENR;
    case PeripheralClock::USART2_PER:
      return &RCC->APB1ENR;
    case PeripheralClock::USART3_PER:
      return &RCC->APB1ENR;

    case PeripheralClock::I2C1_PER:
      return &RCC->APB1ENR;
    case PeripheralClock::I2C2_PER:
      return &RCC->APB1ENR;

    case PeripheralClock::SPI1_PER:
      return &RCC->APB2ENR;
    case PeripheralClock::SPI2_PER:
      return &RCC->APB1ENR;

    default:
      return nullptr;
  }
}
;

static uint32_t ClockBitMapAt(PeripheralClock name)
{
  switch (name)
  {
    case PeripheralClock::GPIOA_PER:
      return RCC_APB2ENR_IOPAEN;
    case PeripheralClock::GPIOB_PER:
      return RCC_APB2ENR_IOPBEN;
    case PeripheralClock::GPIOC_PER:
      return RCC_APB2ENR_IOPCEN;

    case PeripheralClock::TIM1_PER:
      return RCC_APB2ENR_TIM1EN;
    case PeripheralClock::TIM2_PER:
      return RCC_APB1ENR_TIM2EN;
    case PeripheralClock::TIM3_PER:
      return RCC_APB1ENR_TIM3EN;
    case PeripheralClock::TIM4_PER:
      return RCC_APB1ENR_TIM4EN;

    case PeripheralClock::ADC1_PER:
      return RCC_APB2ENR_ADC1EN;
    case PeripheralClock::ADC2_PER:
      return RCC_APB2ENR_ADC2EN;

    case PeripheralClock::RTC_PER:
      return RCC_BDCR_RTCEN;

    case PeripheralClock::USART1_PER:
      return RCC_APB2ENR_USART1EN;
    case PeripheralClock::USART2_PER:
      return RCC_APB1ENR_USART2EN;
    case PeripheralClock::USART3_PER:
      return RCC_APB1ENR_USART3EN;

    case PeripheralClock::I2C1_PER:
      return RCC_APB1ENR_I2C1EN;
    case PeripheralClock::I2C2_PER:
      return RCC_APB1ENR_I2C2EN;

    case PeripheralClock::SPI1_PER:
      return RCC_APB2ENR_SPI1EN;
    case PeripheralClock::SPI2_PER:
      return RCC_APB1ENR_SPI2EN;

    default:
      return 0;
  }
}
;

static BusType busMapAt(PeripheralClock name)
{
  switch (name)
  {
    case PeripheralClock::GPIOA_PER:
      return BusType::APB2;
    case PeripheralClock::GPIOB_PER:
      return BusType::APB2;
    case PeripheralClock::GPIOC_PER:
      return BusType::APB2;

    case PeripheralClock::TIM1_PER:
      return BusType::APB2;
    case PeripheralClock::TIM2_PER:
      return BusType::APB1;
    case PeripheralClock::TIM3_PER:
      return BusType::APB1;
    case PeripheralClock::TIM4_PER:
      return BusType::APB1;

    case PeripheralClock::ADC1_PER:
      return BusType::APB2;
    case PeripheralClock::ADC2_PER:
      return BusType::APB2;

    case PeripheralClock::RTC_PER:
      return BusType::BDCR;

    case PeripheralClock::USART1_PER:
      return BusType::APB2;
    case PeripheralClock::USART2_PER:
      return BusType::APB1;
    case PeripheralClock::USART3_PER:
      return BusType::APB1;

    case PeripheralClock::I2C1_PER:
      return BusType::APB1;
    case PeripheralClock::I2C2_PER:
      return BusType::APB1;

    case PeripheralClock::SPI1_PER:
      return BusType::APB2;
    case PeripheralClock::SPI2_PER:
      return BusType::APB1;

    default:
      return BusType::AHB;
  }
}
;

void Rcc::peripheralClockEnable(PeripheralClock peripheral)
{
  auto *reg = ClockRegMapAt(peripheral);
  auto mask = ClockBitMapAt(peripheral);
  SET_BIT(*reg, mask);
}

void Rcc::peripheralClockDisable(PeripheralClock peripheral)
{
  auto *reg = ClockRegMapAt(peripheral);
  auto mask = ClockBitMapAt(peripheral);
  CLEAR_BIT(*reg, mask);
}

void Rcc::setSystemClockValue(uint32_t clockValue)
{
  systemClock = clockValue;
}

uint32_t Rcc::getSystemClockValue(void)
{
  return systemClock;
}

uint32_t getAPBDivider(uint32_t regValue)
{
  uint32_t divider = 1;

  switch (regValue)
  {
    case 0b100:
      divider = 2;
    break;
    case 0b101:
      divider = 4;
    break;
    case 0b110:
      divider = 8;
    break;
    case 0b111:
      divider = 16;
    break;
  }
  return divider;
}

uint32_t Rcc::getAPB1ClockDivider(void)
{
  uint32_t value = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;
  return getAPBDivider(value);
}

uint32_t Rcc::getAPB2ClockDivider(void)
{
  uint32_t value = (RCC->CFGR & RCC_CFGR_PPRE2) >> 13;
  return getAPBDivider(value);
}

static const uint32_t AHBClockDividerTable[] = { 1, 2, 4, 8, 16, 64, 128, 256, 512 };

uint32_t Rcc::getAHBClockDivider(void)
{
  uint32_t value = (RCC->CFGR & RCC_CFGR_HPRE) >> 4;
  if (value != 0)
    value -= 7; 		// zaleznosc z dokumantacji

  return AHBClockDividerTable[value];
}

BusType Rcc::getBusForPeripheral(PeripheralClock peripheral)
{
  return busMapAt(peripheral);
}

uint32_t Rcc::getBusSpeed(BusType bus)
{
  uint32_t divider;
  uint32_t systemClock = getSystemClockValue();
  systemClock /= getAHBClockDivider();

  switch (bus)
  {
    case BusType::APB1:
      divider = getAPB1ClockDivider();
    break;

    case BusType::APB2:
      divider = getAPB2ClockDivider();
    break;

    case BusType::AHB:
      divider = 1;
    break;
    default:
      divider = 1;
  }
  return (systemClock / divider);
}

uint32_t Rcc::getBusSpeed(PeripheralClock peripheral)
{
  switch (peripheral)
  {
    case PeripheralClock::TIM1_PER:
      return (Rcc::getAPB2ClockDivider() == 1) ? Rcc::getBusSpeed(BusType::APB2) : (Rcc::getBusSpeed(BusType::APB2) * 2);

    case PeripheralClock::TIM2_PER:
    case PeripheralClock::TIM3_PER:
    case PeripheralClock::TIM4_PER:
      return (Rcc::getAPB1ClockDivider() == 1) ? Rcc::getBusSpeed(BusType::APB1) : (Rcc::getBusSpeed(BusType::APB1) * 2);
    break;

    case PeripheralClock::ADC1_PER:
    case PeripheralClock::ADC2_PER:
      return Rcc::getBusSpeed(BusType::APB2) / 2;

    case PeripheralClock::RTC_PER:

    case PeripheralClock::USART1_PER:
    case PeripheralClock::USART2_PER:
    case PeripheralClock::USART3_PER:

    case PeripheralClock::I2C1_PER:
    case PeripheralClock::I2C2_PER:

    case PeripheralClock::SPI1_PER:
    case PeripheralClock::SPI2_PER:

    case PeripheralClock::GPIOA_PER:
    case PeripheralClock::GPIOB_PER:
    case PeripheralClock::GPIOC_PER:
      return Rcc::getBusSpeed(Rcc::getBusForPeripheral(peripheral));

    default:
      return 0;

  }

}

