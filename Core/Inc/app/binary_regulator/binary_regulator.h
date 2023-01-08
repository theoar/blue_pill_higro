/*
 * binary_regulator.h
 *
 *  Created on: 9 lis 2020
 *      Author: theoar
 */

#ifndef SRC_BINARY_REGULATOR_BINARY_REGULATOR_H_
#define SRC_BINARY_REGULATOR_BINARY_REGULATOR_H_

namespace binary_regulator
{

  enum class RegulatorType
  {
    Heater, Cooler
  };
  template<typename T>
    class BinaryRegulator
    {

      private:
	T desiredValue = 0;
	T histeresis = 0;

	T currentValue = 0;

	bool state = false;

	RegulatorType type = RegulatorType::Cooler;

      public:

	auto & setType(RegulatorType type)
	{
	  this->type = type;
	  return *this;
	}

	auto & setDesiredValue(T value)
	{
	  this->desiredValue = value;
	  return *this;
	}

	T getDesiredValue()
	{
	  return this->desiredValue;
	}

	auto & setHisteresis(T value)
	{
	  this->histeresis = value;
	  return *this;
	}

	T getHisteresis()
	{
	  return this->desiredValue;
	}

	auto & setCurrentValue(T value)
	{
	  this->currentValue = value;
	  return *this;
	}

	bool getState()
	{
	  switch (this->type)
	  {
	    case RegulatorType::Heater:
	    {
	      if(this->state)
	      {
		if(this->currentValue >= this->desiredValue + this->histeresis)
		  this->state = false;
	      }
	      else
	      {
		if(this->currentValue <= this->desiredValue)
		  this->state = true;
	      }
	    }
	    break;

	    case RegulatorType::Cooler:
	    {
	      if(this->state)
	      {
		if(this->currentValue + this->histeresis <= this->desiredValue)
		  this->state = false;
	      }
	      else
	      {
		if(this->currentValue >= this->desiredValue)
		  this->state = true;
	      }
	    }
	    break;
	    default:
	      ;
	  }

	  return this->state;
	}
    };
}

#endif /* SRC_BINARY_REGULATOR_BINARY_REGULATOR_H_ */
