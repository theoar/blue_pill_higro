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

#include "app/soft_timer/soft_timer.h"

using namespace board;
using namespace daemon_ctrl;
using namespace binary_regulator;

namespace algorytm
{
  class Algorythm : public IDaemon
  {
    public:
      Algorythm();
      Algorythm(Board *board, Daemon *daemon);

      void setBoard(Board *board);

      void handler() override;

      bool isHumiditySettingsDipSwitch(uint8_t pos);
      bool isConstantOffDipSwitch(uint8_t pos);
      bool isConstantOnDipSwitch(uint8_t pos);
      uint32_t dipSwitchPosToHumidity(uint8_t pos);
    private:
      enum class MachineSt
      {
	WorkingWithRegulator,
	ConstantOn,
	ConstantOff,
	Blockade,
	TimedControlled, //TO DO
      } machineSt = MachineSt::ConstantOff;

      enum class DisplaySt
      {
	MeasuredParameter,
	SettledParameter,
      } displaySt = DisplaySt::MeasuredParameter;

      bool nowTemperature = true;

      SoftTimer maxWorkTimer;
      SoftTimer blockadeTimer;
      SoftTimer displayTimer;

      bool wasEnabled = false;

      uint8_t lastRotaryPos = Algorythm::dsConstantOffPos;
      uint8_t rotartyPosToDisplay = Algorythm::dsConstantOffPos;
      bool rotaryToDisplay = false;

      BinaryRegulator<uint32_t> regulator;

      void handleDisplay();
      void handleAlgol();

      static constexpr uint32_t maxWorkTime = 60*60*1000;
      static constexpr uint32_t blockadeTime = 10*60*1000;

      static constexpr uint32_t displayTime = 3000;

      static constexpr uint8_t dsConstantOnPos = 1;
      static constexpr uint8_t dsConstantOffPos = 0;

      static constexpr uint8_t lowestHumidityPos = 2;
      static constexpr uint8_t higgestHumidityPos = 0xf;

      static constexpr uint32_t lowestHumidity = 30;
      static constexpr uint32_t higgestHumidity = 90;


      Board *board = nullptr;
  };
}




#endif /* INC_ALGORYTHM_ALGORYTHM_H_ */
