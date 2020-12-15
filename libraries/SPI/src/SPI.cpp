// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

extern "C" {
#include <bl_gpio.h>
#include <bl602_spi.h>
#include <bl602_gpio.h>
#include <bl602_glb.h>
#include <bl_irq.h>
#include <spi_reg.h>
}

#include "SPI.h"

SPIClass SPI = SPIClass();

void spiInterrupt(void* ctx)
{

}

void SPIClass::begin()
{
  uint8_t gpio_pins[] = { // TODO: Pin definitions
    17,
    14,
    12,
    11,
    /*0,
    1,
    2,
    3*/
  };
  GLB_GPIO_Func_Init(GPIO_FUN_SPI, (GLB_GPIO_Type*)gpio_pins, 4);

  GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_MASTER);

  SPI_IntMask((SPI_ID_Type)0, SPI_INT_ALL,MASK);
  bl_irq_enable(SPI_IRQn);
  bl_irq_register_with_ctx(SPI_IRQn, (void*)spiInterrupt, this);

  SPI_Disable((SPI_ID_Type)0, SPI_WORK_MODE_MASTER);
  SPI_Disable((SPI_ID_Type)0, SPI_WORK_MODE_SLAVE);
}

void SPIClass::beginTransaction(SPISettings settings)
{
  SPI_CFG_Type spiConfig;
  SPI_FifoCfg_Type fifoConfig;
  
  if (this->currentFrequency != settings.clock) {
    this->setFrequency(settings.clock);
  }

  spiConfig.deglitchEnable = DISABLE;
  spiConfig.continuousEnable = ENABLE;
  spiConfig.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST,
  spiConfig.bitSequence = settings.bitOrder == SPI_MSBFIRST ? SPI_BIT_INVERSE_MSB_FIRST : SPI_BIT_INVERSE_LSB_FIRST,
  spiConfig.frameSize = SPI_FRAME_SIZE_8;
  
  //spiConfig.clkPhaseInv = (settings.dataMode == SPI_MODE0 || settings.dataMode == SPI_MODE2) ? SPI_CLK_PHASE_INVERSE_0 : SPI_CLK_PHASE_INVERSE_1;
  //spiConfig.clkPolarity = (settings.dataMode == SPI_MODE0 || settings.dataMode == SPI_MODE1) ? SPI_CLK_POLARITY_LOW : SPI_CLK_POLARITY_HIGH;
  spiConfig.clkPhaseInv = SPI_CLK_PHASE_INVERSE_0;
  spiConfig.clkPolarity = SPI_CLK_POLARITY_HIGH;

  SPI_Init((SPI_ID_Type)0, &spiConfig);

  fifoConfig.txFifoThreshold = 1;
  fifoConfig.rxFifoThreshold = 5;
  fifoConfig.txFifoDmaEnable = DISABLE;
  fifoConfig.rxFifoDmaEnable = DISABLE;
  SPI_FifoConfig((SPI_ID_Type)0, &fifoConfig);

  //SPI_IntMask((SPI_ID_Type)0, SPI_INT_ALL, UNMASK);
  SPI_Enable((SPI_ID_Type)0, SPI_WORK_MODE_MASTER);
}

void SPIClass::setFrequency(uint32_t frequency) {
  SPI_ClockCfg_Type clockConfig;
  uint8_t clockDivider = (uint8_t)(40000000 / frequency);
  GLB_Set_SPI_CLK(ENABLE, 0);
  clockConfig.startLen = clockDivider;
  clockConfig.stopLen = clockDivider;
  clockConfig.dataPhase0Len = clockDivider;
  clockConfig.dataPhase1Len = clockDivider;
  clockConfig.intervalLen = clockDivider;
  SPI_ClockConfig((SPI_ID_Type)0, &clockConfig);
}

uint8_t SPIClass::transfer(uint8_t data)
{
  SPI_RxIgnoreDisable((SPI_ID_Type)0);
  SPI_SendData((SPI_ID_Type)0, data);
  while(SPI_GetRxFifoCount((SPI_ID_Type)0) == 0){
  }

  return SPI_ReceiveData((SPI_ID_Type)0);
  //return 0;
}

void SPIClass::endTransaction(void)
{
  uint32_t tmpval;
  tmpval = BL_RD_REG(SPI_BASE, SPI_INT_STS); // TODO: Rework
  BL_WR_REG(SPI_BASE,SPI_INT_STS,BL_SET_REG_BIT(tmpval,SPI_CR_SPI_END_CLR));
  SPI_Disable((SPI_ID_Type)0, SPI_WORK_MODE_MASTER);
}