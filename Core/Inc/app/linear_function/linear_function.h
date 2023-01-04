/*
 * linear_function.h
 *
 *  Created on: 4 sty 2023
 *      Author: theoar
 */

#ifndef INC_APP_LINEAR_FUNCTION_LINEAR_FUNCTION_H_
#define INC_APP_LINEAR_FUNCTION_LINEAR_FUNCTION_H_

template<typename T = float>
  class LinearFunction
  {
    public:
      struct Point
      {
	  T x{0};
	  T y{0};
      };

      T a{0};
      T b{0};

    public:
      LinearFunction() = default;

      LinearFunction(const T &a, const T &b)
      {
	this->describe(a, b);
      }

      LinearFunction(const Point &p1, const Point &p2)
      {
	this->describe(p1, p2);
      }

      void describe(const T &a, const T &b)
      {
	this->a = a;
	this->b = b;
      }

      void describe(const Point &p1, const Point &p2)
      {
	T a = (p1.x - p2.x) != 0 ? (p1.y - p2.y) / (p1.x - p2.x) : T{ 0 };
	T b = (p2.x - p1.x) != 0 ? ((p2.x * p1.y) - (p2.y * p1.x)) / (p2.x - p1.x) : T{ 0 };
	this->describe(a, b);
      }

      T value(const T &x) const
      {
	return this->a * x + this->b;
      }

      T getA() const
      {
	return this->a;
      }

      T getB() const
      {
	return this->b;
      }
  };

#endif /* INC_APP_LINEAR_FUNCTION_LINEAR_FUNCTION_H_ */
