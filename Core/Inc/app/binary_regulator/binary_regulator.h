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
		Heater,
		Cooler
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

		void setType(RegulatorType type )
		{
			this->type = type;
		}

		void setDesiredValue(T value)
		{
			this->desiredValue = value;
		}

		T getDesiredValue()
		{
			return this->desiredValue;
		}

		void setHisteresis(T value)
		{
			this->histeresis = value;
		}

		T getHisteresis()
		{
			return this->desiredValue;
		}

		void setCurrentValue(T value)
		{
			this->currentValue = value;
		}

		bool getState()
		{
			switch(this->type)
			{
			case RegulatorType::Heater:
			{
				if (this->state)
				{
					if (this->currentValue >= this->desiredValue+this->histeresis)
						this->state = false;
				}
				else
				{
					if (this->currentValue <= this->desiredValue)
						this->state = true;
				}
			}
			break;

			case RegulatorType::Cooler:
			{
				if(this->state)
				{
					if(this->currentValue<=this->desiredValue-this->histeresis)
						this->state = false;
				}
				else
				{
					if(this->currentValue>=this->desiredValue)
						this->state = true;
				}
			}
			break;
			default:;
			}

			return this->state;
		}
	};
}



#endif /* SRC_BINARY_REGULATOR_BINARY_REGULATOR_H_ */
