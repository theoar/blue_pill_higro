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

#include "peripherials/bkp/bkp.h"

using namespace board;
using namespace daemon_ctrl;
using namespace binary_regulator;
using namespace bck_item;
using namespace menu;

namespace algorytm
{
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
      BackupItem humidityTreshold = BackupItemBuilder::next(31, 89, 50);

      MenuItemTime timeItem;
      MenuItemFormattedRead<Higrometer, uint32_t> humidityReadItem;
      MenuItemFormattedRead<Higrometer, uint32_t> temperatureReadItem;
      MenuItemFormattedRead<BinaryRegulator<uint32_t>, bool> relayReadItem;
      MenuItemGenericReadWrite<BackupItem, uint16_t> humidityHysteresisReadWriteItem;
      MenuItemGenericReadWrite<BackupItem, uint16_t> humidityTresholdReadWriteItem;;

      SimpleMenu<6> menu;

      BinaryRegulator<uint32_t> regulator;

      void handleAlgol();

      static constexpr uint32_t maxWorkTime = 60*60*1000;
      static constexpr uint32_t blockadeTime = 10*60*1000;

      Board *board = nullptr;
  };
}




#endif /* INC_ALGORYTHM_ALGORYTHM_H_ */
