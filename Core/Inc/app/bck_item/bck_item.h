/*
 * bck_item.h
 *
 *  Created on: 7 gru 2020
 *      Author: theoar
 */

#ifndef INC_BCK_ITEM_BCK_ITEM_H_
#define INC_BCK_ITEM_BCK_ITEM_H_

#include "peripherials/bkp/bkp.h"

#include <inttypes.h>

using namespace bkp;

namespace bck_item
{

  class BackupItem
   {
     uint16_t index = 0;
     uint16_t _min = 0;
     uint16_t _max = 0xffff;
     uint16_t _def = 0;

     public:
     BackupItem() = default;
     BackupItem(uint16_t _index);
     BackupItem(uint16_t __index, uint16_t __min, uint16_t __max, uint16_t __def);

      uint16_t get(void);
      uint16_t get(bool *result);

      uint16_t min(void);
      uint16_t max(void);
      void set(uint16_t val);
      void validate(uint16_t min, uint16_t max, uint16_t def);
  };

  class BackupItemBuilder
  {
    private:
      static uint8_t currentIndex;
      BackupItemBuilder() {};
      BackupItemBuilder(const BackupItemBuilder &other) = delete;
    public:
      static BackupItem next(uint16_t min, uint16_t max, uint16_t def);
      static BackupItem next();
  };

}



#endif /* INC_BCK_ITEM_BCK_ITEM_H_ */
