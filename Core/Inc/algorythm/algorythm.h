/*
 * algorythm.h
 *
 *  Created on: 9 lis 2020
 *      Author: theoar
 */

#ifndef INC_ALGORYTHM_ALGORYTHM_H_
#define INC_ALGORYTHM_ALGORYTHM_H_

#include "board/board.h"

#include "app/daemon_ctrl/daemon_interface.h"
#include "app/daemon_ctrl/daemon_ctrl.h"
#include "app/binary_regulator/binary_regulator.h"
#include "app/menu/menu_item_interface.h"
#include "app/menu/simple_menu.h"
#include "app/soft_timer/soft_timer.h"
#include "app/bck_item/bck_item.h"
#include "app/math_functions/generic_function.h"
#include "app/math_functions/point.h"

#include "peripherials/bkp/bkp.h"

#include <functional>
#include <cmath>

using namespace board;
using namespace daemon_ctrl;
using namespace binary_regulator;
using namespace bck_item;
using namespace menu;

namespace algorytm
{
  class Temperature2HumidityRelation
  {
    public:
      enum FunctionFitType
      {
	Lin,
	Min = Lin,
	Log_2,
	Log_e,
	Log_10,
	Max = Log_10,
	Default = Lin
      };

      GenericFunction::FunctionBase getFitFunction(FunctionFitType fnc)
      {
	if(fnc==FunctionFitType::Lin)
	  return &linear;

	if(fnc==FunctionFitType::Log_2)
	  return &std::log2;

	if(fnc==FunctionFitType::Log_10)
	  return &std::log10;

	if(fnc==FunctionFitType::Log_e)
	  return &std::log;

	 return &linear;
      }
    private:
      GenericFunction function;
      Higrometer *higromenter;
      bool blockUpdate = false;

      static inline constexpr BackupItemUnsigned::Data DefaultTemp1Point{0, 99, 15};
      static inline constexpr BackupItemUnsigned::Data DefaultRh1Point{31, 89, 85};
      static inline constexpr BackupItemUnsigned::Data DefaultTemp2Point{0, 99, 25};
      static inline constexpr BackupItemUnsigned::Data DefaultRh2Point{31, 89, 70};

      static inline constexpr BackupItemUnsigned::Data DefaultFitType{FunctionFitType::Min, FunctionFitType::Max, FunctionFitType::Default};

    public:
      BackupItemUnsigned temp1Point = BackupItemBuilder::next<BackupItemUnsigned>(DefaultTemp1Point);
      BackupItemUnsigned rh1Point = BackupItemBuilder::next<BackupItemUnsigned>(DefaultRh1Point);
      BackupItemUnsigned temp2Point = BackupItemBuilder::next<BackupItemUnsigned>(DefaultTemp2Point);
      BackupItemUnsigned rh2Point = BackupItemBuilder::next<BackupItemUnsigned>(DefaultRh2Point);

      BackupItemUnsigned fitType = BackupItemBuilder::next<BackupItemUnsigned>(DefaultFitType);

      Temperature2HumidityRelation(Higrometer *higromenter_) : higromenter(higromenter_)
      {
	temp1Point.observe(std::bind(&Temperature2HumidityRelation::pointCoordsChanged, this, std::placeholders::_1));
	rh1Point.observe(std::bind(&Temperature2HumidityRelation::pointCoordsChanged, this, std::placeholders::_1));
	temp2Point.observe(std::bind(&Temperature2HumidityRelation::pointCoordsChanged, this, std::placeholders::_1));
	rh2Point.observe(std::bind(&Temperature2HumidityRelation::pointCoordsChanged, this, std::placeholders::_1));

	fitType.observe(std::bind(&Temperature2HumidityRelation::fitFunctionChanged, this, std::placeholders::_1));

	this->pointCoordsChanged(nullptr);
      }

      uint32_t get()
      {
	return this->function.value(this->higromenter->getTemperature()/100.f);
      }

      void updateFunction()
      {
	if(this->blockUpdate)
	  return;

	if(temp1Point.get()!=temp2Point.get())
	{
	  this->function.describe(FunctionPoint{(float)temp1Point.get(), (float)rh1Point.get()},
				  FunctionPoint{(float)temp2Point.get(), (float)rh2Point.get()},
				  getFitFunction(static_cast<FunctionFitType>(this->fitType.get())));
	}
	else
	{
	  this->function.describe(FunctionPoint{(float)temp1Point.min(), (float)rh1Point.get()},
				  FunctionPoint{(float)temp1Point.max(), (float)rh1Point.get()},
				  getFitFunction(FunctionFitType::Lin));
	}
      }

      void pointCoordsChanged(BackupItemUnsigned *source)
      {
	this->updateFunction();
      }

      void fitFunctionChanged(BackupItemUnsigned *source)
      {
	this->updateFunction();
      }

      void restoreDefaults()
      {
	this->blockUpdate = true;

	temp1Point.restoreDefault();
	rh1Point.restoreDefault();
	temp2Point.restoreDefault();
	rh2Point.restoreDefault();
	fitType.restoreDefault();

	this->blockUpdate = false;

	this->updateFunction();
      }

  };

   class Algorythm : public IDaemon
  {
    public:
      Algorythm(Board *board_, Daemon *daemon);

      void handler() override;

    private:

      enum class MachineSt
      {
	WorkingWithRegulator,
	ConstantOn,
	ConstantOff,
	Blockade
      } machineSt = MachineSt::WorkingWithRegulator;

      Board *board = nullptr;

      SimpleMenu<15> menu;
      Temperature2HumidityRelation temp2HumFunction;
      BinaryRegulator<uint32_t> regulator;

      std::function<uint16_t (void)> getDesiredHumidityValue;

      SoftTimer maxWorkTimer;
      SoftTimer blockadeTimer;

      static inline constexpr BackupItemUnsigned::Data DefaultHumidityHisteresis{1, 15, 5};
      BackupItemUnsigned humidityHysteresis = BackupItemBuilder::next<BackupItemUnsigned>(DefaultHumidityHisteresis);

      MenuItemTime timeItem;

      MenuItemGenericReadWriteFunctional<uint32_t> humidityReadItem;
      MenuItemGenericReadWriteFunctional<uint32_t> temperatureReadItem;

      MenuItemGenericReadWriteFunctional<bool> relayReadItem;

      MenuItemGenericReadWriteFunctional<BackupItemUnsigned::DataType> humidityHysteresisReadWriteItem;

      MenuItemGenericReadWriteFunctional<uint32_t> calculatedHumidityTreshold;
      MenuItemGenericReadWriteFunctional<BackupItemUnsigned::DataType> temp1PointReadWriteItem;
      MenuItemGenericReadWriteFunctional<BackupItemUnsigned::DataType> rh1PointReadWriteItem;
      MenuItemGenericReadWriteFunctional<BackupItemUnsigned::DataType> temp2PointReadWriteItem;
      MenuItemGenericReadWriteFunctional<BackupItemUnsigned::DataType> rh2PointReadWriteItem;
      MenuItemGenericReadWriteFunctional<BackupItemUnsigned::DataType> fitTypeReadWriteItem;


      bool factoryRestart{false};
      MenuItemGenericReadWriteFunctional<bool> restartDefaultReadWriteItem;


      void handleAlgol();
      void setBoard(Board *board);
      void initRegulator();
      void initMenu(Daemon *daemon);

      void factoryReset();

      static constexpr uint32_t maxWorkTime = 60*60*1000;
      static constexpr uint32_t blockadeTime = 10*60*1000;

  };

}




#endif /* INC_ALGORYTHM_ALGORYTHM_H_ */
