/**
  ******************************************************************************
  * @file           : ADXL362.h
  * @brief Файл объявления функций, макросов, структур. Нужен для работы с акселерометром ADXL362/345
  ******************************************************************************
  */

#ifndef ADXL362_H
#define ADXL362_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f7xx_hal.h"


#define ADXLCS_Pin GPIO_PIN_4         /**< Пин для управления CS акселерометра ADXL362 */
#define ADXLCS_GPIO_Port GPIOA        /**< Порт для управления CS акселерометра ADXL362 */

// Определение регистров ADXL362
#define ADXL362_DEVID_AD            0x00    /**< Регистровый адрес для ID устройства AD */
#define ADXL362_DEVID_MST           0x01    /**< Регистровый адрес для ID устройства MST */
#define ADXL362_PARTID              0x02    /**< Регистровый адрес для ID части устройства */
#define ADXL362_REVID               0x03    /**< Регистровый адрес для ID ревизии устройства */
#define ADXL362_XDATA               0x08    /**< Регистровый адрес для данных по оси X */
#define ADXL362_YDATA               0x09    /**< Регистровый адрес для данных по оси Y */
#define ADXL362_ZDATA               0x0A    /**< Регистровый адрес для данных по оси Z */
#define ADXL362_STATUS              0x0B    /**< Регистровый адрес для статуса устройства */
#define ADXL362_FIFO_ENTRIES_L      0x0C    /**< Регистровый адрес для младшего байта количества данных в FIFO */
#define ADXL362_FIFO_ENTRIES_H      0x0D    /**< Регистровый адрес для старшего байта количества данных в FIFO */
#define ADXL362_XDATA_L             0x0E    /**< Регистровый адрес для младшего байта данных по оси X */
#define ADXL362_XDATA_H             0x0F    /**< Регистровый адрес для старшего байта данных по оси X */
#define ADXL362_YDATA_L             0x10    /**< Регистровый адрес для младшего байта данных по оси Y */
#define ADXL362_YDATA_H             0x11    /**< Регистровый адрес для старшего байта данных по оси Y */
#define ADXL362_ZDATA_L             0x12    /**< Регистровый адрес для младшего байта данных по оси Z */
#define ADXL362_ZDATA_H             0x13    /**< Регистровый адрес для старшего байта данных по оси Z */
#define ADXL362_TEMP_L              0x14    /**< Регистровый адрес для младшего байта температуры */
#define ADXL362_TEMP_H              0x15    /**< Регистровый адрес для старшего байта температуры */
#define ADXL362_SOFT_RESET          0x1F    /**< Регистровый адрес для программного сброса */
#define ADXL362_THRESH_ACT_L        0x20    /**< Регистровый адрес для младшего байта порога активности */
#define ADXL362_THRESH_ACT_H        0x21    /**< Регистровый адрес для старшего байта порога активности */
#define ADXL362_TIME_ACT            0x22    /**< Регистровый адрес для времени активности */
#define ADXL362_THRESH_INACT_L      0x23    /**< Регистровый адрес для младшего байта порога неактивности */
#define ADXL362_THRESH_INACT_H      0x24    /**< Регистровый адрес для старшего байта порога неактивности */
#define ADXL362_TIME_INACT_L        0x25    /**< Регистровый адрес для младшего байта времени неактивности */
#define ADXL362_TIME_INACT_H        0x26    /**< Регистровый адрес для старшего байта времени неактивности */
#define ADXL362_ACT_INACT_CTL       0x27    /**< Регистровый адрес для контроля активности/неактивности */
#define ADXL362_FIFO_CONTROL        0x28    /**< Регистровый адрес для управления FIFO */
#define ADXL362_FIFO_SAMPLES        0x29    /**< Регистровый адрес для количества выборок FIFO */
#define ADXL362_INTMAP1             0x2A    /**< Регистровый адрес для первой карты прерываний */
#define ADXL362_INTMAP2             0x2B    /**< Регистровый адрес для второй карты прерываний */
#define ADXL362_FILTER_CTL          0x2C    /**< Регистровый адрес для управления фильтром */
#define ADXL362_POWER_CTL           0x2D    /**< Регистровый адрес для управления питанием */
#define ADXL362_SELF_TEST           0x2E    /**< Регистровый адрес для самотестирования */


/**
 * @enum adxlStatus
 * @brief Перечисление возможных статусов функций ADXL362
 */
typedef enum {ADXL_OK,ADXL_ERR} adxlStatus;

/**
 * @struct ADXLParams
 * @brief Структура для хранения параметров и состояния ADXL362
 */
typedef struct {
  SPI_HandleTypeDef* hspi1; /**< Указатель на структуру SPI */
  DMA_HandleTypeDef* hdma_rx; /**< Указатель на структуру DMA для приема */
  DMA_HandleTypeDef* hdma_tx; /**< Указатель на структуру DMA для передачи */
  uint8_t m_x; /**< Значение по оси X */
  uint8_t m_y; /**< Значение по оси Y */
  uint8_t m_z; /**< Значение по оси Z */
  uint8_t m_ADXL362_THRESH_ACT_status; /**< Статус порога активности */
  uint8_t ADXL362_THRESH_INACT_status; /**< Статус порога неактивности */
  uint8_t treshold_reg_status; /**< Статус регистра порогов */
  uint8_t soft_res_status; /**< Статус программного сброса */
} ADXLParams;

adxlStatus ADXL362_Init(SPI_HandleTypeDef* handle,DMA_HandleTypeDef* hdma_rx_ptr,DMA_HandleTypeDef* hdma_tx_ptr);
void ADXL362_ReadXYZ_16(uint16_t* x, uint16_t* y, uint16_t* z);
void ADXL362_ReadXYZ_8(uint8_t* x, uint8_t* y, uint8_t* z);
static void SPI_WriteRegister(uint8_t reg, uint8_t value);
static void SPI_ReadRegister(uint8_t address,uint8_t* buffer, uint8_t numBytes);

#endif /* ADXL362_H */
