/*
 * data_format.cpp
 *
 *  Created on: 12 gru 2020
 *      Author: theoar
 */


#include "app/data_format/data_format.h"

#include <stdio.h>

namespace data_format
{
  void DataFormat::humidity(char *buff, uint32_t val)
  {
    sprintf(buff, "Hu.%2lu", val/100);
  }

  void DataFormat::temperature(char *buff, uint32_t val)
  {
    sprintf(buff, "%2lu.%1lu", val/100, (val/10) % 10);
  }
}
