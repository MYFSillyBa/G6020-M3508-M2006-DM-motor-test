#ifndef __TEST_DM_H
#define __TEST_DM_H

#include "Filter.h"

int float_to_uint(float x_float, float x_min, float x_max, int bits);
void CAN_cmd_Enable(CAN_HandleTypeDef hcan,uint8_t id);
void CAN_cmd_Disable(CAN_HandleTypeDef hcan,uint8_t id);
void CAN_cmd_Clear_Error(CAN_HandleTypeDef hcan,uint8_t id);
void CAN_cmd_Save_Zero(CAN_HandleTypeDef hcan,uint8_t id);
void DM_Send(int16_t Torque,CAN_HandleTypeDef hcan,uint8_t motor_ID);   //MIT

#endif
