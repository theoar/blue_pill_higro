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

  volatile uint32_t * BackupDomain::dataPointer(uint16_t index)
  {
    if(index>= 0 && index<=9 )
      return (&BKP->DR1)+index;
    else
      return nullptr;
  }

  uint16_t BackupDomain::readRegister(uint16_t index)
  {
    volatile uint32_t * ptr = BackupDomain::dataPointer(index);
    if(ptr!=0)
      return (uint16_t)((*BackupDomain::dataPointer(index))&0xffff);
    else
      return 0;
  }

  uint16_t BackupDomain::readRegister(uint16_t index, bool *result)
  {
    volatile uint32_t * ptr = BackupDomain::dataPointer(index);

    *result = ptr != nullptr;

    if(result)
      return (uint16_t)((*BackupDomain::dataPointer(index))&0xffff);
    else
      return 0;
  }

  void BackupDomain::readRegisters(uint16_t *destination, uint16_t index, uint16_t count)
  {
    for(uint16_t x = 0; x<count; ++x)
      destination[x] = BackupDomain::readRegister(index+1+x);
  }

  void BackupDomain::writeRegister(uint16_t index, uint16_t data)
  {
    volatile uint32_t * ptr= BackupDomain::dataPointer(index);
    if(ptr!=nullptr)
      *ptr=data;
  }

  void BackupDomain::writeRegister(uint16_t index, uint16_t data, bool *result)
  {
    volatile uint32_t * ptr= BackupDomain::dataPointer(index);
    *result =  ptr!=nullptr;
    if(result)
      *ptr=data;
  }

  void BackupDomain::writeRegisters(uint16_t *source, uint16_t index, uint16_t count)
  {
    for(uint16_t x = 0; x<count; ++x)
      BackupDomain::writeRegister(x+1+index, source[x]);
  }
}
