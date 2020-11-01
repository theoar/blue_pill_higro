/*
 * gpio_defs.h
 *
 *  Created on: Oct 31, 2020
 *      Author: theoar
 */

#ifndef INC_PERIPHERIALS_GPIO_GPIO_DEFS_H_
#define INC_PERIPHERIALS_GPIO_GPIO_DEFS_H_

namespace gpio
{
  class GpioTypes
  {
    public:
      enum class GpioNr
      {
	  A,
	  B,
	  C
      };
  };
}



#endif /* INC_PERIPHERIALS_GPIO_GPIO_DEFS_H_ */
