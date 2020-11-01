/*
 * dht11.cpp
 *
 *  Created on: Oct 31, 2020
 *      Author: theoar
 */

#ifndef SRC_DEVICES_DHT11_DHT11_CPP_
#define SRC_DEVICES_DHT11_DHT11_CPP_

#include "devices/dht11/dht11.h"

#include "peripherials/rcc/rcc.h"

using namespace rcc;
using namespace timer;

namespace devices
{

  void Dht11::setGpioAsInput()
  {
    this->setPin();
    LL_GPIO_SetPinMode(this->hwGpio, this->trPin, LL_GPIO_MODE_FLOATING);
  }

  void Dht11::setGpioAsOutput()
  {
    this->setPin();
    LL_GPIO_SetPinMode(this->hwGpio, this->trPin, LL_GPIO_MODE_OUTPUT);
  }

  void Dht11::setPin()
  {
    LL_GPIO_SetOutputPin(this->hwGpio, this->trPin);
  }

  void Dht11::resetPin()
  {
    LL_GPIO_ResetOutputPin(this->hwGpio, this->trPin);
  }

  void Dht11::clearCaptureChannelValue()
  {
    switch (this->timChannel)
    {
      case LL_TIM_CHANNEL_CH1:
	LL_TIM_OC_SetCompareCH1(this->hwTimer, 0);
      break;
      case LL_TIM_CHANNEL_CH2:
	LL_TIM_OC_SetCompareCH2(this->hwTimer, 0);
      break;
      case LL_TIM_CHANNEL_CH3:
	LL_TIM_OC_SetCompareCH3(this->hwTimer, 0);
      break;
      case LL_TIM_CHANNEL_CH4:
	LL_TIM_OC_SetCompareCH4(this->hwTimer, 0);
      break;
      default:
	return;
    }
  }

  void Dht11::clearCounter()
  {
    LL_TIM_SetCounter(this->hwTimer, 0);
  }

  void Dht11::enableCaptureIT()
  {
    switch (this->timChannel)
    {
      case LL_TIM_CHANNEL_CH1:
	LL_TIM_EnableIT_CC1(this->hwTimer);
      break;
      case LL_TIM_CHANNEL_CH2:
	LL_TIM_EnableIT_CC2(this->hwTimer);
      break;
      case LL_TIM_CHANNEL_CH3:
	LL_TIM_EnableIT_CC3(this->hwTimer);
      break;
      case LL_TIM_CHANNEL_CH4:
	LL_TIM_EnableIT_CC4(this->hwTimer);
      break;
      default:
	return;
    }
  }

  void Dht11::disableCaptureIT()
  {
    switch (this->timChannel)
    {
      case LL_TIM_CHANNEL_CH1:
	LL_TIM_DisableIT_CC1(this->hwTimer);
      break;
      case LL_TIM_CHANNEL_CH2:
	LL_TIM_DisableIT_CC2(this->hwTimer);
      break;
      case LL_TIM_CHANNEL_CH3:
	LL_TIM_DisableIT_CC3(this->hwTimer);
      break;
      case LL_TIM_CHANNEL_CH4:
	LL_TIM_DisableIT_CC4(this->hwTimer);
      break;
      default:
	return;
    }
  }

  void Dht11::enableCapture()
  {
    LL_TIM_CC_EnableChannel(this->hwTimer, this->timChannel);
  }

  void Dht11::disableCapture()
  {
    LL_TIM_CC_DisableChannel(this->hwTimer, this->timChannel);
  }

  void Dht11::startTimer()
  {
    LL_TIM_EnableCounter(this->hwTimer);
  }

  void Dht11::stopTimer()
  {
    LL_TIM_DisableCounter(this->hwTimer);
  }

  void Dht11::initGpio()
  {
    Gpio::init(this->gpioNr);
    LL_GPIO_SetPinSpeed(this->hwGpio, this->trPin, LL_GPIO_SPEED_FREQ_HIGH);
    this->setGpioAsOutput();
  }

