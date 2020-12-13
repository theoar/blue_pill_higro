/*
 * blink_timer.h
 *
 *  Created on: 3 gru 2020
 *      Author: theoar
 */

#ifndef INC_APP_SOFT_TIMER_BLINK_TIMER_H_
#define INC_APP_SOFT_TIMER_BLINK_TIMER_H_

#include <inttypes.h>

class BlinkTimer
{
private:
    uint64_t startTime = 0;
    uint64_t endTime = 0;

    static constexpr uint8_t NumOfPos = 2;

    uint64_t ms[NumOfPos];

    bool hold = true;

    uint8_t cnt = 0;

    void start(uint64_t ms)
    {
      this->hold = false;
      this->startTime = GlobalTimer::get();
      if(this->startTime + ms < this->startTime) //overflow
	this->endTime = ms - (GlobalTimer::max() - this->startTime);
      else
	this->endTime = this->startTime + ms;
    }

  public:
    BlinkTimer() = default;
    BlinkTimer(uint64_t ms1, uint64_t ms2)
    {
      this->start(ms1, ms2);
    }

    void start(uint64_t ms1, uint64_t ms2)
    {
      this->start(ms1);

      this->ms[0] = ms1;
      this->ms[1] = ms2;
    }

    void stop()
    {
      this->hold = true;
    }

    int checkCurrentState()
    {
      return this->cnt;
    }

    uint8_t checkCurrentAndRestart()
    {
      if(this->check())
      {
	this->cnt++;
	this->cnt %=2;

	this->start(this->ms[this->cnt]);
      }

      return this->cnt;
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


    void restart()
    {
      this->cnt = 0;
      this->start(this->ms[this->cnt]);
    }

    bool isStopped()
    {
      return this->hold;
    }
};



#endif /* INC_APP_SOFT_TIMER_BLINK_TIMER_H_ */
