/*
 * global_timer.h
 *
 *  Created on: Oct 26, 2020
 *      Author: theoar
 */

#ifndef INC_GLOBAL_TIMER_H_
#define INC_GLOBAL_TIMER_H_

#include <inttypes.h>
#include <limits.h>

class GlobalTimer
{
  public:
    static void increment()
    {
      GlobalTimer::time++;
    }
    static uint64_t get()
    {
      return GlobalTimer::time;
    }
    static uint64_t max()
    {
      return UINT64_MAX;
    }
  private:
    static volatile uint64_t time;
};

#endif /* INC_GLOBAL_TIMER_H_ */
