/*
 * LCD.c
 *
 *  Created on: Apr 29, 2219
 *      Author: LENOVO
 */


#include "LCD.h"
#include "Timer.h"
#include "Timer_PostBConfig.h"



static uint8_t sgu8_LCD_Execution_Time=LCD_Execution_Delay_Enable;

/*
 * Init LCD.
 */
void LCD_init(gstr_LCD_Config_t* Copy_PntrStr_LCD_Config ,uint8_t* pu8_flag)
{
	  static uint8_t su8_LCD_command_state=1;
	  static uint8_t su8_LCD_init_state=0;
	
	
	  if((Copy_PntrStr_LCD_Config->LCD_Method_Of_Execution)==LCD_Execution_Delay_Enable)
	  {
		   
		   sgu8_LCD_Execution_Time=LCD_Execution_Delay_Enable;
			/*
			 * init pins.
			 * RS = 0 To send CMD for initialization LCD.
			 * RW = 0 To Write CMD To LCD.
			 * E = high to low pulses.
			 * CMD .. for 8-bit Mode.
	 			 * 0x38 for 5 x 8 Matrix (2 Line .. 8 bit).
	 			 * 0x0E display on, Cursor on.
	 			 * 0x01 Clear display screen.
			 */

			Timer_Init(&LCD_TIMER_Init_Config);
			LCD_DDR |= 0xF0;
	
			SET_BIT(LCD_CONFIG_DDR, RS);//output.
			SET_BIT(LCD_CONFIG_DDR, RW);//output.
			SET_BIT(LCD_CONFIG_DDR, E);//output.
			/*
			 * Init CMD.
			 */

			LCD_sendCommand(0x02,NullPointer);
			LCD_sendCommand(0x28,NullPointer);

			LCD_sendCommand(0x0C,NullPointer);
			LCD_sendCommand(0x01,NullPointer);
	  }
	  else if((Copy_PntrStr_LCD_Config->LCD_Method_Of_Execution)==LCD_Execution_Timer_Driven_Enable)
	  {
	         sgu8_LCD_Execution_Time=LCD_Execution_Timer_Driven_Enable;
			/*
			 * Init CMD.
			 */
			switch(su8_LCD_init_state)
			{
				case 0:
				{

					LCD_DDR |= 0xF0;
						
					SET_BIT(LCD_CONFIG_DDR, RS);//output.
					SET_BIT(LCD_CONFIG_DDR, RW);//output.
					SET_BIT(LCD_CONFIG_DDR, E);//output.
					if(su8_LCD_command_state) LCD_sendCommand(0x02,&su8_LCD_command_state);
					else
					{
						su8_LCD_command_state=1;
						su8_LCD_init_state=1;
					}
				}
				break;
				case 1:
				{
					
					if(su8_LCD_command_state) LCD_sendCommand(0x28,&su8_LCD_command_state);	
					else
					{
						su8_LCD_command_state=1;				
						su8_LCD_init_state=2;
					}
				}
				break;
				case 2:
				{
					if(su8_LCD_command_state) LCD_sendCommand(0x0C,&su8_LCD_command_state);
					else
					{
						su8_LCD_command_state=1;				
						su8_LCD_init_state=3;

					}
				}
				break;
				case 3:
				{
			
					if(su8_LCD_command_state) LCD_sendCommand(0x01,&su8_LCD_command_state);
					else
					{

	
						su8_LCD_command_state=1;
						su8_LCD_init_state=0;
						*pu8_flag=0;
					}
				}
				break;		
			}	

	  }
	  else
	  {
	    /*Do Nothing*/
	  }	
}

/*
 * send command to LCD.
 */
