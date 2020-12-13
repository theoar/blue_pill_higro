/*
 * higrometer.h
 *
 *  Created on: 31 paź 2020
 *      Author: theoar
 */

#ifndef SRC_APP_HIGROMETER_HIGROMETER_H_
#define SRC_APP_HIGROMETER_HIGROMETER_H_

#include <inttypes.h>

#include "app/soft_timer/soft_timer.h"
#include "app/daemon_ctrl/daemon_ctrl.h"

#include "devices/dht11/dht11.h"

using namespace devices;
using namespace daemon_ctrl;

namespace higrometer
{
  class Higrometer : public IDaemon
  {
    private:
      uint32_t askTime = 1000 * 60;
      uint32_t inRownAskTime = 1000 * 2.5;

      static constexpr uint32_t timeThreshold = 20 * 1000;

      SoftTimer timer;

      enum class AskStep
      {
	FirstInit, Init, First, Second, Result
      } step;

      enum class MeasureType
      {
	Simple, Advanced
      } measureType;

      Dht11 *dht = nullptr;

      bool connected = false;

      uint32_t humidity;
      uint32_t temperature;

      void handlerSimple();
      void handlerAdvanced();

    public:
      void init(Dht11 *dht, uint32_t askTime);
      void setAskTime(uint32_t askTime);

      bool isConnected();
      uint32_t getTemperature();
      uint32_t getHumidity();

      uint32_t getTemperature(bool *result);
      uint32_t getHumidity(bool *result);

      //IDaemon----------------------------------------------------------
      void handler();
      //-----------------------------------------------------------------
  };
}

#endif /* SRC_APP_HIGROMETER_HIGROMETER_H_ */
