/*
 * algorythm.cpp
 *
 *  Created on: 9 lis 2020
 *      Author: theoar
 */

#include "algorythm/algorythm.h"

namespace algorytm
{
  Algorythm::Algorythm(Board *board, Daemon *daemon)
  {
    this->setBoard(board);
    daemon->addAndStartProcess(this);
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
    switch (this->displaySt)
    {
      case DisplaySt::CurrentHumidity:
      {
	if(this->rotaryToDisplay)
	{
	  this->displaySt = DisplaySt::SettledParameter;
	  this->rotaryToDisplay = false;
	  this->displayTimer.start(this->displayTime);
	}
	else
	{
	  if(this->displayTimer.checkAndRestart())
	    this->displaySt = DisplaySt::CurrentTemperature;

	  if(this->board->getHigrometer().isConnected())
	  {
	    uint32_t humidity = this->board->getHigrometer().getHumidity();
	    this->board->getDisplay().display(humidity / 100);
	  }
	  else
	  {
	    this->board->getDisplay().display("ee");
	  }
	}
      }
      break;

      case DisplaySt::CurrentTemperature:
      {
	if(this->rotaryToDisplay)
	{
	  this->displaySt = DisplaySt::SettledParameter;
	  this->rotaryToDisplay = false;
	  this->displayTimer.start(this->displayTime);
	}
	else
	{
	  if(this->displayTimer.checkAndRestart())
	    this->displaySt = DisplaySt::CurrentHumidity;

	  if(this->board->getHigrometer().isConnected())
	  {
	    uint32_t temperature = this->board->getHigrometer().getTemperature();
	    this->board->getDisplay().display(temperature / 100 + (((temperature % 100) >= 50) ? 1 : 0));
	  }
	  else
	  {
	    this->board->getDisplay().display("ee");
	  }
	}
      }
      break;

      case DisplaySt::SettledParameter:
      {
	if(this->rotaryToDisplay)
	{
	  this->displayTimer.restart();
	  this->rotaryToDisplay = false;
	}
	else
	{
	  if(this->displayTimer.checkAndRestart())
	    this->displaySt = DisplaySt::CurrentTemperature;
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
      }

      default:;
    }
  }

  void Algorythm::handleAlgol()
  {
    switch (this->machineSt)
    {
      case MachineSt::Initializing:
      {
	this->board->getRelayPin().reset();
	this->board->getDisplay().display("FF");

	this->maxWorkTimer.stop();
	this->blockadeTimer.stop();

	uint8_t pos = this->board->getRotaryDipSwitch().number();

	if(this->isConstantOnDipSwitch(pos))
	  this->machineSt = MachineSt::ConstantOn;

	if(this->isConstantOffDipSwitch(pos))
	  this->machineSt = MachineSt::ConstantOff;

	if(this->isHumiditySettingsDipSwitch(pos))
	{
	  uint32_t settings = this->dipSwitchPosToHumidity(pos);

	  this->board->getRegulator().setDesiredValue(settings);
	  this->machineSt = MachineSt::WorkingWithRegulator;
	}

	if(this->machineSt!= MachineSt::Initializing)
	{
	  this->rotaryToDisplay = true;
	  this->rotartyPosToDisplay = pos;
	}
      }
      break;
      case MachineSt::WorkingWithRegulator:
      {
	uint8_t pos = this->board->getRotaryDipSwitch().stableNumber();

	if(this->isConstantOnDipSwitch(pos))
	  this->machineSt = MachineSt::ConstantOn;

	if(this->isConstantOffDipSwitch(pos))
	  this->machineSt = MachineSt::ConstantOff;

	if(this->isHumiditySettingsDipSwitch(pos))
	{
	  uint32_t settings = this->dipSwitchPosToHumidity(pos);
	  this->board->getRegulator().setDesiredValue(settings);
	}

	if(this->lastRotaryPos != pos)
	{
	  this->lastRotaryPos = pos;
	  this->rotaryToDisplay = true;
	  this->rotartyPosToDisplay = pos;
	}

	if(!this->board->getHigrometer().isConnected())
	{
	  this->board->getRelayPin().reset();
	  this->maxWorkTimer.stop();
	}
	else
	{
	  uint32_t humidity = this->board->getHigrometer().getHumidity();
	  this->board->getRegulator().setCurrentValue(humidity);

	  if(this->board->getRegulator().getState())
	  {
	    if(this->maxWorkTimer.isStopped())
	      this->maxWorkTimer.start(Algorythm::maxWorkTime);

	    if(this->maxWorkTimer.check())
	    {
	      this->machineSt = MachineSt::Blockade;
	      this->blockadeTimer.start(this->blockadeTime);
	    }
	  }
	  else
	  {
	    this->maxWorkTimer.stop();
	  }
	}
      }
      break;

      case MachineSt::ConstantOn:
      {
	uint8_t pos = this->board->getRotaryDipSwitch().stableNumber();

	board->getRelayPin().set();

	if(this->isConstantOffDipSwitch(pos))
	  this->machineSt = MachineSt::ConstantOff;

	if(this->isHumiditySettingsDipSwitch(pos))
	{
	  uint32_t settings = this->dipSwitchPosToHumidity(pos);

	  this->board->getRegulator().setDesiredValue(settings);
	  this->machineSt = MachineSt::WorkingWithRegulator;
	}

	if(this->lastRotaryPos != pos)
	{
	  this->lastRotaryPos = pos;
	  this->rotaryToDisplay = true;
	  this->rotartyPosToDisplay = pos;
	}
      }
      break;

      case MachineSt::ConstantOff:
      {
	uint8_t pos = this->board->getRotaryDipSwitch().stableNumber();

	board->getRelayPin().reset();

	if(this->isConstantOnDipSwitch(pos))
	  this->machineSt = MachineSt::ConstantOn;

	if(this->isHumiditySettingsDipSwitch(pos))
	{
	  uint32_t settings = this->dipSwitchPosToHumidity(pos);

	  this->board->getRegulator().setDesiredValue(settings);
	  this->machineSt = MachineSt::WorkingWithRegulator;
	}

	if(this->lastRotaryPos != pos)
	{
	  this->lastRotaryPos = pos;
	  this->rotaryToDisplay = true;
	  this->rotartyPosToDisplay = pos;
	}
      }
      break;

      case MachineSt::Blockade:
      {
	this->board->getRelayPin().reset();

	uint8_t pos = this->board->getRotaryDipSwitch().stableNumber();

	if(this->blockadeTimer.check())
	{
	  this->blockadeTimer.stop();
	  this->machineSt = MachineSt::WorkingWithRegulator;
	}
	else
	{
	  if(this->isConstantOnDipSwitch(pos))
	  {
	    this->machineSt = MachineSt::ConstantOn;
	    this->blockadeTimer.stop();
	  }

	  if(this->isConstantOffDipSwitch(pos))
	  {
	    this->machineSt = MachineSt::ConstantOff;
	    this->blockadeTimer.stop();
	  }

	  if(this->isHumiditySettingsDipSwitch(pos))
	  {
	    uint32_t settings = this->dipSwitchPosToHumidity(pos);

	    this->board->getRegulator().setDesiredValue(settings);
	    this->machineSt = MachineSt::WorkingWithRegulator;

	    this->blockadeTimer.stop();
	  }

	  if(this->lastRotaryPos != pos)
	  {
	    this->lastRotaryPos = pos;
	    this->rotaryToDisplay = true;
	    this->rotartyPosToDisplay = pos;
	  }
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

