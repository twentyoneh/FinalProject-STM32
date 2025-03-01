/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief Основной файл программы, включающий в себя функции инициализации периферии, основной цикл программы и обработку данных.
  * 
  * Этот файл содержит в себе основной цикл программы, инициализацию всех необходимых периферийных устройств и обработку данных,
  * получаемых от акселерометра ADXL362 и передачу данных по UART.
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ADXL362.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
///Размер буфера для передачи по UART
#define UART_TX_BUFFER_SIZE  7

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

/// Флаг успешной передачи данных по SPI (передача завершена). Объявлена в файле ADXL362.c
extern bool spiTxComplete;
/// Флаг успешного приема данных по SPI (прием завершен). Объявлена в файле ADXL362.c
extern bool spiRxComplete;
/// Флаг успешной передачи данных оп UART.
bool uartTxComplete = false;

uint8_t rightXOut[] = {82, 73, 71, 72, 84, 00};
uint8_t leftXOut[] = {76, 69, 70, 84, 00, 00};
uint8_t upYOut[] = {89, 32, 85, 80, 00, 00};
uint8_t downYOut[] = {89, 32, 68, 79, 87, 78};
uint8_t upZOut[] = {90, 32, 85, 80, 00, 00};
uint8_t downZOut[] = {90, 32, 68, 79, 87, 78};
uint8_t* uartOut[6] = {rightXOut,leftXOut,upYOut,downYOut,upZOut,downZOut}; ///<Для вывода по UART>


