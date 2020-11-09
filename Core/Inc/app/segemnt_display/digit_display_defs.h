/*
 * segment_display_defs.h
 *
 *  Created on: 2 lis 2020
 *      Author: theoar
 */

#ifndef INC_APP_SEGEMNT_DISPLAY_DIGIT_DISPLAY_DEFS_H_
#define INC_APP_SEGEMNT_DISPLAY_DIGIT_DISPLAY_DEFS_H_

#include <inttypes.h>

namespace segment_display
{
  class DigitDisplayDefs
  {
    public:
      enum class Segments : uint8_t
      {
	A = 0, B, C, D, E, F, G, DOTP, COUNT = DOTP
      };
    private:
      static constexpr uint8_t getBits(Segments segment)
      {
	return 1 << static_cast<uint8_t>(segment);
      }

      template<typename ...Args>
      static constexpr uint8_t getBits(Segments segment, Args ...args)
      {
	return getBits(segment) | getBits(args...);
      }

    public:
      static uint8_t digit2SegmentMask(uint8_t digit)
      {
	digit = digit % 10;
	uint8_t digits[10];

	digits[0] = getBits(Segments::A, Segments::B, Segments::C, Segments::D, Segments::E, Segments::F),
	digits[1] = getBits(Segments::B, Segments::C);
	digits[2] = getBits(Segments::A, Segments::B, Segments::G, Segments::E, Segments::D);
	digits[3] = getBits(Segments::A, Segments::B, Segments::G, Segments::C, Segments::D);
	digits[4] = getBits(Segments::F, Segments::G, Segments::B, Segments::C);
	digits[5] = getBits(Segments::A, Segments::F, Segments::G, Segments::C, Segments::D);
	digits[6] = getBits(Segments::A, Segments::F, Segments::G, Segments::C, Segments::D, Segments::E);
	digits[7] = getBits(Segments::B, Segments::C, Segments::A);
	digits[8] = getBits(Segments::A, Segments::B, Segments::C, Segments::D, Segments::E, Segments::F, Segments::G);
	digits[9] = getBits(Segments::A, Segments::B, Segments::C, Segments::D, Segments::F, Segments::G);

	return digits[digit];
      }

      static uint8_t char2SegmentMask(char c)
      {
	switch(c)
	{
	  case 'a': return getBits(Segments::A, Segments::B, Segments::C, Segments::D, Segments::E, Segments::G);
	  case 'A': return getBits(Segments::A, Segments::B, Segments::C, Segments::E, Segments::F, Segments::G);
	  case 'c': return getBits(Segments::D, Segments::E, Segments::G);
	  case 'e': return getBits(Segments::A, Segments::B, Segments::D, Segments::E, Segments::F, Segments::G);
	  case 'C': return getBits(Segments::A, Segments::D, Segments::E, Segments::F);
	  case 'E': return getBits(Segments::A, Segments::D, Segments::E, Segments::F, Segments::G);
	  case 'P': return getBits(Segments::A, Segments::B, Segments::E, Segments::F);
	  case 'F': return getBits(Segments::A, Segments::E, Segments::F, Segments::G);
	  case 'd': return getBits(Segments::B, Segments::C, Segments::D, Segments::E, Segments::G);

	  case 'n': return getBits(Segments::C, Segments::E, Segments::G);
	  case 'o': return getBits(Segments::C, Segments::D, Segments::E, Segments::G);

	  case '1': return digit2SegmentMask(1);
	  case '2': return digit2SegmentMask(2);
	  case '3': return digit2SegmentMask(3);
	  case '4': return digit2SegmentMask(4);
	  case '5': return digit2SegmentMask(5);
	  case '6': return digit2SegmentMask(6);
	  case '7': return digit2SegmentMask(7);
	  case '8': return digit2SegmentMask(8);
	  case '9': return digit2SegmentMask(9);
	  case '0': return digit2SegmentMask(0);
	  case ' ': return 0;
	  default: return 0;
	}
      }

      static uint8_t segmentIndex(Segments segment)
      {
	return static_cast<uint8_t>(segment);
      }

      static constexpr uint8_t segmentCount() { return static_cast<uint8_t>(Segments::COUNT); }

  };
}

#endif /* INC_APP_SEGEMNT_DISPLAY_DIGIT_DISPLAY_DEFS_H_ */