#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_usart.h>
#include <stm32f0xx_dma.h>
#include <stm32f0xx_misc.h>
#include <stm32f0xx_i2c.h>
#include <stm32f0xx_tim.h>
#include <stm32f0xx_exti.h>
#include <stm32f0xx_syscfg.h>
#include <math.h>
#include "EZUtils.h"

#define I2C_TIMEOUT_MAX 1000

#define IMU_ADDRESS (0x6A << 1)
#define IMU_CMD_BUFFER_SIZE 14

#define MY_ADDRESS 0xC8

volatile uint8_t _IMUBuffer[IMU_CMD_BUFFER_SIZE];

void delay() {

  int i;

  for (i = 0; i < 600000; i++)
    asm("nop");
}

void BlueAlternateLED() {

  if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_15))
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET);
  else
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);
}

void BlueLED(bool status) {

  GPIO_WriteBit(GPIOA, GPIO_Pin_15, (BitAction)!status);
}

int8_t I2CRead(uint8_t reg, uint8_t *buffer, uint8_t bufferStartIndex, uint8_t bytesToRead) {

  uint8_t singleData = 0;

  // Wait until I2C isn't busy
  while (I2C_GetFlagStatus(I2C2, I2C_ISR_BUSY) == SET);

  I2C_TransferHandling(I2C2, IMU_ADDRESS, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

  // Ensure the transmit interrupted flag is set
  while (I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

  // Send the address of the register we wish to write to
  I2C_SendData(I2C2, (uint8_t)reg);

  // Ensure the transmit interrupted flag is set
  while (I2C_GetFlagStatus(I2C2, I2C_ISR_TC) == RESET);

  I2C_TransferHandling(I2C2, IMU_ADDRESS + 1, bytesToRead, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

  for (uint8_t cnt = 0; cnt < bytesToRead; cnt++) {

    while (I2C_GetFlagStatus(I2C2, I2C_ISR_RXNE) == RESET);

    // If we're only reading one byte, place that data direct into the  SingleData variable. 
    // If we're reading more than 1 piece of data store in the array "Data" (a pointer from main) 		
    if (bytesToRead > 1)
      buffer[bufferStartIndex + cnt] = I2C_ReceiveData(I2C2);
    else
      singleData = I2C_ReceiveData(I2C2);
  }

  while (I2C_GetFlagStatus(I2C2, I2C_ISR_STOPF) == RESET);

  // Clear the stop flag for the next potential transfer
  I2C_ClearFlag(I2C2, I2C_FLAG_STOPF);

  // Return a single piece of data if DCnt was less than 1, otherwise 0 will be returned.
  return singleData;
}

void I2CSend(uint8_t Reg, uint8_t Val) {

  //Wait until I2C isn't busy
  while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) == SET);

  I2C_TransferHandling(I2C2, IMU_ADDRESS, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

  //Ensure the transmit interrupted flag is set
  while (I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

  //Send the address of the register we wish to write to
  I2C_SendData(I2C2, (uint8_t)Reg);

  //Ensure the transmit interrupted flag is set
  while (I2C_GetFlagStatus(I2C2, I2C_ISR_TCR) == RESET);

  I2C_TransferHandling(I2C2, IMU_ADDRESS, 1, I2C_AutoEnd_Mode, I2C_No_StartStop);

  //Again, wait until the transmit interrupted flag is set
  while (I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

  //Send the value you wish you write to the register
  I2C_SendData(I2C2, Val);

  while (I2C_GetFlagStatus(I2C2, I2C_ISR_STOPF) == RESET);

  //Clear the stop flag for the next potential transfer
  I2C_ClearFlag(I2C2, I2C_ISR_STOPF);
}

int _WRITE_POS = 0;

#define STUPID_DELAY 2000

extern "C" void I2C1_IRQHandler() {

  BlueAlternateLED();

  if (I2C_GetITStatus(I2C1, I2C_IT_ADDR)) {

    I2C_ClearITPendingBit(I2C1, I2C_IT_ADDR);

    if (I2C_GetTransferDirection(I2C1) == I2C_Direction_Receiver) {

      // Request for us to begin writing

      _WRITE_POS = 0;

      I2C_SendData(I2C1, _IMUBuffer[_WRITE_POS++]);
    }
    else {

      _WRITE_POS = 0;

      // Get the address, which is our own
      uint8_t tmp = I2C_GetAddressMatched(I2C1);
    }
  }

  if (I2C_GetITStatus(I2C1, I2C_IT_RXNE)) {

    // data was sent to us, let's receive it and reset the position to write memory from

    _WRITE_POS = 0;

    uint8_t tmp = I2C_ReceiveData(I2C1);
  }

  if (I2C_GetITStatus(I2C1, I2C_IT_TXIS)) {

    if (_WRITE_POS >= IMU_CMD_BUFFER_SIZE)
      _WRITE_POS = 0;

    I2C_SendData(I2C1, _IMUBuffer[_WRITE_POS++]);
  }

  if (I2C_GetITStatus(I2C1, I2C_IT_NACKF)) 
    I2C_ClearITPendingBit(I2C1, I2C_IT_NACKF);
  
  if (I2C_GetITStatus(I2C1, I2C_IT_STOPF)) {

    _WRITE_POS = 0;

    I2C_ClearITPendingBit(I2C1, I2C_IT_STOPF);
  }

  if (I2C_GetITStatus(I2C1, I2C_IT_BERR)) 
    I2C_ClearITPendingBit(I2C1, I2C_IT_BERR);
  
  if (I2C_GetITStatus(I2C1, I2C_IT_ARLO)) 
    I2C_ClearITPendingBit(I2C1, I2C_IT_ARLO);

  if (I2C_GetITStatus(I2C1, I2C_IT_OVR)) 
    I2C_ClearITPendingBit(I2C1, I2C_IT_OVR);
}

int main() {

  DelayMS(100);

  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  I2C_InitTypeDef i2cInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  // Clocks
  // -------------------------------------------------------------------------------------------------
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  // Interrupt 1 (accel)
  // -------------------------------------------------------------------------------------------------
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // Interrupt 2 (gyro)
  // -------------------------------------------------------------------------------------------------
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // BLUE LED
  // -------------------------------------------------------------------------------------------------
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  BlueLED(false);

  // i2c EZ-B
  // -------------------------------------------------------------------------------------------------

  I2C_DeInit(I2C1);

  // SCL
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // SDA
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_1);

  I2C_StructInit(&i2cInitStructure);
  i2cInitStructure.I2C_OwnAddress1 = MY_ADDRESS;
  i2cInitStructure.I2C_Timing = 0x10800000;
  I2C_Init(I2C1, &i2cInitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = I2C1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  I2C_ITConfig(I2C1, (I2C_IT_ERRI | I2C_IT_STOPI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI), ENABLE);

  I2C_Cmd(I2C1, ENABLE);

  // I2C IMU
  // -------------------------------------------------------------------------------------------------

  I2C_DeInit(I2C2);

  // SCL
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // SDA
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_1);

  I2C_StructInit(&i2cInitStructure);
  i2cInitStructure.I2C_Timing = 0x10805E89;
  I2C_Init(I2C2, &i2cInitStructure);

  I2C_Cmd(I2C2, ENABLE);

  // Initialize IMU
  // -------------------------------------------------------------------------------------------------
  // #CTRL9_XL = 0x38 Acc X,Y,Z enabled
  I2CSend(0x18, 0x38);

  // #CTRL1_XL = 0x60 Acc = 416Hz High performance mode, +8g, 400hz anti alias
  I2CSend(0x10, 0x6F); //c

  // #INT1_CTRL = 0x01 Acc data ready interrupt on INT1
  I2CSend(0x0d, 0x01);

  // #CTRL10_C = 0x38 Gyro X,Y,Z enabled
  I2CSend(0x19, 0x38);

  // #CTRL2_G = 0x60 Gyro = 416Hz High performance mode
  I2CSend(0x11, 0x6C);

  // #INT2_CTRL = 0x02 Gyro data ready interrupt on INT2
  I2CSend(0x0e, 0x02);

  for (int x = 1; x < 8; x++) {

    BlueAlternateLED();

    DelayMS(25);
  }

  // Program Loop
  // -------------------------------------------------------------------------------------------------
  while (true) {

    // Gyro (INT2)
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))
      I2CRead(0x20, (uint8_t *)_IMUBuffer, 0, 8);

    // Accel (INT1)
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))
      I2CRead(0x28, (uint8_t *)_IMUBuffer, 8, 6);
  }
}