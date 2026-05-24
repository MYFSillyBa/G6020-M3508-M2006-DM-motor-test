#include "test_DM.h"

CAN_TxHeaderTypeDef damiao_TxMessage;
uint8_t damiao_data[8];

int float_to_uint(float x_float, float x_min, float x_max, int bits)
{
    /* Converts a float to an unsigned int, given range and number of bits */
    float span = x_max - x_min;
    float offset = x_min;
    return (int) ((x_float-offset)*((float)((1<<bits)-1))/span);
}

void DM_Send(int16_t Torque,CAN_HandleTypeDef hcan,uint8_t motor_ID)   //MIT
{
	CAN_TxHeaderTypeDef TxMessage;
	uint32_t mail_box;
	uint8_t data[8];
	
	static uint16_t Torquee;	
	Torquee = float_to_uint(Torque,-40.f,40.f,12);
	
	TxMessage.StdId = motor_ID;
	TxMessage.ExtId = 0;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 0x08;
	
	data[0]=0;
	data[1]=0;
	data[2]=0;
	data[3]=0;
	data[4]=0;
	data[5]=0;
	data[6]=(Torquee>>8)&0x0F;
	data[7]=Torquee;
	
	if(HAL_CAN_AddTxMessage(&hcan,&TxMessage,data,&mail_box)!=HAL_OK)
	{
			HAL_CAN_AddTxMessage(&hcan,&TxMessage,data,&mail_box);
	}
}

void CAN_cmd_Enable(CAN_HandleTypeDef hcan,uint8_t id)
{
		damiao_TxMessage.StdId = 0x100+id;
		damiao_TxMessage.IDE = CAN_ID_STD;
		damiao_TxMessage.RTR = CAN_RTR_DATA;
		damiao_TxMessage.DLC = 0x08;

		damiao_data[0] = 0xFF;
		damiao_data[1] = 0xFF;
		damiao_data[2] = 0xFF;
		damiao_data[3] = 0xFF;
		damiao_data[4] = 0xFF;
		damiao_data[5] = 0xFF;
		damiao_data[6] = 0xFF;
		damiao_data[7] = 0xFC;
	
		if(HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX0) != HAL_OK){
		if(HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX1) != HAL_OK){
			 HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX2);}}
}

void CAN_cmd_Disable(CAN_HandleTypeDef hcan,uint8_t id)
{
		damiao_TxMessage.StdId = 0x100+id;
		damiao_TxMessage.IDE = CAN_ID_STD;
		damiao_TxMessage.RTR = CAN_RTR_DATA;
		damiao_TxMessage.DLC = 0x08;

		damiao_data[0] = 0xFF;
		damiao_data[1] = 0xFF;
		damiao_data[2] = 0xFF;
		damiao_data[3] = 0xFF;
		damiao_data[4] = 0xFF;
		damiao_data[5] = 0xFF;
		damiao_data[6] = 0xFF;
		damiao_data[7] = 0xFD;
	
		if(HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX0) != HAL_OK){
		if(HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX1) != HAL_OK){
			 HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX2);}}
}

void CAN_cmd_Clear_Error(CAN_HandleTypeDef hcan,uint8_t id)
{
		damiao_TxMessage.StdId = 0x100+id;
		damiao_TxMessage.IDE = CAN_ID_STD;
		damiao_TxMessage.RTR = CAN_RTR_DATA;
		damiao_TxMessage.DLC = 0x08;

		damiao_data[0] = 0xFF;
		damiao_data[1] = 0xFF;
		damiao_data[2] = 0xFF;
		damiao_data[3] = 0xFF;
		damiao_data[4] = 0xFF;
		damiao_data[5] = 0xFF;
		damiao_data[6] = 0xFF;
		damiao_data[7] = 0xFB;
	
		if(HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX0) != HAL_OK){
		if(HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX1) != HAL_OK){
			 HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX2);}}
}

void CAN_cmd_Save_Zero(CAN_HandleTypeDef hcan,uint8_t id)
{
		damiao_TxMessage.StdId = 0x100+id;
		damiao_TxMessage.IDE = CAN_ID_STD;
		damiao_TxMessage.RTR = CAN_RTR_DATA;
		damiao_TxMessage.DLC = 0x08;

		damiao_data[0] = 0xFF;
		damiao_data[1] = 0xFF;
		damiao_data[2] = 0xFF;
		damiao_data[3] = 0xFF;
		damiao_data[4] = 0xFF;
		damiao_data[5] = 0xFF;
		damiao_data[6] = 0xFF;
		damiao_data[7] = 0xFE;
	
		if(HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX0) != HAL_OK){
		if(HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX1) != HAL_OK){
			 HAL_CAN_AddTxMessage(&hcan, &damiao_TxMessage, damiao_data, (uint32_t*) CAN_TX_MAILBOX2);}}
}
