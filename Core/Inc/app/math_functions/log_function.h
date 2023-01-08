/*
 * log_function.h
 *
 *  Created on: 7 sty 2023
 *      Author: theoar
 */

#ifndef INC_APP_MATH_FUNCTIONS_LOG_FUNCTION_H_
#define INC_APP_MATH_FUNCTIONS_LOG_FUNCTION_H_

#include <app/math_functions/function_value.h>
#include <cmath>


class LogFunction : FunctionValue
{
  public:
    enum LogBase
    {
      _2,
      _10,
      _e
    };

  private:
     using GetValue = float (*)(float);
     GetValue function;
     float a{0};
     float b{0};
     LogBase logBase;

     GetValue getFunction(LogBase logBase)
     {
       if(logBase==LogBase::_10)
 	return &std::log10;

       if(logBase==LogBase::_2)
 	return &std::log2;

       if(logBase==LogBase::_e)
 	return &std::log;

       return &std::log2;
     }

  public:
     LogFunction() {
       this->describe(1.f, 1.f, LogBase::_2);
     }

    template<typename T>
    LogFunction(const T &p1, const T &p2, LogBase base)
    {
	this->describe(p1, p2, base);
    }

    LogFunction(float a, float b, LogBase base)
    {
	this->describe(a, b, base);
    }

    template<typename T>
    void describe(const T &p1, const T &p2, LogBase base)
    {
      auto fnc = this->getFunction(this->logBase);
      float a = (p1.y - p2.y) / (fnc(p1.x) - fnc(p2.x));
      float b = ((fnc(p2.x) * p1.y) - (p2.y * fnc(p1.x))) / (fnc(p2.x) - fnc(p1.x));
      this->describe(a, b, base);
    }

    void describe(float a, float b, LogBase base)
    {
      this->a = a;
      this->b = b;
      this->describe(base);
    }

    void describe(LogBase base)
    {
      this->logBase = base;
      this->function = this->getFunction(this->logBase);
    }

    float value(float x) const
    {
      if(function == nullptr)
	return 0;

      return function(x);
    }

    float getA() const
    {
      return this->a;
    }

    float getB() const
    {
      return this->b;
    }

    LogBase getBase() const
    {
      return this->logBase;
    }




};


#endif /* INC_APP_MATH_FUNCTIONS_LOG_FUNCTION_H_ */
