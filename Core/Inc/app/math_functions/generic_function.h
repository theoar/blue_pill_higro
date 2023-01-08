/*
 * generic_function.h
 *
 *  Created on: 7 sty 2023
 *      Author: theoar
 */

#ifndef INC_APP_MATH_FUNCTIONS_GENERIC_FUNCTION_H_
#define INC_APP_MATH_FUNCTIONS_GENERIC_FUNCTION_H_

#include "function_value.h"
#include "functions.h"

#include <cmath>

class GenericFunction : FunctionValue
{
  public:
    using FunctionBase = float (*)(float);

  private:
     FunctionBase function;
     float a{0};
     float b{0};

  public:
     GenericFunction() {
       this->describe(0.f, 0.f, &linear);
     }

    template<typename T>
    GenericFunction(const T &p1, const T &p2, FunctionBase fnc)
    {
	this->describe(p1, p2, fnc);
    }

    GenericFunction(float a, float b, FunctionBase fnc)
    {
	this->describe(a, b, fnc);
    }

    template<typename T>
    void describe(const T &p1, const T &p2, FunctionBase fnc)
    {
      float a = (p1.y - p2.y) / (fnc(p1.x) - fnc(p2.x));
      float b = ((fnc(p2.x) * p1.y) - (p2.y * fnc(p1.x))) / (fnc(p2.x) - fnc(p1.x));
      this->describe(a, b, fnc);
    }

    void describe(float a, float b, FunctionBase fnc)
    {
      this->a = a;
      this->b = b;
      this->function = fnc;
    }

    template<typename T>
    void describe(const T &p1, const T &p2)
    {
      FunctionBase fnc = this->function==nullptr ? &linear : this->function;
      this->describe(p1, p2, fnc);
    }

    void describe(float a, float b)
    {
      FunctionBase fnc = this->function==nullptr ? &linear : this->function;
      this->describe(a, b, fnc);
    }

    float value(float x) const
    {
      return this->a*this->function(x)+this->b;
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

#endif /* INC_APP_MATH_FUNCTIONS_GENERIC_FUNCTION_H_ */
