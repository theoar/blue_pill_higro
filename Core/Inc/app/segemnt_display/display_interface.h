/*
 * display_interface.h
 *
 *  Created on: 6 gru 2020
 *      Author: theoar
 */

#ifndef INC_APP_SEGEMNT_DISPLAY_DISPLAY_INTERFACE_H_
#define INC_APP_SEGEMNT_DISPLAY_DISPLAY_INTERFACE_H_

#include <inttypes.h>


namespace segment_display
{
  class SegmentDisplayInterface
  {
    public:
      virtual void display(uint32_t number) = 0;
      virtual void display(const char *str) = 0;
      virtual void clear() = 0;
  };
}


#endif /* INC_APP_SEGEMNT_DISPLAY_DISPLAY_INTERFACE_H_ */
