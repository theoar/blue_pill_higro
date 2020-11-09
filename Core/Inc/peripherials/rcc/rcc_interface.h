#ifndef RCC_INTERFACE_H_
#define RCC_INTERFACE_H_

#include "rcc_defs.h"


namespace rcc
{

using namespace rcc;

class InterfaceRCC
{
public:
	virtual ~InterfaceRCC(){}
	virtual void peripheralClockEnable(PeripheralClock peripheral) = 0;
	virtual void peripheralClockDisable(PeripheralClock peripheral) = 0;
	//virtual void peripheralReset(PeripheralReset peripheral);
};


}

#endif
