#include "peripherials/rtc/rtc.h"

#include "proc.h"

#include <inttypes.h>

namespace rtc
{
  const uint8_t Rtc::DayOfMonth[] =
    { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  void Rtc::epochToTmTime(uint32_t ntpSec, TimeStruct *pSysTimeS)
  {
    NTPtime ntpTmS{0};

    epochToNtp(ntpSec, &ntpTmS);
    ntpDst(&ntpTmS);

    pSysTimeS->Year = ntpTmS.year;
    pSysTimeS->Month = ntpTmS.month;
    pSysTimeS->Day = ntpTmS.day;
    pSysTimeS->Hour = ntpTmS.hour;
    pSysTimeS->Minute = ntpTmS.minute;
    pSysTimeS->Second = ntpTmS.second;
    pSysTimeS->Weekday = ntpTmS.weekday + 1;

    if(ntpTmS.dst)
      pSysTimeS->flags |= F_TIME_DST;
    else
      pSysTimeS->flags &= ~F_TIME_DST;
  }

  uint32_t Rtc::tmTimeToEpoch(const TimeStruct *pSysTimeS)
  {
    NTPtime ntpTmS{0};
    uint32_t result = 0;

    ntpTmS.year = pSysTimeS->Year;
    ntpTmS.month = pSysTimeS->Month;
    ntpTmS.day = pSysTimeS->Day;
    ntpTmS.hour = pSysTimeS->Hour;
    ntpTmS.minute = pSysTimeS->Minute;
    ntpTmS.second = pSysTimeS->Second;
    ntpTmS.weekday = (uint8_t) pSysTimeS->Weekday - 1;
    ntpTmS.dst = tmTimeDst(pSysTimeS);

    result = ntpToEpoch(&ntpTmS);

    return result;
  }

  uint8_t Rtc::tmTimeDst(const TimeStruct *pSysTimeS)
  {
    uint8_t weekday, month;
    signed char hour, day;

    hour = pSysTimeS->Hour;
    day = pSysTimeS->Day;
    weekday = (uint8_t) pSysTimeS->Weekday - 1;
    month = pSysTimeS->Month;

    if((month < 3) || (month > 10))                     // miesiace 1, 2, 11, 12
    {
      return 0;                                      // -> Zima
    }
    else if((month > 3) || (month < 10))               // miesiace 4-9
    {
      return 1;                                      // -> Lato - DST=1
    }
    else
    {
      /* Przeliczenie na UTC */
      hour -= NTP_TZ;
      if(hour < 0)
      {
	hour = 24 - hour;
	if(day == 1)
	{
	  day = DayOfMonth[month - 1]; //blad dla 1 marca roku nie przestepnego (ale to niesistotne)
	  if((--month) < 3)
	  {
	    return 0;                            //spadamy do lutego (zima)
	  }
	}
	else
	{
	  day--;
	}
	if(weekday > 0)
	{
	  weekday--;
	}
	else
	{
	  weekday = 6;
	}
      }

      if(((day - weekday) >= 25) && (weekday || (hour >= 2))) // ostatnia niedziela lub po, godz >= 2.00
      {
	if(month == 10)	// pazdziernik -> zima (zmiana 1:00 UTC -> 0:00 UTC)
	{                            // w PL z 3.00 na 2.00, bo latem jest UTC+2
	  return 0;
	}
	else if(month == 3)      // marzec -> lato (zmiana 1:00 UTC -> 2:00 UTC)
	{                                // w PL z 2.00 na 3.00 - UTC+1 -> UTC+2
	  return 1;                                           // -> Lato - DST=1
	}

      }
    }

    return 0;
  }

  void Rtc::ntpDst(NTPtime *tm)
  {
    uint8_t hour, day, weekday, month;            // locals for faster access

    hour = tm->hour;
    day = tm->day;
    weekday = tm->weekday;
    month = tm->month;

    if((month < 3) || (month > 10))                       // month 1, 2, 11, 12
    {
      return;                                          // -> Winter
    }
    // after last Sunday 2:00
    if((day - weekday >= 25) && (weekday || (hour >= 2)))
    {
      if(month == 10)		                            // October -> Winter
      {
	return;
      }
    }
    else
    {			                              // before last Sunday 2:00
      if(month == 3)		                          // March -> Winter
      {
	return;
      }
    }

    // increment all affected parts
    hour++;				                         // add one hour
    if(hour == 24)
    {		                                              // next day
      hour = 0;
      weekday++;				                 // next weekday
      if(weekday == 7)
      {
	weekday = 0;
      }
      if(day == DayOfMonth[month - 1])                   // next month
      {
	day = 0;
	month++;
      }
      day++;
    }

    // write updated values
    tm->dst = 1;
    tm->month = month;
    tm->hour = hour;
    tm->day = day;
    tm->weekday = weekday;
  }

  void Rtc::epochToNtp(uint32_t sec, NTPtime *tm)
  {
    uint16_t day;
    uint8_t year;
    uint16_t dayofyear;
    uint8_t leap400;
    uint8_t month;

    // adjust timezone
    sec += ONE_HOUR * NTP_TZ;

    tm->second = sec % 60;
    sec /= 60;
    tm->minute = sec % 60;
    sec /= 60;
    tm->hour = sec % 24;
    day = sec / 24;

    tm->weekday = (day + FIRSTDAY) % 7;		// weekday

    year = FIRSTYEAR % 100;			// 0..99
    leap400 = 4 - (((FIRSTYEAR - 1) / 100) & 3);	// 4, 3, 2, 1

    for (;;)
    {
      dayofyear = 365;
      if((year & 3) == 0)
      {
	dayofyear = 366;					// leap year
	if(year == 0 || year == 100 || year == 200)	// 100 year exception
	  if(--leap400)					// 400 year exception
	    dayofyear = 365;
      }
      if(day < dayofyear)
	break;
      day -= dayofyear;
      year++;					// 00..136 / 99..235
    }
    tm->year = year + FIRSTYEAR / 100 * 100;	// + century

    if((dayofyear & 1) && (day > 58))		// no leap year and after 28.2.
      day++;					            // skip 29.2.

    for (month = 1; day >= DayOfMonth[month - 1]; month++)
    {
      day -= DayOfMonth[month - 1];
    }

    tm->month = month - 1;				// 1..12
    tm->day = day + 1;				    // 1..31
  }

  uint32_t Rtc::ntpToEpoch(const NTPtime *tm)
  {
    uint16_t year = tm->year;
    uint8_t month = tm->month;
    uint16_t days = (tm->day - 1);
    uint8_t hour = tm->hour;
    uint8_t minute = tm->minute;
    uint8_t second = tm->second;
    uint8_t dst = tm->dst;
    uint32_t result;
    uint16_t y;
    uint8_t leap_year = 0;
//        uint8_t  m;

    /* https://stackoverflow.com/a/11595914 */
    /* Is it leap year? */
    /* x % 2^n == x & (2^n - 1) */
    /*              [mod 4]                [mod 25]             [mod 16]          */
    leap_year =	(((year & 3) == 0) && (((year % 25) != 0) || ((year & 15) == 0)));

    for (y = FIRSTYEAR; y < year; y++)
    {
      days += 365;
      if(((y & 3) == 0) && (((y % 25) != 0) || ((y & 15) == 0)))
      {
	days += 1;
      }
    }

    do
    {
      days += DayOfMonth[month - 1];
      if((month - 1) == 2)
      {
	if(leap_year == 0)
	{
	  days -= 1;	//if not leap year then february has 28 days
	}
      }
      month--;
    }
    while (month);

    result = (uint32_t) days * ONE_DAY;
    result += (uint32_t) hour * ONE_HOUR;
    result += (uint32_t) minute * 60;
    result += second;

    if(dst)
    {
      result -= ONE_HOUR;
    }

    // adjust timezone
    result -= ONE_HOUR * NTP_TZ;

    return result;
  }

  bool Rtc::isWorking(void)
  {
    if((RCC->BDCR & RCC_BDCR_RTCEN) == 0)
      return false;
    return true;
  }

  void Rtc::init(void)
  {
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_BKPEN);
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    SET_BIT(PWR->CR, PWR_CR_DBP);

    //SET_BIT(RCC->CSR, RCC_CSR_LSION);
    //while (!(RCC->CSR & RCC_CSR_LSIRDY));

    SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);
    while (!(RCC->BDCR & RCC_BDCR_LSERDY));

