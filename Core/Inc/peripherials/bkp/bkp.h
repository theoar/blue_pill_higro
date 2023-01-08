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
    public:
      static void init();
      static uint32_t readRegister(uint16_t index, bool *result = nullptr);
      static void writeRegister(uint16_t index, uint32_t data, bool *result = nullptr) ;

      static void readRegisters(uint32_t *destination, uint16_t index, uint16_t count);
      static void writeRegisters(uint32_t *source, uint16_t index, uint16_t count);

  };
}


#endif /* INC_PERIPHERIALS_BKP_BKP_H_ */
