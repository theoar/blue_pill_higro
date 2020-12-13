/*
 * data_format.h
 *
 *  Created on: 12 gru 2020
 *      Author: theoar
 */

#ifndef INC_APP_DATA_FORMAT_DATA_FORMAT_H_
#define INC_APP_DATA_FORMAT_DATA_FORMAT_H_

#include <inttypes.h>

namespace data_format
{
  class DataFormat
  {
    public:
      static void humidity(char *buff, uint32_t val);
      static void temperature(char *buff, uint32_t val);
  };
}


#endif /* INC_APP_DATA_FORMAT_DATA_FORMAT_H_ */
