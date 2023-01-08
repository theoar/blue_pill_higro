/*
 * bkp.cpp
 *
 *  Created on: 26 lis 2020
 *      Author: theoar
 */

#include "peripherials/bkp/bkp.h"
#include "proc.h"


namespace bkp
{
  void BackupDomain::init()
  {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_BKP);
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    LL_PWR_EnableBkUpAccess();
  }

  uint32_t BackupDomain::readRegister(uint16_t index, bool *result)
  {
    uint32_t value{0};
    bool status{false};
    if(index+LL_RTC_BKP_DR1>RTC_BKP_NUMBER)
    {
      status = false;
      value = 0;
    }
    else
    {
      status = true;
      value = LL_RTC_BKP_GetRegister(BKP, index+LL_RTC_BKP_DR1);
    }

    if(result)
      *result = status;

    return value;
  }

  void BackupDomain::readRegisters(uint32_t *destination, uint16_t index, uint16_t count)
  {
    for(uint16_t x = 0; x<count; ++x)
      destination[x] = BackupDomain::readRegister(index+x);
  }

  void BackupDomain::writeRegister(uint16_t index, uint32_t data, bool *result)
  {
    bool status{false};
    if(index+LL_RTC_BKP_DR1>RTC_BKP_NUMBER)
    {
      status = false;
    }
    else
    {
      status = true;
      LL_RTC_BKP_SetRegister(BKP, index+LL_RTC_BKP_DR1, data);
    }

    if(result)
      *result = status;
  }

  void BackupDomain::writeRegisters(uint32_t *source, uint16_t index, uint16_t count)
  {
    for(uint16_t x = 0; x<count; ++x)
      BackupDomain::writeRegister(x+index, source[x]);
  }
}
