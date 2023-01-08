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
#include <functional>

using namespace bkp;

namespace bck_item
{
  template<typename T>
  class BackupItem
   {
     public:
      struct Data
      {
	T _min = 0;
	T _max = 0xffff;
	T _def = 0;
      };

     private:
      Data data;
      uint16_t index = 0;

     std::function<void (BackupItem *)> onValueChanged;

     public:
     using DataType = T;
     BackupItem() = default;

      template<typename Callable>
      void observe(Callable callable)
      {
	 onValueChanged = callable;
      }


      BackupItem(T _index) :
           index(_index)
       {
       }

       BackupItem(uint16_t __index, T __min, T __max, T __def)
       {
         this->index = __index;
         this->update(__min, __max, __def);

       }

       BackupItem(uint16_t __index, const Data & __data)
       {
         this->index = __index;
         this->update(__data);

       }

       void update(const Data & __data)
       {
	 this->data =__data;
	 if(this->valueNotInRange(   this->data._min,    this->data._max, this->get()))
	   this->set(this->data._def);
       }

       T min(void) const
       {
         return this->data._min;
       }

       T max(void) const
       {
         return this->data._max;
       }

       T get(void)
       {
        return this->get(nullptr);
       }

       T get(bool *result)
       {
	 bool myresult{false};
	 if(result==nullptr)
	   result = &myresult;

	 auto val = static_cast<T>(BackupDomain::readRegister(this->index, result));

	  if(*result==false || this->valueNotInRange(this->data._min, this->data._max, val))
	    return this->data._def;
	  else
	    return val;
       }

       void set(T val)
       {
         if(this->get()!=val)
         {
           BackupDomain::writeRegister(this->index, val);
           if(this->onValueChanged!=nullptr)
             this->onValueChanged(this);
         }
       }



       static inline bool valueNotInRange(T min, T max, T current)
       {
	 return (current < min) || (current > max);
       }

       void restoreDefault()
       {
	 this->set(this->data._def);
       }
  };

  using BackupItemUnsigned = BackupItem<uint32_t>;
  using BackupItemSigned = BackupItem<int32_t>;

  class BackupItemBuilder
  {
    private:
      static uint8_t currentIndex;
      BackupItemBuilder() {};
      BackupItemBuilder(const BackupItemBuilder &other) = delete;
    public:
      template<typename T>
      static T next(typename T::DataType min, typename T::DataType max, typename T::DataType def)
      {
	 return T{currentIndex++, min, max, def};
      }

      template<typename T>
      static T next(const typename T::Data & data)
      {
	 return T{currentIndex++, data};
      }

      template<typename T>
      static T next()
      {
	 return T{currentIndex++};
      }
  };


}



#endif /* INC_BCK_ITEM_BCK_ITEM_H_ */
