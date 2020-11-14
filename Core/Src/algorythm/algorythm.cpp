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
  }

  Algorythm::Algorythm()
  {
    this->regulator.setType(RegulatorType::Cooler);
    this->regulator.setDesiredValue(60);
    this->regulator.setHisteresis(5);

    this->maxWorkTimer.stop();
    this->blockadeTimer.stop();

    this->displayTimer.start(this->displayTime);
  }

  void Algorythm::setBoard(Board *board)
  {
    this->board = board;
  }

  bool Algorythm::isHumiditySettingsDipSwitch(uint8_t pos)
  {
    return pos>=lowestHumidityPos && pos<=higgestHumidityPos;
  }

  bool Algorythm::isConstantOffDipSwitch(uint8_t pos)
  {
    return pos==Algorythm::dsConstantOffPos;
  }

  bool Algorythm::isConstantOnDipSwitch(uint8_t pos)
  {
    return pos==Algorythm::dsConstantOnPos;
  }

  uint32_t Algorythm::dipSwitchPosToHumidity(uint8_t pos)
  {
    uint32_t humiditySettings = lowestHumidityPos;
    if(pos>=lowestHumidityPos && pos<=higgestHumidityPos)
    {
      uint8_t optionsCnt = higgestHumidityPos-lowestHumidityPos+1;
      uint8_t selected = pos - lowestHumidityPos;
      humiditySettings = ((higgestHumidity-lowestHumidity)*selected)/optionsCnt + lowestHumidity;
    }

    return humiditySettings;

  }

  void Algorythm::handleDisplay()
  {
    if(this->rotaryToDisplay)
    {
      this->displaySt = DisplaySt::SettledParameter;
      this->rotaryToDisplay = false;
      this->displayTimer.restart();
    }

    switch (this->displaySt)
    {
      case DisplaySt::MeasuredParameter:
      {
	if(this->displayTimer.checkAndRestart())
	  this->nowTemperature = !this->nowTemperature;

	if(this->board->getHigrometer().isConnected())
	{
	  uint32_t humidity = this->board->getHigrometer().getHumidity();
	  uint32_t temperature = this->board->getHigrometer().getTemperature();

	  if(this->nowTemperature)
	    this->board->getDisplay().display(temperature / 100 + (((temperature % 100) >= 50) ? 1 : 0));
	  else
	    this->board->getDisplay().display(humidity / 100);
	}
	else
	{
	  this->board->getDisplay().display("ee");
	}
      }
      break;

      case DisplaySt::SettledParameter:
      {
	if(this->displayTimer.checkAndRestart())
	  this->displaySt = DisplaySt::MeasuredParameter;
	else
	{
	  if(this->isConstantOffDipSwitch(this->rotartyPosToDisplay))
	    this->board->getDisplay().display("oF");

	  if(this->isConstantOnDipSwitch(this->rotartyPosToDisplay))
	    this->board->getDisplay().display("on");

	  if(this->isHumiditySettingsDipSwitch(this->rotartyPosToDisplay))
	  {
	    uint32_t humiditySettled = this->dipSwitchPosToHumidity(this->rotartyPosToDisplay);
	    this->board->getDisplay().display(humiditySettled);
	  }
	}
      }

      default:;
    }
  }

  void Algorythm::handleAlgol()
  {
    uint8_t pos = this->board->getRotaryDipSwitch().stableNumber();

    if(this->lastRotaryPos != pos)
    {
      this->lastRotaryPos = pos;
      this->rotaryToDisplay = true;
      this->rotartyPosToDisplay = pos;
    }

    if(this->isConstantOnDipSwitch(pos))
      this->machineSt = MachineSt::ConstantOn;

    if(this->isConstantOffDipSwitch(pos))
      this->machineSt = MachineSt::ConstantOff;

    if(this->isHumiditySettingsDipSwitch(pos))
    {
      uint32_t settings = this->dipSwitchPosToHumidity(pos);

      this->regulator.setDesiredValue(settings);

      if(this->machineSt != MachineSt::Blockade)
	this->machineSt = MachineSt::WorkingWithRegulator;
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
    this->handleDisplay();
    this->handleAlgol();
  }
}

