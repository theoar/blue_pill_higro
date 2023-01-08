/*
 * algorythm.cpp
 *
 *  Created on: 9 lis 2020
 *      Author: theoar
 */

#include "algorythm/algorythm.h"

namespace algorytm
{
  Algorythm::Algorythm(Board *board_, Daemon *daemon) :
      board(board_),
      menu(&board->getDisplay(), &board->getKeyboard()),
      temp2HumFunction(&board->getHigrometer())
  {
    daemon->addAndStartProcess(this);

    this->initRegulator();
    this->initMenu(daemon);
  }

  void Algorythm::initRegulator()
  {
    this->regulator
    .setType(RegulatorType::Cooler)
    .setDesiredValue(this->temp2HumFunction.get())
    .setHisteresis(this->humidityHysteresis.get());
  }

  int m_sprintf(char * buffer, Temperature2HumidityRelation::FunctionFitType fitType)
  {
    if(fitType==Temperature2HumidityRelation::FunctionFitType::Lin)
      return sprintf(buffer, "%s", "Lin.");

    if(fitType==Temperature2HumidityRelation::FunctionFitType::Log_2)
      return sprintf(buffer, "%s", "Lo.2 ");

    if(fitType==Temperature2HumidityRelation::FunctionFitType::Log_10)
      return sprintf(buffer, "%s", "Lo.10");

    if(fitType==Temperature2HumidityRelation::FunctionFitType::Log_e)
      return sprintf(buffer, "%s", "Lo.E ");

    return sprintf(buffer, "%s", "Lin.");
  }

