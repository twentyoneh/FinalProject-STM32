#include "adxl362.h"

static ADXLParams adxlParams;
uint8_t dataRec[6];
//uint16_t x,y,z;
float xg,yg,zg;

volatile bool spiTxComplete = false;
volatile bool spiRxComplete = false;

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

void ADXL362_ReadXYZ_16(uint16_t* x, uint16_t* y,uint16_t* z){
  
    uint8_t rawData[6];
    SPI_ReadRegister(ADXL362_XDATA_L, rawData, 6);
    
  
    *x = (uint16_t)((rawData[1] << 8) | rawData[0]);
    *y = (uint16_t)((rawData[3] << 8) | rawData[2]);
    *z = (uint16_t)((rawData[5] << 8) | rawData[4]);
  
}

void ADXL362_ReadXYZ_8(uint8_t* x, uint8_t* y, uint8_t* z){
  
  uint8_t rawData[3];
    SPI_ReadRegister(ADXL362_XDATA, rawData, 3);
    
  
    *x = (uint8_t) rawData[0];
    *y = (uint8_t) rawData[1];
    *z = (uint8_t) rawData[2];
}

static void SPI_WriteRegister(uint8_t address, uint8_t value) {
  
		if (address > 63)
		address = 63;
		
    uint8_t data[3];
    data[0]= 0x0A;
    data[1]= address;
    data[2]= value;
    
    spiTxComplete = false;
    spiRxComplete = false;
    
    HAL_GPIO_WritePin(ADXLCS_GPIO_Port, ADXLCS_Pin, GPIO_PIN_RESET);
    //HAL_SPI_Transmit(adxlParams.hspi1, data,3,HAL_MAX_DELAY);
    HAL_SPI_Transmit_DMA(adxlParams.hspi1, data,3);
    while (!spiTxComplete) {}
    /*if(HAL_DMA_PollForTransfer(adxlParams.hdma_tx, HAL_DMA_FULL_TRANSFER, 1000)==HAL_ERROR){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    }*/
    //while (HAL_SPI_GetState(adxlParams.hspi1) != HAL_SPI_STATE_READY) {}
    HAL_GPIO_WritePin(ADXLCS_GPIO_Port, ADXLCS_Pin, GPIO_PIN_SET);
}

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
    HAL_SPI_Transmit_DMA(adxlParams.hspi1,commandAndAddress,2);
    while (!spiTxComplete) {}
    //HAL_SPI_TransmitReceive_DMA(adxlParams.hspi1,commandAndAddress,buffer,numBytes);
    //while (HAL_SPI_GetState(adxlParams.hspi1) != HAL_SPI_STATE_READY) {}
    HAL_SPI_Receive_DMA(adxlParams.hspi1,buffer,numBytes);
    while (!spiRxComplete) {}
    
    HAL_GPIO_WritePin(ADXLCS_GPIO_Port, ADXLCS_Pin,GPIO_PIN_SET);
	
	
}
