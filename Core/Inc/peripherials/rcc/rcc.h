#ifndef RCC_H_
#define RCC_H_

#include <inttypes.h>
#include "rcc_interface.h"
#include "rcc_defs.h"

namespace rcc
{

  class Rcc // : public InterfaceRCC
  {
    private:
      static uint32_t systemClock;

      Rcc()
      {
      }
      ;

    public:
      static void init(uint32_t coreClock);
      static void peripheralClockEnable(PeripheralClock peripheral);
      static void peripheralClockDisable(PeripheralClock peripheral);
      static void setSystemClockValue(uint32_t clockValue);

      static BusType getBusForPeripheral(PeripheralClock peripheral);
      static uint32_t getBusSpeed(PeripheralClock peripheral);
      static uint32_t getBusSpeed(BusType bus);

      //void peripheralReset(PeripheralReset peripheral) override;
      static uint32_t getSystemClockValue(void);
      static uint32_t getAPB1ClockDivider(void);
      static uint32_t getAPB2ClockDivider(void);
      static uint32_t getAHBClockDivider(void);

      ~Rcc()
      {
      }
      ;
  };

}

#endif