    SET_BIT(RCC->BDCR, RCC_BDCR_RTCSEL_LSE);
    SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN);

    while(!(RTC->CRL & RTC_CRL_RSF));

    while(!(RTC->CRL & RTC_CRL_RTOFF));

    SET_BIT(RTC->CRL, RTC_CRL_CNF);
    RTC->PRLL = 32767;
    RTC->PRLH = 0x00;
    CLEAR_BIT(RTC->CRL, RTC_CRL_CNF);

    while(!(RTC->CRL & RTC_CRL_RTOFF));
  }

  void Rtc::setEpochTime(uint32_t UNIXtime)
  {
    while(!(RTC->CRL & RTC_CRL_RTOFF));

    SET_BIT(RTC->CRL, RTC_CRL_CNF);
    RTC->CNTL = UNIXtime & 0xffff;
    RTC->CNTH = (UNIXtime >> 16) & 0xffff;
    CLEAR_BIT(RTC->CRL, RTC_CRL_CNF);

    while(!(RTC->CRL & RTC_CRL_RTOFF));
 }

  void Rtc::setTime(TimeStruct time)
  {
    Rtc::setEpochTime(Rtc::tmTimeToEpoch(&time));
  }

  void Rtc::setTmTime(TimeStruct *timeStruct)
  {
    Rtc::setEpochTime(Rtc::tmTimeToEpoch(timeStruct));
  }


  uint32_t Rtc::epoch()
  {
    uint32_t currentTime  = ((uint32_t)(RTC->CNTH &0xffff) << 16) | ((uint32_t)RTC->CNTL & 0xffff);
    uint32_t tempTime = currentTime;
    do
    {
      currentTime = tempTime;
      tempTime = ((uint32_t)(RTC->CNTH &0xffff) << 16) | ((uint32_t)RTC->CNTL & 0xffff);
    } while(currentTime!=tempTime);

    return currentTime;
  }

  TimeStruct Rtc::time()
  {
    TimeStruct timeSt;
    uint32_t epoch = Rtc::epoch();
    Rtc::epochToTmTime(epoch, &timeSt);

    return timeSt;
  }

  uint32_t Rtc::msc()
  {
    uint32_t val = 0;
    val = RTC->DIVL;
    val |= (RTC->DIVH & 0x0f)<<16;

    val/=32;

    return val;

  }
}

