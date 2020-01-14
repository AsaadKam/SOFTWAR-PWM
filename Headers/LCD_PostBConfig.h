/*
 * LCD_PostBConfig.h
 *
 * Created: 12/21/2222 19:21:44
 *  Author: Native_programmer
 */ 


#ifndef LCD_POSTBCONFIG_H_
#define LCD_POSTBCONFIG_H_

#include "Data_Types.h"

#define LCD_Execution_Delay_Enable           1
#define LCD_Execution_Timer_Driven_Enable    2
#define LCD_4BIT_COMM                        3
#define LCD_Generate_Cursor_Enable           4

typedef struct
{
	uint8_t LCD_Method_Of_Execution;
	uint8_t LCD_TYPE_PARALLEL_COMM;
	uint8_t LCD_Generate_Cursor;
	
}gstr_LCD_Config_t;

extern gstr_LCD_Config_t gstr_Temprature_Sensor_LCD ;


#endif /* LCD_POSTBCONFIG_H_ */