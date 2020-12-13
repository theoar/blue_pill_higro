#ifndef RTC_H_
#define RTC_H_

#include <inttypes.h>

namespace rtc
{

  struct TimeStruct // rozmiar 2B
  {
      uint16_t Year;         // 2000-2099
      uint8_t Month;        // 1-12
      uint8_t Day;          // 1-31
      uint8_t Hour;         // 0-23
      uint8_t Minute;       // 0-59
      uint8_t Second;       // 0-59

      struct
      {
	  uint8_t flags :5;
	  uint8_t Weekday :3; //1-7
      };
  };

  struct NTPtime
  {
      // data
      uint16_t year;
      uint8_t month;
      uint8_t day;
      uint8_t weekday;
      // czas
      uint8_t hour;
      uint8_t minute;
      uint8_t second;

      uint8_t dst; // daylight savings time (+1 hour) czas zimowy
  };

  enum class Month : uint8_t
  {
    JANUARY = 1, //1
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER //112
  };
  class Rtc
  {
    private:

#define FIRSTYEAR           1900    // start year
#define FIRSTDAY            1	    // 1.1.1900 was a Monday (0 = Sunday)

#define DAYS_PER_4_YEARS    (365*4+1)

      /*
       Makra do przeliczania znacznika czasu Y2K (liczba sekund od 1.01.2000 UTC, implementacja avr-gcc)
       na unixowy (liczba sekund od 1.01.1970 UTC) i NTP (liczba sekund od 1.01.1900 UTC)
       */
#ifndef UNIX_OFFSET
#define UNIX_OFFSET         946684800UL
#endif
#ifndef NTP_OFFSET
#define NTP_OFFSET          3155673600UL
#endif
#ifndef ONE_HOUR
#define ONE_HOUR            3600
#endif
#ifndef ONE_DAY
#define ONE_DAY             86400
#endif
      /* Roznica miedzy czasem unixowym a NTP */
#define UNIX_NTP_OFFSET         (NTP_OFFSET - UNIX_OFFSET)

#define NTP_START_TIME          3781674655UL    //04.11.2019 10:10 GMT - NTP timestamp do walidacji czasu

#define NTP_TIME_IS_VALID(tm)   (tm >= NTP_START_TIME)

#define NTP_TZ                  +1              //strefa czasowa dla NTP

#define F_TIME_UPDATED          (1 << 0)
#define F_TIME_VALID            (1 << 3)
#define F_TIME_DST              (1 << 4)        // daylight savings time (+1 hour) czas zimowy

      // czas systemowy

      static const uint8_t DayOfMonth[13];

      static uint32_t ntpToEpoch(const NTPtime *tm);
      static void epochToNtp(uint32_t sec, NTPtime *tm);
      static void ntpDst(NTPtime *tm);
      static uint8_t tmTimeDst(const TimeStruct *pSysTimeS);

    public:
      static void init(void);

      static void setEpochTime(uint32_t time);
      static void setTime(TimeStruct time);
      static void setTmTime(TimeStruct *timeStruct);

      static bool isWorking();

      static uint32_t epoch();
      static TimeStruct time();

      static uint32_t msc();

      static void epochToTmTime(uint32_t ntpSec, TimeStruct *pSysTimeS);
      static uint32_t tmTimeToEpoch(const TimeStruct *pSysTimeS);

  };

}

#endif