void LCD_sendCommand(const uint8_t cmd,uint8_t*Copy_Pntr_u8_LCD_sendCommand_Flag)
{
    static uint8_t su8_Command_State=0;
	if(sgu8_LCD_Execution_Time==LCD_Execution_Delay_Enable)
	{
		CLR_BIT(LCD_CONFIG_PORT, RS);
		CLR_BIT(LCD_CONFIG_PORT ,RW);
		/*
		 * After each CMD I must wait about (100 Ms).
		 */
		//Changed in 4-bit.


				SET_BIT(LCD_CONFIG_PORT, E);
				Timer_Start(TIMER0,2,NullPointer);
				//( LCD_PORT & 0x0F ) ﬂ‰  ‰«”ÌÂ« Œ«·’ „Õÿ Â«‘ ›Ì «·Õ”»«‰
				LCD_PORT = ( LCD_PORT & 0x0F )|( ( cmd & 0xF0 ));//Passed.
				CLR_BIT(LCD_CONFIG_PORT, E);
				Timer_Start(TIMER0,2,NullPointer);
				SET_BIT(LCD_CONFIG_PORT, E);
				Timer_Start(TIMER0,2,NullPointer);
				LCD_PORT = ( LCD_PORT & 0x0F ) | (( cmd << 4 ));
				CLR_BIT(LCD_CONFIG_PORT, E);
				Timer_Start(TIMER0,2,NullPointer);

		/*
		 * High to low pulse to on PIN "E".
		 */
	}
	else if(sgu8_LCD_Execution_Time==LCD_Execution_Timer_Driven_Enable)
	{

		/*
		 * After each CMD I must wait about (100 Ms).
		 */
		//Changed in 4-bit.
		switch(su8_Command_State)
		{
				case 0:
					CLR_BIT(LCD_CONFIG_PORT, RS);
					CLR_BIT(LCD_CONFIG_PORT, RW);
					SET_BIT(LCD_CONFIG_PORT, E);
					su8_Command_State=1;
					//( LCD_PORT & 0x0F ) ﬂ‰  ‰«”ÌÂ« Œ«·’ „Õÿ Â«‘ ›Ì «·Õ”»«‰
				break;
				case 1:				
					LCD_PORT = ( LCD_PORT & 0x0F )|( ( cmd & 0xF0 ));//Passed.
					CLR_BIT(LCD_CONFIG_PORT, E);
					su8_Command_State=2;
				break;
				case 2:
					SET_BIT(LCD_CONFIG_PORT, E);
					su8_Command_State=3;
				break;
				case 3:

					LCD_PORT = ( LCD_PORT & 0x0F ) | (( cmd << 4 ));
					CLR_BIT(LCD_CONFIG_PORT, E);
					su8_Command_State=0;
					*Copy_Pntr_u8_LCD_sendCommand_Flag=0;
				break;

			/*
			 * High to low pulse to on PIN "E".
			 */		
		}
	}
	else
	{
		/**Do Nothing**/
	}
}

/*
 * sendingData.
 */