  uint32_t Dht11::getCaptureValue()
  {
    switch (this->timChannel)
    {
      case LL_TIM_CHANNEL_CH1:
	return LL_TIM_IC_GetCaptureCH1(this->hwTimer);
      break;
      case LL_TIM_CHANNEL_CH2:
	return LL_TIM_IC_GetCaptureCH2(this->hwTimer);
      break;
      case LL_TIM_CHANNEL_CH3:
	return LL_TIM_IC_GetCaptureCH3(this->hwTimer);
      break;
      case LL_TIM_CHANNEL_CH4:
	return LL_TIM_IC_GetCaptureCH4(this->hwTimer);
      break;
      default:
	return 0;
    }
  }

  void Dht11::pushStamp(uint32_t stamp)
  {
    this->timeStamps[this->stampIndex] = stamp;
    this->stampIndex++;

//    if(this->stampIndex>=STAMPS_SIZE)
//      this->machineSt=MachineSt::
  }

  void Dht11::initTimer()
  {
    //NVIC_SetPriority(TIM4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));

    uint32_t timerClock = 0;
    switch (this->timerNr)
    {
      case TimerDefs::TimerNr::Tim2:
	NVIC_EnableIRQ(TIM2_IRQn);
	Rcc::peripheralClockEnable(PeripheralClock::TIM2_PER);
	timerClock = Rcc::getBusSpeed(PeripheralClock::TIM2_PER);
      break;
      case TimerDefs::TimerNr::Tim3:
	NVIC_EnableIRQ(TIM3_IRQn);
	Rcc::peripheralClockEnable(PeripheralClock::TIM3_PER);
	timerClock = Rcc::getBusSpeed(PeripheralClock::TIM3_PER);
      break;
      case TimerDefs::TimerNr::Tim4:
	NVIC_EnableIRQ(TIM4_IRQn);
	Rcc::peripheralClockEnable(PeripheralClock::TIM4_PER);
	timerClock = Rcc::getBusSpeed(PeripheralClock::TIM4_PER);
      break;
      default:
	;
    }

    LL_TIM_InitTypeDef TIM_InitStruct = { 0 };

    TIM_InitStruct.Prescaler = timerClock / 1000000 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = UINT16_MAX;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(this->hwTimer, &TIM_InitStruct);
    LL_TIM_SetClockSource(this->hwTimer, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableMasterSlaveMode(this->hwTimer);

    LL_TIM_IC_SetActiveInput(this->hwTimer, this->timChannel, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(this->hwTimer, this->timChannel, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(this->hwTimer, this->timChannel, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(this->hwTimer, this->timChannel, LL_TIM_IC_POLARITY_FALLING);

    this->stopTimer();
    this->clearCounter();
    this->clearCaptureChannelValue();
  }

  void Dht11::onError()
  {
    this->trError = true;
    this->resultReady = false;

    this->noResponseCnt++;

    this->machineSt = MachineSt::StopTr;
  }

  void Dht11::init(GpioTypes::GpioNr gpio, uint32_t pin, TimerDefs::TimerNr timerNr, uint32_t timChannel)
  {
    this->trPin = pin;
    this->gpioNr = gpio;
    this->timChannel = timChannel;
    this->timerNr = timerNr;
    this->hwTimer = TimerDefs::getTimer(this->timerNr);
    this->hwGpio = Gpio::getGpio(this->gpioNr);

    this->initGpio();
    this->initTimer();

    TimerEvents::getGeneralPurposeTimerEvents(this->timerNr)->writeCaptureEvent(this);

    this->timer.start(this->powerWatiTms);
    this->machineSt = MachineSt::Wait;
  }

  void Dht11::captureEvent(uint32_t status)
  {
    switch (this->machineSt)
    {
      case MachineSt::WaitingForStart:
      {
	this->machineSt = MachineSt::Transmitting;
	this->pushStamp(this->getCaptureValue());
      }
      break;

      case MachineSt::Transmitting:
      {
	this->pushStamp(this->getCaptureValue());
      }
      break;

      default:
	;
    }
  }

  void Dht11::prepareResult()
  {
    uint32_t diff = 0;
    uint32_t bitNo = 0;
    uint32_t byteNo = 0;
    uint32_t crc = 0;
    bool ackFine = false;
    bool bitFine = false;
    bool bitValue = false;
    bool crcFine = false;
    bool framingError = false;
    uint8_t byte = 0;

    uint8_t frame[FRAME_SIZE];
    for(uint32_t x = 1; (x<STAMP_DESIRED_COUNT) && (framingError==false); ++x)
    {
      bitFine = false;

      diff = this->timeStamps[x]-this->timeStamps[x-1];

      if(diff > (this->ackTuS - this->treshold) && diff < (this->ackTuS + this->treshold))
      {
	ackFine = true;
      }
      else
      {
	if (diff > (this->highTuS - this->treshold) && diff < (this->highTuS + this->treshold))
	{
	  bitFine = true;
	  bitValue = true;
	}
	else
	{
	    if((diff > this->lowTuS - this->treshold) && diff < (this->lowTuS + this->treshold))
	    {
	      bitFine = true;
	      bitValue = false;
	    }
	    else
	    {
	      framingError = true;
	    }
	}
      }

      if(!framingError)
      {
	if(bitFine)
	{
	  if(bitValue)
	    byte |= (1<<(7-bitNo));

	  bitNo++;

	  if(bitNo==8)
	  {
	    frame[byteNo] = byte;
	    byteNo++;
	    bitNo = 0;
	    byte = 0;
	  }
	}
      }
    }

    for(uint8_t x = 0; x<FRAME_DATA_SIZE; ++x)
      crc+=frame[x];

    if((crc & 0xff)==frame[FRAME_DATA_SIZE])
      crcFine = true;

    if(framingError==false && ackFine==true && byteNo==FRAME_SIZE && crcFine==true)
    {
      this->resultReady = true;
      this->trError = false;
      this->humidity = frame[0]*100 + frame[1];
      this->temperature = frame[2]*100 + frame[3]*10;
    }
    else
    {
      this->resultReady = false;
      this->trError = true;
    }

  }

  void Dht11::handler()
  {
    switch (this->machineSt)
    {
      case MachineSt::Idle:
      break;

      case MachineSt::PreparingStart:
	if (this->timer.check())
	{
	  this->setGpioAsInput();

	  this->clearCaptureChannelValue();
	  this->clearCounter();

	  this->enableCaptureIT();
	  this->enableCapture();

	  this->timer.start(this->maxTransmissionTms);
	  this->machineSt = MachineSt::WaitingForStart;

	  this->startTimer();
	}
      break;

      case MachineSt::WaitingForStart:
	if (this->timer.check())
	  this->onError();
      break;

      case MachineSt::Transmitting:
	if (this->timer.check())
	  this->onError();
	else
	{
	  if (this->stampIndex == STAMP_DESIRED_COUNT)
	  {
	    this->machineSt = MachineSt::StopTr;
	    this->noResponseCnt = 0;
	    this->prepareResult();
	  }
	}
      break;

      case MachineSt::StopTr:
      {
	this->timer.stop();

	this->disableCapture();
	this->disableCaptureIT();

	this->stopTimer();
	this->setGpioAsOutput();

	if(this->noResponseCnt >= this->noResponseLimit)
	  this->timer.start(this->powerWatiTms);
	else
	  this->timer.start(this->waitTms);

	this->machineSt = MachineSt::Wait;
      }
      break;

      case MachineSt::Wait:
	if (this->timer.check())
	  this->machineSt = MachineSt::Idle;
      break;

      case MachineSt::Recieved:
	this->machineSt = MachineSt::StopTr;
	this->resultReady = true;
      break;

    }
  }

  bool Dht11::isError()
  {
    bool retVal = this->trError;
    this->trError = false;

    return retVal;
  }

  bool Dht11::isBusy()
  {
    return this->machineSt != MachineSt::Idle;
  }

  bool Dht11::isIdle()
  {
    return this->machineSt == MachineSt::Idle;
  }

  bool Dht11::isResultReady()
  {
    bool retVal = this->resultReady;
    this->resultReady = false;

    return retVal;
  }

  bool Dht11::read()
  {
    if (this->machineSt == MachineSt::Idle)
    {
      this->machineSt = MachineSt::PreparingStart;

      memset(this->timeStamps, 0, STAMP_DESIRED_COUNT);
      this->stampIndex = 0;

      this->setGpioAsOutput();
      this->resetPin();

      this->timer.start(this->preparationTms);

      return true;
    }
    else
    {
      return false;
    }
  }

  uint32_t Dht11::getHumidity()
  {
    return this->humidity;
  }
  uint32_t Dht11::getTemperature()
  {
    return this->temperature;
  }
}

#endif /* SRC_DEVICES_DHT11_DHT11_CPP_ */
