/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Filter.h"
#include "stdio.h"
#include "test_DM.h"
#include "String.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define pi 3.141592653589793238462643383279f
#define get_motor_measure_DM(ptr, data)				  																									\
		{											  																																			\
			(ptr)->ID 			 	  	= data[0]&0x0F;																												\
			(ptr)->ERR				  	= (data[0]>>4)&0x0F;																									\
			(ptr)->DM_ecd			 	= (uint16_t)((uint16_t)data[1]<<8|(uint16_t)data[2])/65536.0*pi-pi/2.f;	\
			(ptr)->DM_speed_rpm  = ((uint16_t)data[3]<<4|(uint16_t)data[4]>>4)/4096.0*60-30;						\
			(ptr)->Torque		 	  	= ((uint16_t)(data[4]&0x0F)<<8|(uint16_t)data[5])/4096.0*20-10;				\
			(ptr)->T_MOS		 	  	= data[6];																														\
			(ptr)->T_Rotor	 			= data[7];																														\
		}
		
#define get_motor_measure_DJI(ptr, data)                                \
    {                                                                   \
      (ptr)->last_ecd 		   = (ptr)->ecd;                              \
      (ptr)->ecd 					   = (uint16_t)((data)[0] << 8 | (data)[1]);  \
      (ptr)->speed_rpm 		   = (uint16_t)((data)[2] << 8 | (data)[3]);  \
      (ptr)->given_current   = (uint16_t)((data)[4] << 8 | (data)[5]);  \
      (ptr)->temperate 		   = (data)[6];                               \
    }
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */	
#define POS_ERROR_SUM_LIM 500.0f         //pos_error_sum限幅
#define TARGET_SPEED_LIM 300.0f          //target_speed限幅
#define SPEED_ERROR_SUM_LIM 800.0f       //speed_error_sum限幅
#define OUTPUT_LIM 20000                 //output限幅
		
float actual_speed[16];
float actual_angle[16];
		
uint8_t dm_id = 0;
uint16_t suo = 0;
uint8_t rx_data[8]={0};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
typedef struct motor32_struct    //一些参数，PID用,[id-1]为对应id电机参数
{
    float speed_error_sum;
    float speed_last_error;
    float pos_error_sum;
    float pos_last_error;
} motor32_measure_t;

motor32_measure_t motor[16];

typedef struct  motor_struct       //电机反馈参数,[id-1]为对应id电机参数
{
	uint16_t ecd;
	int16_t speed_rpm;
	int16_t given_current;
	uint8_t temperate;
	int16_t last_ecd;
	int16_t flag;
	int32_t flag_ecd;

	uint8_t ID;
	uint8_t ERR;

	uint8_t T_MOS;
	uint8_t T_Rotor;
	float DM_ecd;
	float	DM_speed_rpm;
	float	Torque;
	
	void    (*clear_cnt )(struct motor_struct *self);
} motor_measure_t;

