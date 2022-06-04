#include "menu.h"

extern volatile uint16 session_counter;//Stores the session time
extern uint8 timeout_flag;//stores if the session is still valid or outdated

uint8 ui8ComparePass(const uint8* pass1,const uint8* pass2,const uint8 size)
{
	uint8 pin_counter=0;
	uint8 ret_value = TRUE;
	while (pin_counter<size)
	{
		if (*(pass1+pin_counter)!=*(pass2+pin_counter))
		{
			ret_value= FALSE;
			break;
		}
		pin_counter++;
	}
	return ret_value;
}

void vMenuOption(const uint8 u8SelectedRoom,const uint8 u8LoginMode)
{
	uint8 u8StatusCode  = 0;
	uint8 u8TurnOnCode  = 0;
	uint8 u8TurnOffCode = 0;
	uint8 response      = DEFAULT_ACK; //the response of the slave that is sent back based on the command of the master
	uint8 key_pressed   = NOT_PRESSED;
	
	do
	{
		LCD_clearscreen();//clear LCD screen and move the cursor to the first column of the first row
		
		/* Set the commands to be sent to the slave based on  the sent room or device */
		switch(u8SelectedRoom)
		{
			case ROOM1_MENU:
			u8StatusCode = ROOM1_STATUS;
			u8TurnOnCode = ROOM1_TURN_ON;
			u8TurnOffCode = ROOM1_TURN_OFF;
			LCD_vSend_string("Room1 S:");
			break;
			case ROOM2_MENU:
			u8StatusCode = ROOM2_STATUS;
			u8TurnOnCode = ROOM2_TURN_ON;
			u8TurnOffCode = ROOM2_TURN_OFF;
			LCD_vSend_string("Room2 S:");
			break;
			case ROOM3_MENU:
			u8StatusCode = ROOM3_STATUS;
			u8TurnOnCode = ROOM3_TURN_ON;
			u8TurnOffCode = ROOM3_TURN_OFF;
			LCD_vSend_string("Room3 S:");
			break;
			case ROOM4_MENU:
			u8StatusCode = ROOM4_STATUS;
			u8TurnOnCode = ROOM4_TURN_ON;
			u8TurnOffCode = ROOM4_TURN_OFF;
			LCD_vSend_string("Room4 S:");
			break;
			case TV_MENU:
			u8StatusCode = TV_STATUS;
			u8TurnOnCode = TV_TURN_ON;
			u8TurnOffCode = TV_TURN_OFF;
			LCD_vSend_string("TV S:");
			break;
			case AIRCOND_CTRL_MENU:
			u8StatusCode = AIR_COND_STATUS;
			u8TurnOnCode = AIR_COND_TURN_ON;
			u8TurnOffCode = AIR_COND_TURN_OFF;
			LCD_vSend_string("Air Cond. S:");
			break;
		}
		
		SPI_ui8TransmitRecive(u8StatusCode);//demand the status from the slave
		_delay_ms(100);
		response = SPI_ui8TransmitRecive(DEMAND_RESPONSE);//the master micro controller asks for the status
		if(response == ON_STATUS)
		{
			LCD_vSend_string("ON");
		}
		else
		{
			LCD_vSend_string("OFF");
		}

		LCD_movecursor(2,1);
		LCD_vSend_string("1-On 2-Off 0-RET");
		
		/*******************************************************************************/
		
		key_pressed = u8GetKeyPressed(u8LoginMode);//wait for the user till key is pressed or the time is out
		_delay_ms(100); //to avoid the duplication of the pressed key
		
		/*there is no need to take any action in case of the user pressed 0(RET) key
		breaking the loop will be enough since it will be handled in the main*/
		
		if (key_pressed == '1')
		{
			SPI_ui8TransmitRecive(u8TurnOnCode);
		}
		else if (key_pressed == '2')
		{
			SPI_ui8TransmitRecive(u8TurnOffCode);
		}
		else if( (key_pressed != NOT_PRESSED) && (key_pressed != '0') )
		{
			LCD_clearscreen();
			LCD_vSend_string("Wrong input");
			_delay_ms(500);
		}
	} while ( ( (key_pressed < '0') || (key_pressed > '2') ) && (timeout_flag == FALSE));
}

uint8 u8GetKeyPressed(const uint8 u8LoginMode)
{
	uint8 key_pressed = NOT_PRESSED;
	while (key_pressed == NOT_PRESSED)//repeat till the user press any key
	{
		if ( (session_counter >= ADMIN_TIMEOUT && u8LoginMode == ADMIN) || ( session_counter >= GUEST_TIMEOUT && u8LoginMode == GUEST ))
		{
			timeout_flag = TRUE;
			break; // continue execute code after calling function
		}
		
		key_pressed = keypad_u8check_press();
	}
	return key_pressed;
}