void LCD_displayCharacter(const uint8_t data,uint8_t*Copy_Pntr_u8_LCD_displayCharacter_Flag)
{
	static uint8_t su8_LCD_init_state=0;
		
	if(sgu8_LCD_Execution_Time==LCD_Execution_Delay_Enable)
	{	
		SET_BIT(LCD_CONFIG_PORT, RS);
		CLR_BIT(LCD_CONFIG_PORT, RW);
		//Changed in 4-bit.

		SET_BIT(LCD_CONFIG_PORT, E);
		Timer_Start(TIMER0,2,NullPointer);
		LCD_PORT = ( LCD_PORT & 0x0F ) | ( ( data & 0xF0 ));//Passed.
		CLR_BIT(LCD_CONFIG_PORT, E);
		Timer_Start(TIMER0,2,NullPointer);
		SET_BIT(LCD_CONFIG_PORT, E);
		Timer_Start(TIMER0,2,NullPointer);
		LCD_PORT = ( LCD_PORT & 0x0F ) | (( data << 4 ));
		CLR_BIT(LCD_CONFIG_PORT, E);
		Timer_Start(TIMER0,2,NullPointer);
	}
	else if(sgu8_LCD_Execution_Time==LCD_Execution_Timer_Driven_Enable)
	{
		switch(su8_LCD_init_state)
		{

			case 0:
				SET_BIT(LCD_CONFIG_PORT, RS);
				CLR_BIT(LCD_CONFIG_PORT, RW);
				SET_BIT(LCD_CONFIG_PORT, E);
				su8_LCD_init_state=1;
			break;
			case 1:
				LCD_PORT = ( LCD_PORT & 0x0F ) | ( ( data & 0xF0 ));//Passed.
				CLR_BIT(LCD_CONFIG_PORT, E);
				su8_LCD_init_state=2;
			break;
			case 2:
				SET_BIT(LCD_CONFIG_PORT, E);
				su8_LCD_init_state=3;
			break;
			case 3:
				LCD_PORT = ( LCD_PORT & 0x0F ) | (( data << 4 ));
				CLR_BIT(LCD_CONFIG_PORT, E);
				*Copy_Pntr_u8_LCD_displayCharacter_Flag=0;
				su8_LCD_init_state=0;
			break;
		
		}	
	}
	else
	{
		/*Do Nothing*/
	}

}


void LCD_displayString(uint8_t* Copy_Pntr_u8_LCD_displayString,uint8_t*Copy_Pntr_u8_LCD_displayString_Flag)
{
	uint8_t u8_index_Polling_String = 0;
	static uint8_t u8_index_Time_event_string = 0;
	static uint8_t u8_LCD_displayString_FLag=1;
	
	if(sgu8_LCD_Execution_Time==LCD_Execution_Delay_Enable)
	{
		while( Copy_Pntr_u8_LCD_displayString[u8_index_Polling_String] != '\0' )
		{
			LCD_displayCharacter( Copy_Pntr_u8_LCD_displayString[u8_index_Polling_String],NullPointer);
			u8_index_Polling_String++;
		}
	}
	else if(sgu8_LCD_Execution_Time==LCD_Execution_Timer_Driven_Enable)
	{
		if(Copy_Pntr_u8_LCD_displayString[u8_index_Time_event_string]!='\0')
		{
		
			if(u8_LCD_displayString_FLag==1) LCD_displayCharacter(Copy_Pntr_u8_LCD_displayString[u8_index_Time_event_string],&u8_LCD_displayString_FLag);
			else
			{
				u8_LCD_displayString_FLag=1;
				u8_index_Time_event_string++;
			}
		}
		else if(Copy_Pntr_u8_LCD_displayString[u8_index_Time_event_string]=='\0')
		{
			u8_index_Time_event_string=0;
			*Copy_Pntr_u8_LCD_displayString_Flag=0;
		}

		else
		{
			/**Do Nothing**/
		}
			
	}
    else
    {
	    /**Do Nothing**/
    }
}


void LCD_goToRowColumn(uint8_t row, uint8_t col,uint8_t* Copy_Pntr_su8_LCD_goToRowColumn_Flag)
{
	uint8_t u8_address;
	static uint8_t su8_LCD_goToRowColumn_Flag=1;	
	
	switch(row)
	{
		case 1:
			u8_address = 0x80 + col;
			//OR LCD_sendCommand(0x82).
			break;
		case 2:
			u8_address = 0xC0 + col;
			//OR LCD_sendCommand(0xC2);
			break;
		default:
			break;
			//Nothing.
	}

	if(sgu8_LCD_Execution_Time==LCD_Execution_Delay_Enable)
	{

		LCD_sendCommand(u8_address | 0b10000000,NullPointer);
	}
	else if(sgu8_LCD_Execution_Time==LCD_Execution_Timer_Driven_Enable)
	{
		if(su8_LCD_goToRowColumn_Flag==1) LCD_sendCommand(u8_address | 0b100000000,& su8_LCD_goToRowColumn_Flag);
		else
		{
			su8_LCD_goToRowColumn_Flag=1;
			*Copy_Pntr_su8_LCD_goToRowColumn_Flag=0;
		}

	}
    else
    {
	    /**Do Nothing**/
    }
}