motor_measure_t can2_motor[16];
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float SetTargetangle1(float angle);
int16_t Pos_PID1(float kp, float ki, float kd,float target_angle,uint8_t id,char motor_type);
int16_t Speed_PID1(float kp, float ki, float kd,float target_speed,uint8_t id,char motor_type);
void RM_M3508M2006G6020_Control(int16_t torque,CAN_HandleTypeDef hcan,uint8_t id,char motor_type,char mode,float target_Speed,float target_Angle);
void motor_clear_cnt(motor_measure_t *self);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	Filter();
	for(int i=0;i<16;i++)
	{
		can2_motor[i].clear_cnt=motor_clear_cnt;
		actual_speed[i]=0;
		actual_angle[i]=0;
	}
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
int16_t Pos_PID1(float kp, float ki, float kd,float target_angle,uint8_t id,char motor_type)
{
		float target_speed = 0.0f;
		uint8_t arr = 0;
	
		if(motor_type=='M'||motor_type=='m') arr = id-1;
		else if(motor_type=='G'||motor_type=='g') arr = 3+id;
	
    float pos_error = target_angle - actual_angle[arr];
	
		if (pos_error > 4096)  pos_error -= 8192;
		if (pos_error < -4096) pos_error += 8192;
		
    motor[arr].pos_error_sum += pos_error;
    
    if(motor[arr].pos_error_sum > POS_ERROR_SUM_LIM)    motor[arr].pos_error_sum = POS_ERROR_SUM_LIM;
    if(motor[arr].pos_error_sum < -POS_ERROR_SUM_LIM)   motor[arr].pos_error_sum = -POS_ERROR_SUM_LIM;
    
    float pos_change = pos_error - motor[arr].pos_last_error;
    
    target_speed = kp * pos_error + ki * motor[arr].pos_error_sum + kd * pos_change;
    
    if(target_speed > TARGET_SPEED_LIM) target_speed = TARGET_SPEED_LIM;
    if(target_speed < -TARGET_SPEED_LIM) target_speed = -TARGET_SPEED_LIM;
    
    motor[arr].pos_last_error = pos_error;
		return target_speed;
}

int16_t Speed_PID1(float kp, float ki, float kd,float target_speed,uint8_t id,char motor_type)
{
	uint8_t arr = 0;
	if(motor_type=='M'||motor_type=='m') arr = id-1;
	else if(motor_type=='G'||motor_type=='g') arr = 3+id;
	
    float speed_error = target_speed - actual_speed[arr];
    motor[arr].speed_error_sum += speed_error;
    
    if(motor[arr].speed_error_sum > SPEED_ERROR_SUM_LIM)    motor[arr].speed_error_sum = SPEED_ERROR_SUM_LIM;
    if(motor[arr].speed_error_sum < -SPEED_ERROR_SUM_LIM)   motor[arr].speed_error_sum = -SPEED_ERROR_SUM_LIM;
    
    float speed_change = speed_error - motor[arr].speed_last_error;
    
    float output = kp * speed_error + ki * motor[arr].speed_error_sum + kd * speed_change;
    
    if(output > OUTPUT_LIM) output = OUTPUT_LIM;
    if(output < -OUTPUT_LIM) output = -OUTPUT_LIM;
    
    motor[arr].speed_last_error = speed_error;
		return output;
}