/// Буфер XDATA для передачи по UART
uint8_t xBuff[UART_TX_BUFFER_SIZE] = "X: "; 
/// Буфер YDATA для передачи по UART
uint8_t yBuff[UART_TX_BUFFER_SIZE] = "Y: "; 
/// Буфер ZDATA для передачи по UART
uint8_t zBuff[UART_TX_BUFFER_SIZE] = "Z: "; 
/// Финальный буфер для передачи по UART, содержащий данные из xBuff, yBuff и zBuff
uint8_t txBuff[UART_TX_BUFFER_SIZE*3+2];    
/// Индекс буфера передачи данных по UART
uint8_t index_b = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void OLED_Move_Up_Z(SSD1306_COLOR);
void OLED_Move_Down_Z(SSD1306_COLOR);
void OLED_Move_Up(SSD1306_COLOR);
void OLED_Move_Down(SSD1306_COLOR);
void OLED_Move_Right(SSD1306_COLOR);
void OLED_Move_Left(SSD1306_COLOR);
void CreateTxBuff(uint8_t*, uint8_t*, uint8_t* ,uint8_t* );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief      Проверка работоспособности платы
 * 
 */
  void ping(){
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
  HAL_Delay(100);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init(); // Инициализация OLED дисплея 
  ADXL362_Init(&hspi1,&hdma_spi1_rx,&hdma_spi1_tx); // Инициализация акселерометра ADXL362 

  ping();
  ping();
  // Объявление и инициализация переменных для хранения данных акселерометра
  //uint16_t uint16x = 0,uint16y = 0,uint16z = 0;
  uint8_t uint8x = 0,uint8y = 0,uint8z = 0;
  uint8_t uint8x1 = 0,uint8y1 = 0,uint8z1 = 0;
  int8_t delta = 0;
  uint8_t flag_xyz;
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  while (1)
  {
   
    HAL_Delay(500);
    // Чтение данных с регистрова ADXL362
    ADXL362_ReadXYZ_8(&uint8x,&uint8y,&uint8z); 

    // Определение оси с наибольшим изменением
    delta = 0;
    if(abs(uint8x - uint8x1) > abs(uint8y - uint8y1) && abs(uint8x - uint8x1) > abs(uint8z - uint8z1)){
      flag_xyz = 0;
      delta = uint8x - uint8x1;
    } else 
    if(abs(uint8y - uint8y1) > abs(uint8x - uint8x1) && abs(uint8y - uint8y1) > abs(uint8z - uint8z1)){
      flag_xyz = 1;
      delta = uint8y - uint8y1;
    } else {
      flag_xyz = 2;
      delta = (uint8z - uint8z1)*5;
    } 
    index_b = 6;
    // В зависимости от разницы начальной и конечной координат рисуем стрелки / кружочки
    if(delta < -10 || delta > 10){
    switch(flag_xyz){
      
      // Изменения по оси X
      case 0:
          if(delta > 0){
          ssd1306_FillRectangle(0,0,128,64,Black);
          OLED_Move_Right(White);
          index_b = 0;
          }
          else{
          ssd1306_FillRectangle(0,0,128,64,Black);
          OLED_Move_Left(White);
          index_b = 1;
          }
    break;
          
      // Изменения по оси Y
      case 1:
          if(delta > 0){
          ssd1306_FillRectangle(0,0,128,64,Black);
          OLED_Move_Up(White);
          index_b = 2;
          }
          else{
          ssd1306_FillRectangle(0,0,128,64,Black);
          OLED_Move_Down(White);
          index_b = 3;
          }
          break;
          
      // Изменения по оси Z
      case 2:
        if(delta > 0){
          ssd1306_FillRectangle(0,0,128,64,Black);
          OLED_Move_Up_Z(White);
          index_b = 4;
          }
          else{
          ssd1306_FillRectangle(0,0,128,64,Black);
          OLED_Move_Down_Z(White);
          index_b = 5;
          }
          break;
      }
    }
        else{
         ssd1306_FillRectangle(0,0,128,64,Black);
        }
        
    ssd1306_UpdateScreen(); //Обновление экрана
        
        if(index_b!=6){
          HAL_UART_Transmit_DMA(&huart2, uartOut[index_b], 6);
        }
        
    
    
    //ADXL362_ReadXYZ_16(&uint16x,&uint16y,&uint16z);
    /*
    index_b=5; // Индекс для заполнения массива 

    // Цикл, который заполняет массивы из которых соберём массив для передачи по UART
    do
    {
      xBuff[index_b]=uint8x%10 + 48;  // Запись в ASCII 
      yBuff[index_b]=uint8y%10 + 48;
      zBuff[index_b]=uint8z%10 + 48;
      if(uint8x!=0){
        uint8x/=10;
      }
      if(uint8y!=0){
        uint8y/=10;
      }
      if(uint8z!=0){
        uint8z/=10;
      }
      index_b--;
    }while(uint8x || uint8y || uint8z);
    
    xBuff[6] = ';';
    yBuff[6] = ';';
    zBuff[6] = ' ';
    
    CreateTxBuff(xBuff,yBuff,zBuff,txBuff); //Создаём массив для передачи
    
    HAL_UART_Transmit_DMA(&huart2, txBuff, UART_TX_BUFFER_SIZE*3+2);  //Передаём получившийся массив
    */
    ADXL362_ReadXYZ_8(&uint8x1,&uint8y1,&uint8z1);  //Считываем данные с акселерометра 
   
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1;
  PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x20404768;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : User_Button_Pin */
  GPIO_InitStruct.Pin = User_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(User_Button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SS_Pin */
  GPIO_InitStruct.Pin = SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : led2_Pin */
  GPIO_InitStruct.Pin = led2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(led2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/**
 * @brief  Собирает буфер для передачи данных по USART.
 * @note   На вход приходят 3 переменные (значения регистров XDATA, YDATA, ZDATA). На выходе получаем регистр размером UART_TX_BUFFER_SIZE*3+2
 * @param[in]  xb    Указатель на буфер с данными XDATA.
 * @param[in]  yb    Указатель на буфер с данными YDATA.
 * @param[in]  zb    Указатель на буфер с данными ZDATA.
 * @param[in,out]  tb  Указатель на буфер для передачи данных.
 */
void CreateTxBuff(uint8_t* xb,uint8_t* yb,uint8_t* zb,uint8_t* tb){
  for(int i = 0; i < UART_TX_BUFFER_SIZE; i++){
    tb[i] = xb[i];
    tb[i+UART_TX_BUFFER_SIZE] = yb[i];
    tb[i+UART_TX_BUFFER_SIZE*2] = zb[i];
  }
  tb[UART_TX_BUFFER_SIZE*3] = '\r';
  tb[UART_TX_BUFFER_SIZE*3+1] = '\n';
}

/**
 * @brief  Обратный вызов по завершению передачи половины данных по UART.
 *
 * @param[in]  huart  Указатель на структуру UART_HandleTypeDef
 */
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart){
  
}

/**
 * @brief  Обратный вызов по завершению передачи данных по UART.
 *
 * @param[in]  huart  Указатель на структуру UART_HandleTypeDef
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  if(huart == &huart2){
    //uartTxComplete = true;
  }
}

/**
 * @brief  Обратный вызов при ошибке передачи по UART.
 *
 * @param[in]  huart  Указатель на структуру UART_HandleTypeDef
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart2)
        {
                uint32_t er = HAL_UART_GetError(&huart2);
 
                if (er & HAL_UART_ERROR_PE)
                {
                        HAL_UART_Transmit(&huart2, (uint8_t*) "ERR_Callbck_UART - Parity error\n", 31, 1000);
                        __HAL_UART_CLEAR_PEFLAG(&huart2);
                }
                if (er & HAL_UART_ERROR_NE)
                {
                        HAL_UART_Transmit(&huart2, (uint8_t*) "ERR_Callbck_UART - Noise error\n", 30, 1000);
                        __HAL_UART_CLEAR_NEFLAG(&huart2);
                }
                if (er & HAL_UART_ERROR_FE)
                {
                        HAL_UART_Transmit(&huart2, (uint8_t*) "ERR_Callbck_UART - Frame error\n", 30, 1000);
                        __HAL_UART_CLEAR_FEFLAG(&huart2);
                }
                if (er & HAL_UART_ERROR_ORE)
                {
                        HAL_UART_Transmit(&huart2, (uint8_t*) "ERR_Callbck_UART - Overrun error\n", 32, 1000);
                        __HAL_UART_CLEAR_OREFLAG(huart);
                }
                if (er & HAL_UART_ERROR_DMA)
                {
                        HAL_UART_Transmit(&huart2, (uint8_t*) "ERR_Callbck_UART - DMA transfer error\n", 38, 1000);
                        __HAL_UART_CLEAR_NEFLAG(&huart2);
                }
                huart->ErrorCode = HAL_UART_ERROR_NONE;
        }
}

/**
 * @brief  Обратный вызов при ошибке передачи по SPI.
 *
 * @param[in]  hspi  Указатель на структуру SPI_HandleTypeDef
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi)
{
  ping();
  if (hspi == &hspi1)
    {
        uint32_t er = HAL_SPI_GetError(hspi);

        if (er & HAL_SPI_ERROR_MODF)
        {
            HAL_UART_Transmit(&huart2, (uint8_t *)"ERR_Callback_SPI - Mode Fault error\n", 35, 1000);
            __HAL_SPI_CLEAR_MODFFLAG(hspi);
        }
        if (er & HAL_SPI_ERROR_CRC)
        {
            HAL_UART_Transmit(&huart2, (uint8_t *)"ERR_Callback_SPI - CRC error\n", 28, 1000);
            __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
        }
        if (er & HAL_SPI_ERROR_OVR)
        {
            HAL_UART_Transmit(&huart2, (uint8_t *)"ERR_Callback_SPI - Overrun error\n", 32, 1000);
            __HAL_SPI_CLEAR_OVRFLAG(hspi);
        }
        if (er & HAL_SPI_ERROR_FRE)
        {
            HAL_UART_Transmit(&huart2, (uint8_t *)"ERR_Callback_SPI - Frame Format error\n", 37, 1000);
            __HAL_SPI_CLEAR_FREFLAG(hspi);
        }
        if (er & HAL_SPI_ERROR_DMA)
        {
            HAL_UART_Transmit(&huart2, (uint8_t *)"ERR_Callback_SPI - DMA transfer error\n", 37, 1000);
            // Дополнительные действия при ошибке DMA могут быть добавлены здесь при необходимости
        }

        // Сброс всех флагов ошибок
        hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    
        }
}

/**
 * @brief  Обратный вызов по завершению передачи данных по SPI.
 *
 * @param[in]  hspi  Указатель на структуру SPI_HandleTypeDef
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1)
    {
        spiTxComplete = true;
    }
}

/**
 * @brief  Обратный вызов по завершению приема данных по SPI.
 *
 * @param[in]  hspi  Указатель на структуру SPI_HandleTypeDef
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1)
    {
        spiRxComplete = true;
    }
}

/**
 * @brief  Обратный вызов по завершению приема половины данных по SPI.
 *
 * @param[in]  hspi  Указатель на структуру SPI_HandleTypeDef
 */
void HAL_SPI_TxHalfCpltCallback	(	SPI_HandleTypeDef * 	hspi	)	{
  
}

/**
 * @brief  Обратный вызов по завершению передачи половины данных по SPI.
 *
 * @param[in]  hspi  Указатель на структуру SPI_HandleTypeDef
 */
void HAL_SPI_RxHalfCpltCallback	(	SPI_HandleTypeDef * 	hspi	)	{


}

/**
 * @brief  Отображение круга на OLED дисплее для перемещения вверх по оси Z.
 *
 * @param[in]  color  Цвет круга
 */
void OLED_Move_Up_Z(SSD1306_COLOR color){
  ssd1306_DrawCircle(64,32,1,color);
  ssd1306_DrawCircle(64,32,10,color);
}

/**
 * @brief  Отображение линий на OLED дисплее для перемещения вниз по оси Z.
 *
 * @param[in]  color  Цвет линий
 */
void OLED_Move_Down_Z(SSD1306_COLOR color){
  ssd1306_Line(32,16, 96,48,White);
  ssd1306_Line(32,48, 96,16,White);
}

/**
 * @brief  Отображение линий на OLED дисплее для перемещения вверх.
 *
 * @param[in]  color  Цвет линий
 */
void OLED_Move_Up(SSD1306_COLOR color){
  ssd1306_Line(64,0, 64,64,color);
  ssd1306_Line(65,0, 65,65,color);
  ssd1306_Line(64,0, 32,32,color);
  ssd1306_Line(65,0, 96,32,color);
}

/**
 * @brief  Отображение линий на OLED дисплее для перемещения вниз.
 *
 * @param[in]  color  Цвет линий
 */
void OLED_Move_Down(SSD1306_COLOR color){
  ssd1306_Line(64,0, 64,64,color);
  ssd1306_Line(65,0, 65,65,color);
  ssd1306_Line(64,64, 32,32,color);
  ssd1306_Line(65,65, 96,32,color);
}

/**
 * @brief  Отображение линий на OLED дисплее для перемещения вправо.
 *
 * @param[in]  color  Цвет линий
 */
void OLED_Move_Right(SSD1306_COLOR color){
  ssd1306_Line(0,32, 128,32,color);
  ssd1306_Line(0,31, 128,31,color);
  ssd1306_Line(128,32, 96,16,color);
  ssd1306_Line(128,31, 96,48,color);
}

/**
 * @brief  Отображение линий на OLED дисплее для перемещения влево.
 *
 * @param[in]  color  Цвет линий
 */
void OLED_Move_Left(SSD1306_COLOR color){
  ssd1306_Line(0,32, 128,32,color);
  ssd1306_Line(0,31, 128,31,color);
  ssd1306_Line(0,32, 32,16,color);
  ssd1306_Line(0,31, 32,48,color);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
