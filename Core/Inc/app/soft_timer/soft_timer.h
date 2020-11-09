/*
 * soft_timer.h
 *
 *  Created on: Oct 26, 2020
 *      Author: theoar
 */

#ifndef INC_SOFT_TIMER_H_
#define INC_SOFT_TIMER_H_

#include <inttypes.h>
#include "global_timer.h"

class SoftTimer
{
  private:
    uint64_t startTime = 0;
    uint64_t endTime = 0;
    uint64_t ms = 0;
    bool hold = true;
  public:
    SoftTimer() = default;
    SoftTimer(uint64_t ms)
    {
      this->start(ms);
    }
    void start(uint64_t ms)
    {
      this->hold = false;
      this->ms = ms;
      this->startTime = GlobalTimer::get ();
      if (this->startTime + ms < this->startTime) //overflow
	this->endTime = ms - (GlobalTimer::max () - this->startTime);
      else
	this->endTime = this->startTime + ms;
    }

    void stop()
    {
      this->hold = true;
    }

    bool check()
    {
      if (this->hold)
	return false;

      if (this->endTime >= this->startTime)
	return GlobalTimer::get () >= this->endTime;
      else
	//overflow
	return this->endTime >= GlobalTimer::get ();
    }

    bool checkAndRestart()
    {
      if(this->check())
      {
	this->restart();
	return true;
      }
      else
	return false;
    }

    void restart()
    {
      this->start (this->ms);
    }

    bool isStopped()
    {
      return this->hold;
    }
};

#endif /* INC_SOFT_TIMER_H_ */
