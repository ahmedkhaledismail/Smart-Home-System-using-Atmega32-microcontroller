#include "main_config.h"
#include "LCD.h"
#include "EEPROM.h"
#include "timer_driver.h"
#include "keypad_driver.h"
#include "SPI.h"
#include "LED.h"
#include "menu.h"
#include <avr/io.h>

volatile uint16 session_counter = 0;
uint8 timeout_flag = FALSE;

int main(void)
{
	
	uint8 temperature = 0;//The average temperature of the room
	uint8 temp_ones = NOT_SELECTED;
	uint8 temp_tens = NOT_SELECTED;
	
	uint8 login_mode = NO_MODE;
	uint8 block_mode_flag = FALSE;
	uint8 key_pressed = NOT_PRESSED;//
	/*****************  INITIALIZE  ***********************/
	LED_vInit(ADMIN_LED_PORT,ADMIN_LED_PIN);
	LED_vInit(GUEST_LED_PORT,GUEST_LED_PIN);
	LED_vInit(BLOCK_LED_PORT,BLOCK_LED_PIN);
	LED_vTurnOff(ADMIN_LED_PORT, ADMIN_LED_PIN);
	LED_vTurnOff(GUEST_LED_PORT, GUEST_LED_PIN);
	LED_vTurnOff(BLOCK_LED_PORT, BLOCK_LED_PIN);
	LCD_vInit();
	keypad_vInit();
	SPI_vInitMaster();//initializes the communication protocol of SPI in master side
	/******************************************************/
	/* Printing Welcome screen */
	LCD_vSend_string("Welcome to smart");
	LCD_movecursor(2,1);
	LCD_vSend_string("home system");
	_delay_ms(2000);
	LCD_clearscreen();
	/***************************/
	/*Setting Admin and Guest passwords if not set */
	//check if admin and guest passwords are set or not set
	if ( (EEPROM_ui8ReadByteFromAddress(ADMIN_PASS_STATUS_ADDRESS)!=PASS_SET) || (EEPROM_ui8ReadByteFromAddress(GUEST_PASS_STATUS_ADDRESS)!=PASS_SET) )
	{
		LCD_vSend_string("Login for");
		LCD_movecursor(2,1);
		LCD_vSend_string("first time");
		_delay_ms(2000);
		LCD_clearscreen();
		LCD_vSend_string("Set Admin pass");
		LCD_movecursor(2,1);
		LCD_vSend_string("Admin pass:");

		/********************************* setting Admin password **********************************************/
		uint8 pass_counter=0;//the counter of the characters of the password
		uint8 pass[PASS_SIZE]={NOT_STORED,NOT_STORED,NOT_STORED,NOT_STORED};
		while (pass_counter<PASS_SIZE)
		{
			key_pressed = NOT_PRESSED;
			while (key_pressed == NOT_PRESSED)
			{
				key_pressed = keypad_u8check_press();
			}

			pass[pass_counter]=key_pressed;
			LCD_vSend_char(key_pressed);
			_delay_ms(CHARACTER_PREVIEW_TIME);
			LCD_movecursor(2,12+pass_counter);
			LCD_vSend_char(PASSWORD_SYMBOL); // to display (Password sign *)
			_delay_ms(200);
			pass_counter++;
		}
		EEPROM_vWriteBlockToAddress(EEPROM_ADMIN_ADDRESS,pass,PASS_SIZE);
		EEPROM_vWriteByteToAddress(ADMIN_PASS_STATUS_ADDRESS,PASS_SET);
		LCD_clearscreen();
		LCD_vSend_string("Pass Saved");
		_delay_ms(1000);
		LCD_clearscreen();

		/********************************* setting guest password **********************************************/
		pass_counter=0;
		LCD_vSend_string("Set Guest Pass");
		LCD_movecursor(2,1);
		LCD_vSend_string("Guest Pass:");
		while (pass_counter<PASS_SIZE)
		{
			key_pressed = NOT_PRESSED;
			while (key_pressed == NOT_PRESSED)
			{
				key_pressed = keypad_u8check_press();
			}

			pass[pass_counter]=key_pressed;
			LCD_vSend_char(key_pressed);
			_delay_ms(CHARACTER_PREVIEW_TIME);
			LCD_movecursor(2,12+pass_counter);
			LCD_vSend_char(PASSWORD_SYMBOL); // to display (Password sign *)
			_delay_ms(200);
			pass_counter++;
		}
		EEPROM_vWriteBlockToAddress(EEPROM_GUEST_ADDRESS,pass,PASS_SIZE);
		EEPROM_vWriteByteToAddress(GUEST_PASS_STATUS_ADDRESS,PASS_SET);
		LCD_clearscreen();
		LCD_vSend_string("Pass Saved");
		_delay_ms(1000);
		LCD_clearscreen();
		EEPROM_vWriteByteToAddress(LOGIN_BLOCKED_ADDRESS,FALSE);
	}//The end of if admin and guest password is set
	else// in case of previously set admin and guest passwords
	{
		block_mode_flag = EEPROM_ui8ReadByteFromAddress(LOGIN_BLOCKED_ADDRESS); //read the blocked location from EEPROM
	}
	while (1)//The start of the periodic code
	{
		key_pressed = NOT_PRESSED;
		uint8 pass_tries_count=0;
		
		if ( timeout_flag==TRUE )//if timeout flag was raised
		{
			timer0_stop();//stop the timer that increase the session counter
			session_counter = 0;
			timeout_flag=FALSE;
			login_mode=NO_MODE;//log the user out
			key_pressed = NOT_PRESSED;//clear the key_pressed to avoid unwanted selection in the menu switch
			LED_vTurnOff(GUEST_LED_PORT,GUEST_LED_PIN);
			LED_vTurnOff(ADMIN_LED_PORT,ADMIN_LED_PIN);
			LCD_clearscreen();
			LCD_vSend_string("Session Timeout");
			_delay_ms(2000);
		}
		while (login_mode==NO_MODE)//The user can only leave the loop only in case of he was logged in as guest or admin
		{
			if(block_mode_flag==TRUE)
			{
				LCD_clearscreen();
				LCD_vSend_string("Login blocked");
				LCD_movecursor(2,1);
				LCD_vSend_string("wait 20 seconds");
				LED_vTurnOn(BLOCK_LED_PORT,BLOCK_LED_PIN);
				_delay_ms(BLOCK_MODE_TIME);
				pass_tries_count = 0; 
				block_mode_flag = FALSE;
				LED_vTurnOff(BLOCK_LED_PORT,BLOCK_LED_PIN);
				EEPROM_vWriteByteToAddress(LOGIN_BLOCKED_ADDRESS,FALSE);
			}
			LCD_clearscreen();
			LCD_vSend_string("Select mode :");
			LCD_movecursor(2,1);
			LCD_vSend_string("0:Admin 1:Guest");
			while(key_pressed==NOT_PRESSED)//wait for the selection of the mode
			{
				key_pressed = keypad_u8check_press();
			}
			if ( key_pressed!=CHECK_ADMIN_MODE && key_pressed!=CHECK_GUEST_MODE )
			{
				LCD_clearscreen();
				LCD_vSend_string("Wrong input.");
				key_pressed = NOT_PRESSED;
				_delay_ms(2000);
				continue;//return to while (login_mode==NO_MODE)
			}

			uint8 pass_counter=0;//counts the entered key of the password from the keypad
			uint8 pass[PASS_SIZE]={NOT_STORED,NOT_STORED,NOT_STORED,NOT_STORED};
			uint8 stored_pass[PASS_SIZE]={NOT_STORED,NOT_STORED,NOT_STORED,NOT_STORED};
			
			switch(key_pressed)
			{
				/********************************* Admin login **********************************************/
				case CHECK_ADMIN_MODE:
				while(login_mode!=ADMIN)//this loop is to repeat the login for admin in case of wrong password
				{
					key_pressed = NOT_PRESSED;
					LCD_clearscreen();
					LCD_vSend_string("Admin mode");
					LCD_movecursor(2,1);
					LCD_vSend_string("Enter Pass:");
					_delay_ms(200);
					pass_counter=0;
					while(pass_counter<PASS_SIZE)
					{
						while (key_pressed == NOT_PRESSED)
						{
							key_pressed = keypad_u8check_press();
						}
						pass[pass_counter]=key_pressed;
						LCD_vSend_char(key_pressed);
						_delay_ms(CHARACTER_PREVIEW_TIME);
						LCD_movecursor(2,12+pass_counter);
						LCD_vSend_char(PASSWORD_SYMBOL);// to display (Password sign *)
						_delay_ms(200);
						pass_counter++;
						key_pressed = NOT_PRESSED;
					}
					EEPROM_vReadBlockFromAddress(EEPROM_ADMIN_ADDRESS,stored_pass,PASS_SIZE);

					/*compare passwords*/
					if ((ui8ComparePass(pass,stored_pass,PASS_SIZE)) == TRUE)//in case of right password
					{
						login_mode = ADMIN;
						pass_tries_count=0;
						LCD_clearscreen();
						LCD_vSend_string("Right pass");
						LCD_movecursor(2,1);
						LCD_vSend_string("Admin mode");
						_delay_ms(1000);
						LED_vTurnOn(ADMIN_LED_PORT,ADMIN_LED_PIN);
						timer0_initializeCTC();//start the timer that counts the session time
						LCD_clearscreen();
					}
					else//in case of wrong password
					{
						pass_tries_count++;
						login_mode = NO_MODE;
						LCD_clearscreen();
						LCD_vSend_string("Wrong Pass");
						LCD_movecursor(2,1);
						LCD_vSend_string("Tries left:");
						LCD_vSend_char(TRIES_ALLOWED-pass_tries_count+ASCII_ZERO);
						_delay_ms(2000);
						LCD_clearscreen();
						if (pass_tries_count>=TRIES_ALLOWED)//if the condition of the block mode is true
						{
							EEPROM_vWriteByteToAddress(LOGIN_BLOCKED_ADDRESS,TRUE);
							block_mode_flag = TRUE;
							break;//break the loop of admin login #while(login_mode!=ADMIN)
						}
					}
				}
				break;//bREAK SWITCH case
				/********************************* Guest login **********************************************/
				case  CHECK_GUEST_MODE:
				while(login_mode != GUEST)
				{
					key_pressed = NOT_PRESSED;
					LCD_clearscreen();
					LCD_vSend_string("Guest mode");
					LCD_movecursor(2,1);
					LCD_vSend_string("Enter pass:");
					_delay_ms(200);
					pass_counter=0;
					while(pass_counter<PASS_SIZE)
					{
						while (key_pressed == NOT_PRESSED)
						{
							key_pressed = keypad_u8check_press();
						}
						pass[pass_counter]=key_pressed;
						LCD_vSend_char(key_pressed);
						_delay_ms(CHARACTER_PREVIEW_TIME);
						LCD_movecursor(2,12+pass_counter);
						LCD_vSend_char(PASSWORD_SYMBOL); // to display (Password sign *)
						_delay_ms(200);
						pass_counter++;
						key_pressed = NOT_PRESSED;
					}
					EEPROM_vReadBlockFromAddress(EEPROM_GUEST_ADDRESS,stored_pass,PASS_SIZE);//read guest password from the EEPROM

					/*compare passwords*/
					if (ui8ComparePass(pass,stored_pass,PASS_SIZE)==TRUE)//in case of right password
					{
						login_mode = GUEST;
						pass_tries_count=0;
						LCD_clearscreen();
						LCD_vSend_string("Right pass");
						LCD_movecursor(2,1);
						LCD_vSend_string("Guest mode");
						_delay_ms(1000);
						LED_vTurnOn(GUEST_LED_PORT,GUEST_LED_PIN);
						timer0_initializeCTC();//start the counter of the session
						LCD_clearscreen();
					}
					else//in case of wrong password
					{
						pass_tries_count++;
						login_mode = NO_MODE;
						LCD_clearscreen();
						LCD_vSend_string("Wrong pass");
						LCD_movecursor(2,1);
						LCD_vSend_string("Tries left:");
						LCD_vSend_char(TRIES_ALLOWED-pass_tries_count+ASCII_ZERO);
						_delay_ms(2000);
						LCD_clearscreen();
						if (pass_tries_count>=TRIES_ALLOWED)
						{
							EEPROM_vWriteByteToAddress(LOGIN_BLOCKED_ADDRESS,TRUE);//write to the EEPROM TRUE to the the block mode address
							block_mode_flag = TRUE;//turn on block mode
							break;//breaks the loop of insert guest password #while(login_mode != GUEST)# line 228
						}//end of if that check if the number of tries exceeds the maximum tries allowed
					}//end of the case of wrong password
				}//end of loop of guest login
				break;//break for CHECK_GUEST_MODE switch case
			}//end of switch
			
		}
		
		/*************************************************************************************************/
		uint8 show_menu = MAIN_MENU;
		
		
		while(timeout_flag!=TRUE)
		{
			key_pressed = NOT_PRESSED;
			switch (show_menu)
			{
				case MAIN_MENU:
				do
				{
					/******************** print main Menu ******************/
					LCD_clearscreen();
					LCD_vSend_string("1:Room1 2:Room2");
					LCD_movecursor(2,1);
					if(login_mode==ADMIN)
					{
						LCD_vSend_string("3:Room3 4:More ");
					}
					else if(login_mode==GUEST)
					{
						LCD_vSend_string("3:Room3 4:Room4");
					}
					/*******************************************************/
					
					key_pressed = u8GetKeyPressed(login_mode);//wait for the user till key is pressed or the time is out
					_delay_ms(200);
					
					if (key_pressed == SELECT_ROOM1)
					{
						show_menu = ROOM1_MENU;
					}
					else if (key_pressed == SELECT_ROOM2)
					{
						show_menu = ROOM2_MENU;
					}
					else if (key_pressed == SELECT_ROOM3)
					{
						show_menu = ROOM3_MENU;
					}
					else if (key_pressed == SELECT_ROOM4 && login_mode == GUEST)
					{
						show_menu = ROOM4_MENU;
					}
					else if (key_pressed == ADMIN_MORE_OPTION && login_mode == ADMIN)
					{
						show_menu = MORE_MENU;
					}
					else if(key_pressed != NOT_PRESSED)
					{
						LCD_clearscreen();
						LCD_vSend_string("Wrong input");
						_delay_ms(1000);
					}
				} while ( ((key_pressed < '1') || (key_pressed > '4') ) && (timeout_flag == FALSE) );//break the loop in case of valid key or time is out
				
				break;//End of main menu case
				
				case MORE_MENU:
				do
				{
					/******************** print more Menu ******************/
					LCD_clearscreen();
					LCD_vSend_string("1:Room4    2:TV   ");
					LCD_movecursor(2,1);
					LCD_vSend_string("3:Air Cond.4:RET");
					/*******************************************************/
					key_pressed = u8GetKeyPressed(login_mode);//wait for the user till key is pressed or the time is out
					_delay_ms(200);
					
					if (key_pressed == SELECT_ROOM4_ADMIN)
					{
						show_menu = ROOM4_MENU;
					}
					else if (key_pressed == SELECT_TV)
					{
						show_menu = TV_MENU;
					}
					else if (key_pressed == SELECT_AIR_CONDITIONING)
					{ 
						show_menu = AIRCONDITIONING_MENU;
					}
					else if (key_pressed == ADMIN_RET_OPTION)
					{
						show_menu = MAIN_MENU;
					}
					else if(key_pressed != NOT_PRESSED)//show wrong input message if the user pressed wrong key
					{
						LCD_clearscreen();
						LCD_vSend_string("Wrong input");
						_delay_ms(1000);
					}
				} while (( (key_pressed < '1') || (key_pressed > '4') ) && (timeout_flag == FALSE));//break the loop in case of valid key or time is out
				
				break;//End of more menu case
				
				case AIRCONDITIONING_MENU:
				do
				{
					/******************** print more Menu ******************/
					LCD_clearscreen();
					LCD_vSend_string("1:Set temperature ");
					LCD_movecursor(2,1);
					LCD_vSend_string("2:Control  0:RET");
					/*******************************************************/
					key_pressed = u8GetKeyPressed(login_mode);//wait for the user till key is pressed or the time is out
					_delay_ms(200);//to avoid the duplication of the pressed key
					
					if (key_pressed == SELECT_SET_TEMPERATURE)
					{
						show_menu = TEMPERATURE_MENU;
					}
					else if (key_pressed == SELECT_AIR_COND_CTRL)
					{
						show_menu = AIRCOND_CTRL_MENU;
					}
					else if (key_pressed == SELECT_AIR_COND_RET)
					{
						show_menu = MORE_MENU;
					}
					else if(key_pressed != NOT_PRESSED)
					{
						LCD_clearscreen();
						LCD_vSend_string("Wrong input");
						_delay_ms(1000);
					}
				} while (( (key_pressed < '0') || (key_pressed > '2') ) && (timeout_flag == FALSE));
				break;//End of air conditioning menu case
				
				case ROOM1_MENU:
				vMenuOption(ROOM1_MENU,login_mode);
				show_menu = MAIN_MENU;
				break;//End of room1 menu case
				
				case ROOM2_MENU:
				vMenuOption(ROOM2_MENU,login_mode);
				show_menu = MAIN_MENU;
				break;//End of room2 menu case
				
				case ROOM3_MENU:
				vMenuOption(ROOM3_MENU,login_mode);
				show_menu = MAIN_MENU;
				break;//End of room3 menu case
				
				case ROOM4_MENU:
				vMenuOption(ROOM4_MENU,login_mode);
				if (login_mode == GUEST)
				{
					show_menu = MAIN_MENU;
				}
				else//in case of admin is logged in
				{
					show_menu = MORE_MENU;
				}
				break;//End of room4 menu case
				
				case TV_MENU:
				vMenuOption(TV_MENU,login_mode);
				show_menu = MORE_MENU;
				break;//End of TV menu case
				
				case AIRCOND_CTRL_MENU:
				vMenuOption(AIRCOND_CTRL_MENU,login_mode);
				show_menu = AIRCONDITIONING_MENU;
				break;//End of air conditioning control menu case
				
				case TEMPERATURE_MENU:
				temperature = 0;//clear the value of temperature
				while (temperature==0 && timeout_flag == FALSE)
				{
					key_pressed = NOT_PRESSED;
					LCD_clearscreen();
					LCD_vSend_string("Set temp.:__ ");
					LCD_vSend_char(DEGREES_SYMBOL); 
					LCD_vSend_char('C');
					LCD_movecursor(1,11);
					_delay_ms(200);
					/*******************************************************************************/
					key_pressed = u8GetKeyPressed(login_mode);
					_delay_ms(500);//to avoid the duplication of the pressed key

					if (timeout_flag == TRUE) //in case of the time is out before the user press a key
					{
						break;//break the loop that ask for temperature
					}
					if (key_pressed <'0' || key_pressed >'9')
					{
						LCD_clearscreen();
						LCD_vSend_string("Wrong input");
						_delay_ms(1000);
						continue;//return to while (temperature==0)
					}
					else//if the value is valid
					{
						LCD_vSend_char(key_pressed);
						temp_tens = key_pressed-ASCII_ZERO;
						key_pressed = NOT_PRESSED;
					}
					/*******************************************************************************/
					key_pressed = u8GetKeyPressed(login_mode);//wait for the user till key is pressed or the time is out
					_delay_ms(500);//to avoid the duplication of the pressed key
					
					if (timeout_flag == TRUE)//if the user session is timeout
					{
						break;//break the loop that ask for temperature
					}
					if ((key_pressed <'0' || key_pressed >'9'))
					{
						LCD_clearscreen();
						LCD_vSend_string("Wrong input");
						_delay_ms(1000);
						continue;
					}
					else//if the value is valid
					{
						LCD_vSend_char(key_pressed);
						temp_ones = key_pressed-ASCII_ZERO;
						key_pressed = NOT_PRESSED;
					}
					temperature = temp_tens*10 + temp_ones;
					SPI_ui8TransmitRecive(SET_TEMPERATURE);//Send the code of set temperature
					_delay_ms(200);//Halt the system to prevent write collision
					SPI_ui8TransmitRecive(temperature);//send the entered temperature
					LCD_clearscreen();
					LCD_vSend_string("Temperature Sent");
					_delay_ms(1000);
				}
				show_menu = AIRCONDITIONING_MENU;
				break;//break from switch
			}//End of the switch
		}//End of while that repeats the menu after each successful action till session timeout
	}// end of the main while(1)
}//end of main function

ISR(TIMER0_COMP_vect)
{
	session_counter++; //increase the indicator of session time for every tick
}
