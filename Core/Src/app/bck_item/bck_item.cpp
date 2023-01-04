/*
 * bck_item.cpp
 *
 *  Created on: 7 gru 2020
 *      Author: theoar
 */

#ifndef SRC_APP_BCK_ITEM_BCK_ITEM_CPP_
#define SRC_APP_BCK_ITEM_BCK_ITEM_CPP_

#include "app/bck_item/bck_item.h"

namespace bck_item
{
  BackupItem::BackupItem(uint16_t _index) :
      index(_index)
  {
  }

  BackupItem::BackupItem(uint16_t __index, uint16_t __min, uint16_t __max, uint16_t __def)
  {
    this->index = __index;
    this->_min = __min;
    this->_max = __max;
    this->_def = __def;

    this->validate(this->_min, this->_max, this->_def);
  }

  uint16_t BackupItem::min(void)
  {
    return this->_min;
  }

  uint16_t BackupItem::max(void)
  {
    return this->_max;
  }


  uint16_t BackupItem::get(void)
  {
    bool result = false;

    uint16_t val =  BackupDomain::readRegister(this->index, &result);

    if(result==false)
      return this->min();
    else
      return val;
  }

  uint16_t BackupItem::get(bool *result)
  {
   return BackupDomain::readRegister(this->index, result);
  }

  void BackupItem::set(uint16_t val)
  {
    if(this->get()!=val)
    {
      this->modified=true;
      BackupDomain::writeRegister(this->index, val);
    }
  }

  bool BackupItem::wasModified()
  {
    bool ret = this->modified;
    this->modified = false;
    return ret;
  }

  void BackupItem::validate(uint16_t min, uint16_t max, uint16_t def)
  {
    uint16_t current = this->get();

    if(current < min || current > max)
      this->set(def);
  }

  uint8_t BackupItemBuilder::currentIndex = 0;

  BackupItem BackupItemBuilder::next(uint16_t min, uint16_t max, uint16_t def)
  {
    BackupItem toRet = BackupItem(currentIndex++, min, max, def);
    return toRet;
  }

  BackupItem BackupItemBuilder::next()
  {
    BackupItem toRet = BackupItem(currentIndex++);
    return toRet;
  }

}


#endif /* SRC_APP_BCK_ITEM_BCK_ITEM_CPP_ */
