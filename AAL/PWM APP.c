/*
 * APP.c
 *
 * Created: 06/01/2020 13:30:26
 *  Author: Native_programmer
 */ 
#include "SOS.h"
#include "LCD.h"
#include "LCD_PostBConfig.h"
#include "ADC.h"
#include "ADC_PostBConfig.h"
#include "DIO.h"
#include "Timer.h"
#include "Timer_PostBConfig.h"
#define   PWM_State_Inc				0
#define   PWM_State_Dec				1
#define   PWM_NO_STATE				2
#define   NUMBER_PWM_TIMER_2_OUTPUT 4
 
 typedef struct 
 {
	uint8_t u8_PIN;
	uint8_t u8_Duty_cycle;	 
 }str_PWM_NODE_t;
 
 volatile static str_PWM_NODE_t sgastr_PWM_TIMER2[NUMBER_PWM_TIMER_2_OUTPUT]={   
																				 {28,30}
																				,{29,50}
																				,{30,70}
																				,{31,90}
																			  };
													 

/*Tick is determined by Config by choose prescaler value of timer and and count value*/
void PWM_EXECUTE_EVERY_TICK(void)
{
	       uint8_t  u8_Duty_Cycle=50U;/*Outside ISR*/
	       uint16_t su16_PWM_VALUE=(u8_Duty_Cycle*255)/100.0;/*Outside ISR(PWM_Start(duty,pin))*/
	static uint16_t su16_PWM_Counter=0U;/*Inside ISR*/
	static uint8_t  su8_PWM_State=PWM_State_Inc;/*Inside ISR*/

	for(uint8_t u8_index=0;u8_index<NUMBER_PWM_TIMER_2_OUTPUT;u8_index++)
	{
		    
		su16_PWM_VALUE=((sgastr_PWM_TIMER2[u8_index].u8_Duty_cycle*255)/100.0);
		
		if(su16_PWM_VALUE==0U)
		{
			DIO_Write_Pin(sgastr_PWM_TIMER2[u8_index].u8_PIN,0);
			su8_PWM_State=PWM_NO_STATE;
		}
		else if(su16_PWM_VALUE==255U)
		{
			su8_PWM_State=PWM_NO_STATE;			
		}
		else
		{
			/*Do Nothing*/
		}
	}

    switch(su8_PWM_State)
	{
		case PWM_State_Inc:
		{
			for(uint8_t u8_index=0;u8_index<NUMBER_PWM_TIMER_2_OUTPUT;u8_index++)
			{
				su16_PWM_VALUE=((sgastr_PWM_TIMER2[u8_index].u8_Duty_cycle*255)/100.0);
				if(su16_PWM_Counter==su16_PWM_VALUE)
				{
					if(u8_index==0)
					{
						su16_PWM_Counter++;
					}
					DIO_toggle_Pin(sgastr_PWM_TIMER2[u8_index].u8_PIN);
				}

			}
			
			if(su16_PWM_Counter==255U)
			{
				su8_PWM_State=PWM_State_Dec;
				su16_PWM_Counter--;
			}
			else
			{
				su16_PWM_Counter++;
			}

		}
		break;
		
		case PWM_State_Dec:
		{
			for(uint8_t u8_index=0;u8_index<NUMBER_PWM_TIMER_2_OUTPUT;u8_index++)
			{	
				su16_PWM_VALUE=((sgastr_PWM_TIMER2[u8_index].u8_Duty_cycle*255)/100.0);
				if(su16_PWM_Counter==su16_PWM_VALUE)
				{
					DIO_toggle_Pin(sgastr_PWM_TIMER2[u8_index].u8_PIN);
				}
			}
			if(su16_PWM_Counter==1U)
			{
				su8_PWM_State=PWM_State_Inc;
			}

			su16_PWM_Counter--;
		}
		break;
		
		case PWM_NO_STATE:
	    {
			/*Do Nothing*/
		}
		break;		
	}
}

int main()
 {
	 
   Timer_Init(&PWM_TIMER_Init_Config);
   
   Timer_Start(TIMER2,200U,PWM_EXECUTE_EVERY_TICK); 
    /*Counter_Mode*/
    /*Need to configured */
	/*Timer*/
    /*Timer mode*/
    /*Tick Time of PWM (Period=510*t) (t=Period/510) it will be divided into two terms*/
	/*1-Prescaler of timer*/
	/*2-count that ranges from 0-255*/
	/*t=( (count*prescaler)/8000000(FREQY. OF CPU)  )=(Period/510)	*/
	/*Timer*/
	/*PWM*/		
    /*PWM execution->> method Hardware or software*/
    /*PWM mode ->>Phase correct or fast pwm*/
	/*PWM*/
   	for(uint8_t u8_index=0;u8_index<NUMBER_PWM_TIMER_2_OUTPUT;u8_index++)
   	{
	   	DIO_Init_Pin(sgastr_PWM_TIMER2[u8_index].u8_PIN,1);
	   	DIO_Write_Pin(sgastr_PWM_TIMER2[u8_index].u8_PIN,1);
   	}
   	
     
   while(1)
   {	

   }
 }
 