void RM_M3508M2006G6020_Control(int16_t torque,CAN_HandleTypeDef hcan,uint8_t id,char motor_type,char mode,float target_Speed,float target_Angle)
{
	uint32_t StdID = 0;
	if(motor_type=='M'||motor_type=='m')      {  if(id>=1&&id<=4) StdID = 0x200;   else if(id>=5&&id<=8) StdID = 0x1FF;}
	else if(motor_type=='G'||motor_type=='g') {  if(id>=1&&id<=4) StdID = 0x1FF;   else if(id>=5&&id<=8) StdID = 0x2FF;}
	
	int32_t Torque = 0;
	
	if(mode=='p')     //闭角度PID
	{
		int16_t target_speed = Pos_PID1(0.45f, 0.00002f, 0.0f,target_Angle,id,motor_type);
    Torque = Speed_PID1(7.0f, 0.08f, 8.0f,target_speed,id,motor_type);
	}
	else if(mode=='s')          //闭速度PID
	{
		Torque = Speed_PID1(7.0f, 0.08f, 8.0f,target_Speed,id,motor_type);
	}
	else if(mode=='0')          //没有PID
	{
		Torque = torque;
	}
	/*-----------发送部分-----------*/
		CAN_TxHeaderTypeDef tx_header;
    uint32_t mailbox;
    uint8_t data[8] = {0};
    
    tx_header.StdId = StdID;
    tx_header.ExtId = 0;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;
    tx_header.TransmitGlobalTime = DISABLE;

		uint8_t data_index;
    if (id <= 4) {
        data_index = (id - 1) * 2;
    } else {
        data_index = (id - 5) * 2;
    }
    
    data[data_index] = (Torque >> 8) & 0xFF;
    data[data_index + 1] = Torque;
		
		/*data[0]=(Torque>>8) & 0xFF;
		data[1]=Torque & 0xFF;
		data[2]=(Torque>>8)& 0xFF;
		data[3]=Torque & 0xFF;
		data[4]=(Torque>>8) & 0xFF;
		data[5]=Torque & 0xFF;
		data[6]=(Torque>>8) & 0xFF;
		data[7]=Torque & 0xFF;*/
		
		HAL_CAN_AddTxMessage(&hcan, &tx_header, data, &mailbox);

}
void motor_clear_cnt(motor_measure_t *self)
	{
    
    self->flag = 0;
    self->flag_ecd = 0;
    self->last_ecd = self->ecd;
	}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
	
		if(hcan->Instance==CAN1||hcan->Instance==CAN2)
		{
			switch(rx_header.StdId)
			{
				case 0x201:
					get_motor_measure_DJI(&can2_motor[0], rx_data);
					can2_motor[0].clear_cnt(&can2_motor[0]);
					actual_angle[0] = can2_motor[0].ecd;
					actual_speed[0] = can2_motor[0].speed_rpm;
				break;
				
				case 0x202:
					get_motor_measure_DJI(&can2_motor[1], rx_data);
					can2_motor[1].clear_cnt(&can2_motor[1]);
					actual_angle[1] = can2_motor[1].ecd;
					actual_speed[1] = can2_motor[1].speed_rpm;
				break;
				
				case 0x203:
					get_motor_measure_DJI(&can2_motor[2], rx_data);
					can2_motor[2].clear_cnt(&can2_motor[2]);
					actual_angle[2] = can2_motor[2].ecd;
					actual_speed[2] = can2_motor[2].speed_rpm;
				break;
				
				case 0x204:
					get_motor_measure_DJI(&can2_motor[3], rx_data);
					can2_motor[3].clear_cnt(&can2_motor[3]);
					actual_angle[3] = can2_motor[3].ecd;
					actual_speed[3] = can2_motor[3].speed_rpm;
				break;
				
				case 0x205:
					get_motor_measure_DJI(&can2_motor[4], rx_data);
					can2_motor[4].clear_cnt(&can2_motor[4]);
					actual_angle[4] = can2_motor[4].ecd;
					actual_speed[4] = can2_motor[4].speed_rpm;
				break;
				
				case 0x206:
					get_motor_measure_DJI(&can2_motor[5], rx_data);
					can2_motor[5].clear_cnt(&can2_motor[5]);
					actual_angle[5] = can2_motor[5].ecd;
					actual_speed[5] = can2_motor[5].speed_rpm;
				break;
				
				case 0x207:
					get_motor_measure_DJI(&can2_motor[6], rx_data);
					can2_motor[6].clear_cnt(&can2_motor[6]);
					actual_angle[6] = can2_motor[6].ecd;
					actual_speed[6] = can2_motor[6].speed_rpm;
				break;
				
				case 0x208:
					get_motor_measure_DJI(&can2_motor[7], rx_data);
					can2_motor[7].clear_cnt(&can2_motor[7]);
					actual_angle[7] = can2_motor[7].ecd;
					actual_speed[7] = can2_motor[7].speed_rpm;
				break;
				
				case 0x012:                                         //DM4310反馈
					dm_id = rx_data[0]&0x0F;
					suo = (dm_id + 7);
					get_motor_measure_DM(&can2_motor[suo], rx_data);
					can2_motor[suo].clear_cnt(&can2_motor[suo]);
				break;
			}
		}
}

float SetTargetangle1(float angle)
{
    int32_t target_angle =(angle / 360.0f) * 8191.0f;
	return target_angle;
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
