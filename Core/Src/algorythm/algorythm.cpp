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
    this->regulator.setType(RegulatorType::Cooler);
    this->regulator.setDesiredValue(this->humidityTreshold.get());
    this->regulator.setHisteresis(this->humidityHysteresis.get());
  }

  void Algorythm::initMenu(Daemon *daemon)
  {
   this->humidityHysteresisReadWriteItem.setProvider(&this->humidityHysteresis, &BackupItem::get, &BackupItem::set);
   this->humidityTresholdReadWriteItem.setProvider(&this->humidityTreshold, &BackupItem::get, &BackupItem::set);

   this->humidityHysteresisReadWriteItem.setDataFormat("%02hu");
   this->humidityHysteresisReadWriteItem.setPrefix("Hi.");
   this->humidityHysteresisReadWriteItem.setRange(this->humidityHysteresis.min(), this->humidityHysteresis.max());
   this->humidityHysteresisReadWriteItem.setStep(1);

   this->humidityTresholdReadWriteItem.setDataFormat("%02hu");
   this->humidityTresholdReadWriteItem.setPrefix("On.");
   this->humidityTresholdReadWriteItem.setRange(this->humidityTreshold.min(), this->humidityTreshold.max());
   this->humidityTresholdReadWriteItem.setStep(1);

   this->humidityReadItem.setProviderFeedback(&this->board->getHigrometer(), &Higrometer::getHumidity);
   this->humidityReadItem.setPrefix("Hu.");
   this->humidityReadItem.setSprintfFunction([](char *buff, uint32_t val) -> void {sprintf(buff, "%2lu", val/100);});
   this->humidityReadItem.setError("Hu Error");

   this->temperatureReadItem.setProviderFeedback(&this->board->getHigrometer(), &Higrometer::getTemperature);
   this->temperatureReadItem.setSprintfFunction([](char *buff, uint32_t val) -> void{  sprintf(buff, "%2lu.%1lu", val/100, (val/10) % 10); });
   this->temperatureReadItem.setSufix("C");
   this->temperatureReadItem.setError("Hu Error");

   this->relayReadItem.setProvider(&this->regulator, &BinaryRegulator<uint32_t>::getState);
   this->relayReadItem.setPrefix("P.");
   this->relayReadItem.setSprintfFunction([](char *buff, bool val) -> void{ sprintf(buff, "%s", val ? " On" : "OFF"); });

   this->menu.addItemAt(&this->timeItem, 0, false);
   this->menu.addItemAt(&this->humidityReadItem, 1, true);
   this->menu.addItemAt(&this->temperatureReadItem, 2, true);
   this->menu.addItemAt(&this->relayReadItem, 3, false);
   this->menu.addItemAt(&this->humidityHysteresisReadWriteItem, 4, false);
   this->menu.addItemAt(&this->humidityTresholdReadWriteItem, 5, false);

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
    this->regulator.setDesiredValue(this->humidityTreshold.get());
    this->regulator.setHisteresis(this->humidityHysteresis.get());

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
	  uint32_t humidity = this->board->getHigrometer().getHumidity();
	  this->regulator.setCurrentValue(humidity/100);

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

