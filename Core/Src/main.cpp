/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include "board/board.h"
#include "algorythm/algorythm.h"

#include "app/soft_timer/soft_timer.h"
#include "app/daemon_ctrl/daemon_ctrl.h"

#include <stdio.h>

using namespace timer;
using namespace daemon_ctrl;
using namespace board;
using namespace algorytm;


int main(void)
{
  Daemon deamon;
  Board board(&deamon);
  SoftTimer blinkTimer(1000);
  Algorythm algol(&board, &deamon);

  while(1)
  {
    if(blinkTimer.checkAndRestart())
      board.getLed().toggle();

    deamon.handler();
  }

  return 0;
}