/*
 * Put string in ROW-COL.
 */
void LCD_stringRowCol(uint8_t* Copy_Pntr_u8_LCD_displayString, uint8_t row, uint8_t col,uint8_t* Copy_Pntr_u8_LCD_stringRowCol_Flag)
{
	static uint8_t u8_LCD_displayString_FLAG=1;
	static uint8_t su8_LCD_goToRowColumn_Flag=1;
	static uint8_t su8_LCD_stringRowCol_State=0;	
	if(sgu8_LCD_Execution_Time==LCD_Execution_Delay_Enable)
	{
		LCD_goToRowColumn(row, col,NullPointer);
		LCD_displayString(Copy_Pntr_u8_LCD_displayString,NullPointer);
	}
	else if(sgu8_LCD_Execution_Time==LCD_Execution_Timer_Driven_Enable)
	{
		switch (su8_LCD_stringRowCol_State)
		{
			case 0:
			if(su8_LCD_goToRowColumn_Flag) LCD_goToRowColumn(row, col,&su8_LCD_goToRowColumn_Flag) ;
			else
			{
				su8_LCD_goToRowColumn_Flag=1;
				su8_LCD_stringRowCol_State=1;
			}
			break;
		
			case 1:
			if(u8_LCD_displayString_FLAG) LCD_displayString(Copy_Pntr_u8_LCD_displayString,&u8_LCD_displayString_FLAG);
			else
			{
				u8_LCD_displayString_FLAG=1;
				su8_LCD_stringRowCol_State=0;
				*Copy_Pntr_u8_LCD_stringRowCol_Flag=0;
			}
			break;
		
			case 2:
			/*Do Nothing*/
			break;
		}


	}
	else
	{
		/*Do Nothing*/
	}
}


/*
 * Clear Screen.
 */

void LCD_clearScreen(uint8_t*Copy_Pntr_u8_LCD_clearScreen_Flag)
{
	static uint8_t su8_LCD_clearScreen_state=1;

	if(sgu8_LCD_Execution_Time==LCD_Execution_Delay_Enable)
	{	
		LCD_sendCommand(0x01,NullPointer);
	}
	else if(sgu8_LCD_Execution_Time==LCD_Execution_Timer_Driven_Enable)
	{
		if(su8_LCD_clearScreen_state) LCD_sendCommand(0x01,&su8_LCD_clearScreen_state);
		else
		{
			su8_LCD_clearScreen_state=1;
			*Copy_Pntr_u8_LCD_clearScreen_Flag=0;
		}	
	}
    else
    {
	    /**Do Nothing**/
    }	
}
/*
 * integerToString.
 */
void LCD_integerToString(uint32_t value,uint8_t row,uint8_t col,uint8_t* Copy_Pntr_su8_LCD_integerToString_FLAG)
{
	static uint8_t su8_LCD_integerToString_FLAG=1;
	uint32_t arru32_buffer[20];
	
	itoa(value, arru32_buffer , 10);	
	
	if(sgu8_LCD_Execution_Time==LCD_Execution_Delay_Enable)
	{
		LCD_stringRowCol(arru32_buffer, row,col,NullPointer);
	}
	else if(sgu8_LCD_Execution_Time==LCD_Execution_Timer_Driven_Enable)
	{

		if(su8_LCD_integerToString_FLAG==1) LCD_stringRowCol(arru32_buffer,row,col,&su8_LCD_integerToString_FLAG);
		else
		{
			su8_LCD_integerToString_FLAG=1;
			*Copy_Pntr_su8_LCD_integerToString_FLAG=0;
		}
	}
	else
	{	
	    /**Do Nothing**/		
	}

}