  void Algorythm::initMenu(Daemon *daemon)
  {
    this->humidityHysteresisReadWriteItem
    .setFormatValue([this] (auto buffer, auto value) { sprintf(buffer, "%02lu", value); })
    .setValueGet([this] (auto status) { return this->humidityHysteresis.get(status); })
    .setValueChanged([this] (auto newValue) { this->humidityHysteresis.set(newValue); })
    .setPrefix("hi.")
    .setSufix(nullptr)
    .setEditable(true)
    .setRange(this->humidityHysteresis.min(), this->humidityHysteresis.max())
    ;

    this->temp1PointReadWriteItem
    .setFormatValue([this] (auto buffer, auto value) { sprintf(buffer, "%02lu", value); })
    .setValueGet([this] (auto status) { return this->temp2HumFunction.temp1Point.get(status); })
    .setValueChanged([this] (auto newValue) { this->temp2HumFunction.temp1Point.set(newValue); })
    .setPrefix("1c.")
    .setSufix(nullptr)
    .setEditable(true)
    .setRange(this->temp2HumFunction.temp1Point.min(), this->temp2HumFunction.temp1Point.max())
    ;

    this->rh1PointReadWriteItem
    .setFormatValue([this] (auto buffer, auto value) { sprintf(buffer, "%02lu", value); })
    .setValueGet([this] (auto status) { return this->temp2HumFunction.rh1Point.get(status); })
    .setValueChanged([this] (auto newValue) { this->temp2HumFunction.rh1Point.set(newValue); })
    .setPrefix("1h.")
    .setSufix(nullptr)
    .setEditable(true)
    .setRange(this->temp2HumFunction.rh1Point.min(), this->temp2HumFunction.rh1Point.max())
    ;

    this->temp2PointReadWriteItem
    .setFormatValue([this] (auto buffer, auto value) { sprintf(buffer, "%02lu", value); })
    .setValueGet([this] (auto status) { return this->temp2HumFunction.temp2Point.get(status); })
    .setValueChanged([this] (auto newValue) { this->temp2HumFunction.temp2Point.set(newValue); })
    .setPrefix("2c.")
    .setSufix(nullptr)
    .setEditable(true)
    .setRange(this->temp2HumFunction.temp2Point.min(), this->temp2HumFunction.temp2Point.max())
    ;


    this->rh2PointReadWriteItem
    .setFormatValue([this] (auto buffer, auto value) { sprintf(buffer, "%02lu", value); })
    .setValueGet([this] (auto status) { return this->temp2HumFunction.rh2Point.get(status); })
    .setValueChanged([this] (auto newValue) { this->temp2HumFunction.rh2Point.set(newValue); })
    .setPrefix("2h.")
    .setSufix(nullptr)
    .setEditable(true)
    .setRange(this->temp2HumFunction.rh2Point.min(), this->temp2HumFunction.rh2Point.max())
    ;

    this->fitTypeReadWriteItem
    .setFormatValue([this] (auto buffer, auto value) { m_sprintf(buffer, static_cast<Temperature2HumidityRelation::FunctionFitType>(value)); })
    .setValueGet([this] (auto status) { return this->temp2HumFunction.fitType.get(status); })
    .setValueChanged([this] (auto newValue) {  return this->temp2HumFunction.fitType.set(static_cast<Temperature2HumidityRelation::FunctionFitType>(newValue)); })
    .setPrefix("Fu. ")
    .setError(nullptr)
    .setSufix(nullptr)
    .setEditable(true)
    .setRange(this->temp2HumFunction.fitType.min(), this->temp2HumFunction.fitType.max())
    .setPrefixVisibleDuringEdit(false)
    .setSufixVisibleDuringEdit(false)
    ;

    this->calculatedHumidityTreshold
    .setFormatValue([this] (auto buffer, auto value) { sprintf(buffer, "%02lu", value); })
    .setValueGet([this] (auto status) { return this->temp2HumFunction.get(); })
    .setValueChanged([this] (auto newValue) {})
    .setPrefix("on.")
    .setError(nullptr)
    .setSufix(nullptr)
    .setEditable(false)
    .setRange(0, 99)
    ;

    this->humidityReadItem
    .setFormatValue([this] (auto buffer, auto value) { sprintf(buffer, "%02lu", value/100); })
    .setValueGet([this] (auto status) { return this->board->getHigrometer().getHumidity(status); })
    .setValueChanged([this] (auto newValue) {})
    .setPrefix("rh.")
    .setError("Error")
    .setSufix(nullptr)
    .setEditable(false)
    .setRange(0, 9999)
    ;

    this->temperatureReadItem
    .setFormatValue([this] (auto buffer, auto value) { sprintf(buffer, "%02lu.%1lu", value/100, (value/10) % 10); })
    .setValueGet([this] (auto status) { return this->board->getHigrometer().getTemperature(status); })
    .setValueChanged([this] (auto newValue) {})
    .setPrefix(nullptr)
    .setError("Error")
    .setSufix("C")
    .setEditable(false)
    .setRange(0, 9999)
    ;

    this->relayReadItem
    .setFormatValue([this] (auto buffer, auto value) { sprintf(buffer, "%s", value ? " On" : "OFF"); })
    .setValueGet([this] (auto status) { return this->regulator.getState(); })
    .setValueChanged([this] (auto newValue) {})
    .setPrefix(nullptr)
    .setError(nullptr)
    .setSufix(nullptr)
    .setEditable(false)
    .setRange(0, 1)
    ;

    this->restartDefaultReadWriteItem
    .setFormatValue([this] (auto buffer, auto value) { sprintf(buffer, "%s", value ? "1-dA" : "0-no"); })
    .setValueGet([this] (auto status) { return this->factoryRestart; })
    .setValueChanged([this] (auto newValue) {if(newValue) this->factoryReset();})
    .setPrefix("rESE.")
    .setError(nullptr)
    .setSufix(nullptr)
    .setEditable(true)
    .setRange(0, 1)
    .setValueVisableDuringNonEditMode(false)
    ;

    //restartDefaultReadWriteItem


   this->menu.addItem(&this->timeItem);
   this->menu.addItem(&this->humidityReadItem, true);
   this->menu.addItem(&this->temperatureReadItem, true);
   this->menu.addItem(&this->relayReadItem);
   this->menu.addItem(&this->humidityHysteresisReadWriteItem);
   this->menu.addItem(&this->calculatedHumidityTreshold, true);
   this->menu.addItem(&this->fitTypeReadWriteItem);
   this->menu.addItem(&this->temp1PointReadWriteItem);
   this->menu.addItem(&this->rh1PointReadWriteItem);
   this->menu.addItem(&this->temp2PointReadWriteItem);
   this->menu.addItem(&this->rh2PointReadWriteItem);
   this->menu.addItem(&this->restartDefaultReadWriteItem);


   daemon->addAndStartProcess(&this->menu);
  }

  void Algorythm::setBoard(Board *board)
  {
    this->board = board;
  }

  void Algorythm::handleAlgol()
  {
    if(this->board->getHigrometer().isConnected())
    {
      this->regulator
      .setHisteresis(this->humidityHysteresis.get())
      .setCurrentValue(this->board->getHigrometer().getHumidity()/100)
      .setDesiredValue(this->temp2HumFunction.get());
    }
    else
    {
      this->regulator
      .setCurrentValue(50)
      .setDesiredValue(100);
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
	      this->maxWorkTimer.start(this->maxWorkTime);

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

  void Algorythm::factoryReset()
  {
    this->humidityHysteresis.restoreDefault();
    this->temp2HumFunction.restoreDefaults();
  }

}

