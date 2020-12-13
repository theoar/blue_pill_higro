/*
 * higrometer.cpp
 *
 *  Created on: 1 lis 2020
 *      Author: theoar
 */

#include "app/higrometer/higrometer.h"

namespace higrometer
{
  void Higrometer::handlerSimple()
  {
    switch (this->step)
    {
      case AskStep::Init:
      {
	if(this->timer.check())
	{
	  if(this->dht->isIdle())
	  {
	    this->dht->read();
	    this->step = AskStep::Result;
	  }
	}
      }
      break;
      case AskStep::FirstInit:
      {
	this->connected = false;
	if(this->dht->isIdle())
	{
	  this->dht->read();
	  this->step = AskStep::Result;
	}
      }
      break;

      case AskStep::Result:
      {
	if(this->dht->isResultReady())
	{
	  this->humidity = this->dht->getHumidity();
	  this->temperature = this->dht->getTemperature();

	  this->timer.start(this->askTime);

	  this->connected = true;

	  this->step = AskStep::Init;
	}
	else
	{
	  if(this->dht->isError())
	  {
	    this->step = AskStep::FirstInit;
	  }
	}
      }
      break;

      default:
	;

    }
  }

  void Higrometer::handlerAdvanced()
  {
    switch (this->step)
    {
      case AskStep::FirstInit:
      {
	this->connected = false;
	if(this->dht->isIdle())
	{
	  this->dht->read();
	  this->step = AskStep::First;
	}
      }
      break;

      case AskStep::Init:
      {
	if(this->timer.check())
	{
	  if(this->dht->isIdle())
	  {
	    this->dht->read();
	    this->step = AskStep::First;
	  }
	}
      }
      break;

      case AskStep::First:
      {
	if(this->dht->isResultReady())
	{
	  this->timer.start(this->inRownAskTime);
	  this->step = AskStep::Second;
	}
	else
	{
	  if(this->dht->isError())
	  {
	    this->step = AskStep::FirstInit;
	  }
	}
      }
      break;

      case AskStep::Second:
      {
	if(this->timer.check())
	{
	  if(this->dht->isIdle())
	  {
	    this->dht->read();
	    this->step = AskStep::Result;
	  }
	}
	break;

	case AskStep::Result:
	{
	  if(this->dht->isResultReady())
	  {
	    this->humidity = this->dht->getHumidity();
	    this->temperature = this->dht->getTemperature();

	    this->connected = true;

	    this->timer.start(this->askTime);

	    this->step = AskStep::Init;
	  }
	  else
	  {
	    if(this->dht->isError())
	    {
	      this->step = AskStep::FirstInit;
	    }
	  }
	}
	break;
      }
    }
  }

  void Higrometer::init(Dht11 *dht, uint32_t askTime)
  {
    this->dht = dht;
    this->step = AskStep::FirstInit;
    this->connected = false;

    this->humidity = 0;
    this->temperature = 0;

    if(askTime < 1000)
      this->askTime = 1000;
    else
      this->askTime = askTime;

    if(askTime < timeThreshold)
      this->measureType = MeasureType::Simple;
    else
      this->measureType = MeasureType::Advanced;
  }

  void Higrometer::setAskTime(uint32_t askTime)
  {
    this->step = AskStep::FirstInit;

    if(askTime < 1000)
      this->askTime = 1000;
    else
      this->askTime = askTime;

    if(askTime < timeThreshold)
      this->measureType = MeasureType::Simple;
    else
      this->measureType = MeasureType::Advanced;
  }

  void Higrometer::handler()
  {
    switch (this->measureType)
    {
      case MeasureType::Advanced:
	this->handlerAdvanced();
      break;
      case MeasureType::Simple:
	this->handlerSimple();
      break;
    }
  }

  bool Higrometer::isConnected()
  {
    return this->connected;
  }

  uint32_t Higrometer::getTemperature()
  {
    return this->temperature;
  }

  uint32_t Higrometer::getHumidity()
  {
    return this->humidity;
  }

  uint32_t Higrometer::getTemperature(bool *result)
  {
    *result = this->isConnected();
    return this->getTemperature();
  }

  uint32_t Higrometer::getHumidity(bool *result)
  {
    *result = this->isConnected();
    return this->getHumidity();
  }

}

