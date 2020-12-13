/*
 * bkp.h
 *
 *  Created on: 26 lis 2020
 *      Author: theoar
 */

#ifndef INC_PERIPHERIALS_BKP_BKP_H_
#define INC_PERIPHERIALS_BKP_BKP_H_

#include <inttypes.h>


namespace bkp
{
  class BackupDomain
  {
    private:
      volatile static uint32_t * dataPointer(uint16_t index);
    public:
      static void init();
      static uint16_t readRegister(uint16_t index);
      static uint16_t readRegister(uint16_t index, bool *result);

      static void writeRegister(uint16_t index, uint16_t data);
      static void writeRegister(uint16_t index, uint16_t data, bool *result);

      static void readRegisters(uint16_t *destination, uint16_t index, uint16_t count);
      static void writeRegisters(uint16_t *source, uint16_t index, uint16_t count);

  };
}


#endif /* INC_PERIPHERIALS_BKP_BKP_H_ */
