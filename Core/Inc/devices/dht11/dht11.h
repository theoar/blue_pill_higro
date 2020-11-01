/*
 * dht11.h
 *
 *  Created on: Oct 31, 2020
 *      Author: theoar
 */

#ifndef INC_DEVICES_DHT11_DHT11_H_
#define INC_DEVICES_DHT11_DHT11_H_

#include <memory.h>

#include "app/soft_timer/soft_timer.h"
#include "app/daemon_ctrl/daemon_interface.h"

#include "peripherials/rcc/rcc.h"
#include "peripherials/rcc/rcc_interface.h"
#include "peripherials/rcc/rcc_defs.h"

#include "peripherials/timer/timer_defs.h"
#include "peripherials/timer/timer_events.h"
#include "peripherials/gpio/gpio.h"

using namespace timer;
using namespace gpio;
using namespace daemon_ctrl;

namespace devices
{
  class Dht11 : public IInputCompareEvent, public IDaemon
  {
    private:
      enum class MachineSt
      {
	Idle, PreparingStart, WaitingForStart, Transmitting, Recieved, StopTr, Wait
      } machineSt = MachineSt::Idle;

      SoftTimer timer;

      static constexpr uint32_t waitTms = 200;
      static constexpr uint32_t powerWatiTms = 2000;
      static constexpr uint32_t maxTransmissionTms = 75;
      static constexpr uint32_t preparationTms = 25;

      static constexpr uint32_t ackTuS = 175;
      static constexpr uint32_t highTuS = 127;
      static constexpr uint32_t lowTuS = 80;

      static constexpr uint8_t treshold = 15;

      uint32_t humidity = 0;
      uint32_t temperature = 0;


      static constexpr uint32_t noResponseLimit = 5;
      uint32_t noResponseCnt = 0;


      uint32_t trPin = 0;
      uint32_t timChannel = 0;
      TimerDefs::TimerNr timerNr;
      GpioTypes::GpioNr gpioNr;
      GPIO_TypeDef *hwGpio = nullptr;
      TIM_TypeDef *hwTimer = nullptr;

      static constexpr uint32_t FRAME_CRC_SIZE = 1;
      static constexpr uint32_t FRAME_DATA_SIZE = 4;
      static constexpr uint32_t FRAME_SIZE = FRAME_CRC_SIZE+FRAME_DATA_SIZE;

      static constexpr uint32_t FRAME_START_BITS = 1;
      static constexpr uint32_t STAMP_DESIRED_COUNT = FRAME_SIZE*8+FRAME_START_BITS+1;
      uint32_t timeStamps[STAMP_DESIRED_COUNT];
      uint8_t stampIndex = 0;

      bool resultReady = false;
      bool trError = false;

      void setGpioAsInput();
      void setGpioAsOutput();
      void setPin();
      void resetPin();
      void clearCaptureChannelValue();
      void clearCounter();
      void enableCaptureIT();
      void disableCaptureIT();
      void enableCapture();
      void disableCapture();
      void startTimer();
      void stopTimer();
      void prepareResult();
      uint32_t getCaptureValue();

      void pushStamp(uint32_t stamp);

      void initGpio();
      void initTimer();
      void onError();

    public:
      void init(GpioTypes::GpioNr gpio, uint32_t pin, TimerDefs::TimerNr timerNr, uint32_t timChannel);

      bool isError();
      bool isBusy();
      bool isIdle();
      bool isResultReady();
      bool read();
      uint32_t getHumidity();
      uint32_t getTemperature();

      //IDaemon--------------------------------------------------------------------------------
      void handler();
      //---------------------------------------------------------------------------------------

      //ICompareChannelEvent-------------------------------------------------------------------
      void captureEvent(uint32_t status) override;
      //---------------------------------------------------------------------------------------
  };

}

#endif /* INC_DEVICES_DHT11_DHT11_H_ */
