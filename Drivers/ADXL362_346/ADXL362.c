/**
  ******************************************************************************
  * @file           : adxl362.c
  * @brief          Реализация функций для работы с акселерометром ADXL362.
  * 
  * Этот файл содержит реализацию функций инициализации, чтения данных с акселерометра
  * и функций для работы с SPI.
  ******************************************************************************
  */

#include "adxl362.h"

static ADXLParams adxlParams;

volatile bool spiTxComplete = false;
volatile bool spiRxComplete = false;

/**
  * @brief  Инициализация акселерометра ADXL362.
  * 
  * @param  handle Указатель на структуру SPI.
  * @param  hdma_rx_ptr Указатель на структуру DMA для приема.
  * @param  hdma_tx_ptr Указатель на структуру DMA для передачи.
  * @retval Статус инициализации (ADXL_OK или ADXL_ERR).
  */
adxlStatus ADXL362_Init(SPI_HandleTypeDef* handle,DMA_HandleTypeDef* hdma_rx_ptr,DMA_HandleTypeDef* hdma_tx_ptr) {
  
  adxlParams.hspi1 = handle;
  adxlParams.hdma_rx = hdma_rx_ptr;
  adxlParams.hdma_tx = hdma_tx_ptr;
  SPI_WriteRegister(ADXL362_THRESH_ACT_L,0x00);
  SPI_WriteRegister(ADXL362_TIME_INACT_H,0x00);
  SPI_WriteRegister(ADXL362_TIME_ACT,0x05);
  SPI_WriteRegister(ADXL362_ACT_INACT_CTL,0x00);
  SPI_WriteRegister(ADXL362_FIFO_CONTROL,0x00); //0x00
  SPI_WriteRegister(ADXL362_FIFO_SAMPLES,0x80);
  SPI_WriteRegister(ADXL362_INTMAP1,0x00);
  SPI_WriteRegister(ADXL362_INTMAP2,0x00);
  SPI_WriteRegister(ADXL362_FILTER_CTL,0x13);
  SPI_WriteRegister(ADXL362_POWER_CTL,0x00);

  SPI_WriteRegister(ADXL362_THRESH_ACT_L,0xFA);
  //SPI_ReadRegister(ADXL362_THRESH_ACT_L,&adxlParams.m_ADXL362_THRESH_ACT_status,1);
  SPI_WriteRegister(ADXL362_THRESH_INACT_L,0x96);
  //SPI_ReadRegister(ADXL362_THRESH_INACT_L,&adxlParams.ADXL362_THRESH_INACT_status,1);
  SPI_WriteRegister(ADXL362_TIME_INACT_L,0x1E);
  SPI_WriteRegister(ADXL362_ACT_INACT_CTL,0x3F);
  //SPI_WriteRegister(ADXL362_INTMAP2,0x00); // ???
  SPI_WriteRegister(ADXL362_POWER_CTL,0x0A); //0x0A
  return ADXL_OK;
}

/**
  * @brief  Чтение значений осей X, Y, Z в 16-битном формате.
  * 
  * @param  x Указатель на переменную для хранения значения по оси X.
  * @param  y Указатель на переменную для хранения значения по оси Y.
  * @param  z Указатель на переменную для хранения значения по оси Z.
  */
void ADXL362_ReadXYZ_16(uint16_t* x, uint16_t* y,uint16_t* z){
  
    uint8_t rawData[6]; //Адреса нужных регистров 0x0E - 0x13(Включительно) т.е. 6 байт
    SPI_ReadRegister(ADXL362_XDATA_L, rawData, 6);    
    
    // т.к. Данные одной оси хранятся в формате 12 бит -> мы считываем 6 регистров,
    // получаем 6 байт(по 2 байта на ось). Для корректного вывода сдвинем каждый нечётный элемент получившегося массива
    // на 1 байт влево и с помощью логического И запишем оставшиеся данные в наши переменные, которые в свою очередь имеют размер
    // 16 бит. На выходе получаем 12 битов информации и 4 нуля перед данными (0000 101010101010)
    *x = (uint16_t)((rawData[1] << 8) | rawData[0]);
    *y = (uint16_t)((rawData[3] << 8) | rawData[2]);
    *z = (uint16_t)((rawData[5] << 8) | rawData[4]);
  
}

/**
  * @brief  Чтение значений осей X, Y, Z в 8-битном формате.
  * 
  * @param  x Указатель на переменную для хранения значения по оси X.
  * @param  y Указатель на переменную для хранения значения по оси Y.
  * @param  z Указатель на переменную для хранения значения по оси Z.
  */
void ADXL362_ReadXYZ_8(uint8_t* x, uint8_t* y, uint8_t* z){
  
  uint8_t rawData[3];
    SPI_ReadRegister(ADXL362_XDATA, rawData, 3);
    
  
    *x = (uint8_t) rawData[0];
    *y = (uint8_t) rawData[1];
    *z = (uint8_t) rawData[2];
}

/**
  * @brief  Запись значения в регистр акселерометра.
  * 
  * @param  address Адрес регистра.
  * @param  value Значение для записи.
  */
static void SPI_WriteRegister(uint8_t address, uint8_t value) {
  
		if (address > 63) // Максимальный адрес регистра 62(0x3E)
		address = 63;
		
    uint8_t data[3];  // Команда + Адрес + Данные
    data[0]= 0x0A;  // Команда для записи 
    data[1]= address; // Адрес регистра
    data[2]= value; // Данные для записи 
    
    spiTxComplete = false;  
    
    HAL_GPIO_WritePin(ADXLCS_GPIO_Port, ADXLCS_Pin, GPIO_PIN_RESET);  // Выключаем CS (NSS в случае мастера) для начала передачи команды 
    HAL_SPI_Transmit_DMA(adxlParams.hspi1, data,3); // Занесение в регистр данных
    while (!spiTxComplete) {} // В момент откончания передачи данных по SPI вызывается прерывание HAL_SPI_TxCpltCallback и меняет spiTxComplete = true
    HAL_GPIO_WritePin(ADXLCS_GPIO_Port, ADXLCS_Pin, GPIO_PIN_SET);  // Включаем CS (NSS в случае мастера) для окончания передачи команды 
}

/**
  * @brief  Чтение значений из регистра.
  * 
  * @param  address Адрес регистра.
  * @param  value Значение для записи.
  */
static void SPI_ReadRegister(uint8_t address,uint8_t* buffer, uint8_t numBytes)
{
  
		if (address > 63)
		address = 63;
    
    uint8_t commandAndAddress[2];
    commandAndAddress[0] = 0x0B;
    commandAndAddress[1] = address;
    
    spiTxComplete = false;
    spiRxComplete = false;
    
    HAL_GPIO_WritePin(ADXLCS_GPIO_Port, ADXLCS_Pin,GPIO_PIN_RESET);
    HAL_SPI_Transmit_DMA(adxlParams.hspi1,commandAndAddress,2); // Отправка команды на чтение
    while (!spiTxComplete) {}
    HAL_SPI_Receive_DMA(adxlParams.hspi1,buffer,numBytes);  // Получение значений из регистра
    while (!spiRxComplete) {} // В момент откончания передачи данных по SPI вызывается прерывание HAL_SPI_RxCpltCallback и меняет spiTxComplete = true
    
    HAL_GPIO_WritePin(ADXLCS_GPIO_Port, ADXLCS_Pin,GPIO_PIN_SET);
}
