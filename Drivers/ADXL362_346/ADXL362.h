#ifndef ADXL362_H
#define ADXL362_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f7xx_hal.h"


#define ADXLCS_Pin GPIO_PIN_4
#define ADXLCS_GPIO_Port GPIOA

// Определение регистров ADXL362
#define ADXL362_DEVID_AD            0x00
#define ADXL362_DEVID_MST           0x01
#define ADXL362_PARTID              0x02
#define ADXL362_REVID               0x03
#define ADXL362_XDATA               0x08
#define ADXL362_YDATA               0x09
#define ADXL362_ZDATA               0x0A
#define ADXL362_STATUS              0x0B
#define ADXL362_FIFO_ENTRIES_L      0x0C
#define ADXL362_FIFO_ENTRIES_H      0x0D
#define ADXL362_XDATA_L             0x0E
#define ADXL362_XDATA_H             0x0F
#define ADXL362_YDATA_L             0x10
#define ADXL362_YDATA_H             0x11
#define ADXL362_ZDATA_L             0x12
#define ADXL362_ZDATA_H             0x13
#define ADXL362_TEMP_L              0x14
#define ADXL362_TEMP_H              0x15
#define ADXL362_SOFT_RESET          0x1F
#define ADXL362_THRESH_ACT_L        0x20
#define ADXL362_THRESH_ACT_H        0x21
#define ADXL362_TIME_ACT            0x22
#define ADXL362_THRESH_INACT_L      0x23
#define ADXL362_THRESH_INACT_H      0x24
#define ADXL362_TIME_INACT_L        0x25
#define ADXL362_TIME_INACT_H        0x26
#define ADXL362_ACT_INACT_CTL       0x27
#define ADXL362_FIFO_CONTROL        0x28
#define ADXL362_FIFO_SAMPLES        0x29
#define ADXL362_INTMAP1             0x2A
#define ADXL362_INTMAP2             0x2B
#define ADXL362_FILTER_CTL          0x2C
#define ADXL362_POWER_CTL           0x2D
#define ADXL362_SELF_TEST           0x2E


typedef enum {ADXL_OK,ADXL_ERR} adxlStatus;

typedef struct{
  SPI_HandleTypeDef* hspi1;
  DMA_HandleTypeDef* hdma_rx;
  DMA_HandleTypeDef* hdma_tx;
  uint8_t m_x;
  uint8_t m_y;
  uint8_t m_z;
  uint8_t m_ADXL362_THRESH_ACT_status;
  uint8_t ADXL362_THRESH_INACT_status;
  uint8_t treshold_reg_status;
  uint8_t soft_res_status;
} ADXLParams;

adxlStatus ADXL362_Init(SPI_HandleTypeDef* handle,DMA_HandleTypeDef* hdma_rx_ptr,DMA_HandleTypeDef* hdma_tx_ptr);
void ADXL362_ReadXYZ_16(uint16_t* x, uint16_t* y, uint16_t* z);
void ADXL362_ReadXYZ_8(uint8_t* x, uint8_t* y, uint8_t* z);
static void SPI_WriteRegister(uint8_t reg, uint8_t value);
static void SPI_ReadRegister(uint8_t address,uint8_t* buffer, uint8_t numBytes);

#endif
