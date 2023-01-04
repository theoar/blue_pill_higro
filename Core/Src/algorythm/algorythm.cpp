/*
 * algorythm.cpp
 *
 *  Created on: 9 lis 2020
 *      Author: theoar
 */

#include "algorythm/algorythm.h"

namespace algorytm
{
  Algorythm::Algorythm(Board *board, Daemon *daemon) :  Algorythm()
  {
    this->setBoard(board);
    daemon->addAndStartProcess(this);

    this->initRegulator();
    this->initMenu(daemon);
  }

  Algorythm::Algorythm()
  {
    this->maxWorkTimer.stop();
    this->blockadeTimer.stop();
  }

  void Algorythm::initRegulator()
  {
    this->temp2HumFunction.setGetArgumentCallable([this] {
	return this->board->getHigrometer().getTemperature()/100.f;
    });

    this->regulator.setType(RegulatorType::Cooler);
    this->regulator.setDesiredValue(this->temp2HumFunction.getValue());
    this->regulator.setHisteresis(this->humidityHysteresis.get());
  }

 template<typename T>
  void setupReadWriteItem(T & menuItem, BackupItem & backupItem, const char * prefix, const char *sufix, const char * dataFormat, typename T::VarTypeE step)
  {
    menuItem.setProvider(&backupItem, &BackupItem::get, &BackupItem::set);
    menuItem.setDataFormat(dataFormat);
    menuItem.setPrefix(prefix);
    menuItem.setSufix(sufix);
    menuItem.setRange(backupItem.min(), backupItem.max());
    menuItem.setStep(step);
  }

  void Algorythm::initMenu(Daemon *daemon)
  {
   setupReadWriteItem(this->humidityHysteresisReadWriteItem, this->humidityHysteresis, "Hi.", nullptr, "%02hu", 1);

   setupReadWriteItem(this->rh1PointReadWriteItem, this->temp2HumFunction.rh1Point, "0h.", nullptr, "%02hu", 1);
   setupReadWriteItem(this->temp1PointReadWriteItem, this->temp2HumFunction.temp1Point, "0c.", nullptr, "%02hu", 1);
   setupReadWriteItem(this->rh2PointReadWriteItem, this->temp2HumFunction.rh2Point, "1h.", nullptr, "%02hu", 1);
   setupReadWriteItem(this->temp2PointReadWriteItem, this->temp2HumFunction.temp2Point, "1c.", nullptr, "%02hu", 1);


   this->humidityReadItem.setProviderFeedback(&this->board->getHigrometer(), &Higrometer::getHumidity);
   this->humidityReadItem.setPrefix("Hu.");
   this->humidityReadItem.setSprintfFunction([](char *buff, uint32_t val) -> void {sprintf(buff, "%2lu", val/100);});
   this->humidityReadItem.setError("Hu Error");

   this->temperatureReadItem.setProviderFeedback(&this->board->getHigrometer(), &Higrometer::getTemperature);
   this->temperatureReadItem.setSprintfFunction([](char *buff, uint32_t val) -> void{sprintf(buff, "%2lu.%1lu", val/100, (val/10) % 10);});
   this->temperatureReadItem.setSufix("C");
   this->temperatureReadItem.setError("Hu Error");

   this->relayReadItem.setProvider(&this->regulator, &BinaryRegulator<uint32_t>::getState);
   this->relayReadItem.setPrefix("P.");
   this->relayReadItem.setSprintfFunction([](char *buff, bool val) -> void{ sprintf(buff, "%s", val ? " On" : "OFF"); });

   this->calculatedHumidityTreshold.setProvider(&this->temp2HumFunction, &TemperatureHumidityLinearFunction::getValue);
   this->calculatedHumidityTreshold.setSprintfFunction([](char *buff, uint16_t val) -> void{sprintf(buff, "%2hu", val);});
   this->calculatedHumidityTreshold.setPrefix("Ch.");

   this->menu.addItem(&this->timeItem, false);
   this->menu.addItem(&this->humidityReadItem, true);
   this->menu.addItem(&this->temperatureReadItem, true);
   this->menu.addItem(&this->relayReadItem, false);
   this->menu.addItem(&this->humidityHysteresisReadWriteItem, false);
   this->menu.addItem(&this->calculatedHumidityTreshold, true);
   this->menu.addItem(&this->temp1PointReadWriteItem, false);
   this->menu.addItem(&this->rh1PointReadWriteItem, false);
   this->menu.addItem(&this->temp2PointReadWriteItem, false);
   this->menu.addItem(&this->rh2PointReadWriteItem, false);


   this->menu.setKeyboard(&this->board->getKeyboard());
   this->menu.setDisplay(&this->board->getDisplay());

   daemon->addAndStartProcess(&this->menu);
  }

  void Algorythm::setBoard(Board *board)
  {
    this->board = board;
  }

  void Algorythm::handleAlgol()
  {
    if(this->menu.enteredIntoDisplayMode())
    {
      this->temp2HumFunction.update();
      this->regulator.setHisteresis(this->humidityHysteresis.get());
    }

    if(this->board->getHigrometer().isConnected())
    {
      uint32_t humidity = this->board->getHigrometer().getHumidity();
      uint32_t temperature = this->board->getHigrometer().getTemperature();
      this->regulator.setCurrentValue(humidity/100);
      this->regulator.setDesiredValue(this->temp2HumFunction.getValue(temperature/100.f));
    }
    else
    {
      this->regulator.setCurrentValue(50);
      this->regulator.setDesiredValue(100);
    }

    switch (this->machineSt)
    {
      case MachineSt::WorkingWithRegulator:
      {
	if(!this->board->getHigrometer().isConnected())
	{
	  this->board->setRelayState(false);
	  this->maxWorkTimer.stop();
	}
	else
	{
	  if(this->regulator.getState())
	  {
	    if(this->maxWorkTimer.isStopped())
	      this->maxWorkTimer.start(Algorythm::maxWorkTime);

	    if(this->maxWorkTimer.check())
	    {
	      this->machineSt = MachineSt::Blockade;
	      this->blockadeTimer.start(this->blockadeTime);
	      this->maxWorkTimer.stop();
	    }

	    this->board->setRelayState(true);
	  }
	  else
	  {
	    this->maxWorkTimer.stop();
	    this->board->setRelayState(false);
	  }
	}
      }
      break;

      case MachineSt::ConstantOn:
	this->board->setRelayState(true);
      break;

      case MachineSt::ConstantOff:
	this->board->setRelayState(false);
      break;

      case MachineSt::Blockade:
      {
	this->board->setRelayState(false);

	if(this->blockadeTimer.check())
	{
	  this->blockadeTimer.stop();
	  this->machineSt = MachineSt::WorkingWithRegulator;
	}
      }
      break;

      default:;
    }
  }

  void Algorythm::handler()
  {
    this->handleAlgol();
  }
}

