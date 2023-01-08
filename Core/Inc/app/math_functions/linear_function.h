/*
 * linear_function.h
 *
 *  Created on: 4 sty 2023
 *      Author: theoar
 */

#ifndef INC_APP_MATH_FUNCTIONS_LINEAR_FUNCTION_H_
#define INC_APP_MATH_FUNCTIONS_LINEAR_FUNCTION_H_

  class LinearFunction
  {
    private:
      float a{0};
      float b{0};

    public:
      LinearFunction() = default;

      template<typename T>
      LinearFunction(const T &p1, const T &p2)
      {
	this->describe(p1, p2);
      }

      LinearFunction(float a, float b)
      {
	this->describe(a, b);
      }

      void describe(float a, float b)
      {
	this->a = a;
	this->b = b;
      }

      template<typename T>
      void describe(const T &p1, const T &p2)
      {
	float a = (p1.x - p2.x) != 0 ? (p1.y - p2.y) / (p1.x - p2.x) : float{ 0 };
	float b = (p2.x - p1.x) != 0 ? ((p2.x * p1.y) - (p2.y * p1.x)) / (p2.x - p1.x) : float{ 0 };
	this->describe(a, b);
      }

      float value(float x) const
      {
	return this->a * x + this->b;
      }

      float getA() const
      {
	return this->a;
      }

      float getB() const
      {
	return this->b;
      }
  };

#endif /* INC_APP_MATH_FUNCTIONS_LINEAR_FUNCTION_H_ */
