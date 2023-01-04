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
#include "app/linear_function/linear_function.h"

#include <functional>

#include "peripherials/bkp/bkp.h"

using namespace board;
using namespace daemon_ctrl;
using namespace binary_regulator;
using namespace bck_item;
using namespace menu;

namespace algorytm
{
  class TemperatureHumidityLinearFunction
  {
    private:
      std::function<float (void)> getArgument;
      LinearFunction<> function;

    public:
      BackupItem temp1Point = BackupItemBuilder::next(0, 99, 10);
      BackupItem rh1Point = BackupItemBuilder::next(31, 89, 50);
      BackupItem temp2Point = BackupItemBuilder::next(0, 99, 30);
      BackupItem rh2Point = BackupItemBuilder::next(31, 89, 89);

      TemperatureHumidityLinearFunction()
      {
	this->rh1Point.set(50);
	this->temp1Point.set(15);

	this->rh2Point.set(80);
	this->temp2Point.set(30);

	this->function.describe(LinearFunction<>::Point{temp1Point.get(), rh1Point.get()}, LinearFunction<>::Point{temp2Point.get(), rh2Point.get()});
      }

      template<typename T>
      void setGetArgumentCallable(T callable)
      {
	this->getArgument = callable;
      }

      uint16_t getValue()
      {
	return this->function.value(this->getArgument());
      }

      uint16_t getValue(float argument)
      {
	return this->function.value(argument);
      }

      void update()
      {
	this->function.describe(LinearFunction<>::Point{temp1Point.get(), rh1Point.get()}, LinearFunction<>::Point{temp2Point.get(), rh2Point.get()});
      }

  };

  class Algorythm : public IDaemon
  {
    public:
      Algorythm();
      Algorythm(Board *board, Daemon *daemon);

      void setBoard(Board *board);
      void initRegulator();
      void initMenu(Daemon *daemon);

      void handler() override;
    private:
      enum class MachineSt
      {
	WorkingWithRegulator,
	ConstantOn,
	ConstantOff,
	Blockade
      } machineSt = MachineSt::WorkingWithRegulator;

      SoftTimer maxWorkTimer;
      SoftTimer blockadeTimer;

      BackupItem humidityHysteresis = BackupItemBuilder::next(1, 15, 5);
      //BackupItem humidityTreshold = BackupItemBuilder::next(31, 89, 50);

      TemperatureHumidityLinearFunction temp2HumFunction;

      MenuItemTime timeItem;

      MenuItemFormattedRead<Higrometer, uint32_t> humidityReadItem;
      MenuItemFormattedRead<Higrometer, uint32_t> temperatureReadItem;
      MenuItemFormattedRead<BinaryRegulator<uint32_t>, bool> relayReadItem;
      MenuItemGenericReadWrite<BackupItem, uint16_t> humidityHysteresisReadWriteItem;
    //  MenuItemGenericReadWrite<BackupItem, uint16_t> humidityTresholdReadWriteItem;

      MenuItemFormattedRead<TemperatureHumidityLinearFunction, uint16_t> calculatedHumidityTreshold;
      MenuItemGenericReadWrite<BackupItem, uint16_t> temp1PointReadWriteItem;
      MenuItemGenericReadWrite<BackupItem, uint16_t> rh1PointReadWriteItem;
      MenuItemGenericReadWrite<BackupItem, uint16_t> temp2PointReadWriteItem;
      MenuItemGenericReadWrite<BackupItem, uint16_t> rh2PointReadWriteItem;

      SimpleMenu<10> menu;

      BinaryRegulator<uint32_t> regulator;

      void handleAlgol();

      static constexpr uint32_t maxWorkTime = 60*60*1000;
      static constexpr uint32_t blockadeTime = 10*60*1000;

      Board *board = nullptr;
  };


}




#endif /* INC_ALGORYTHM_ALGORYTHM_H_ */
