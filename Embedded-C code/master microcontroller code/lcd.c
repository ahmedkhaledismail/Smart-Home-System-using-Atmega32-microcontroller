static void send_falling_edge(void); // local function

#include "LCD.h" 

void LCD_vInit(void)
{
	
	/* set LCD pins as output pins in case of 8 bit LCD*/
	DIO_vsetPINDir(LCD_PORT,LCD_FIRST_PIN,1);
	DIO_vsetPINDir(LCD_PORT,LCD_SECOND_PIN,1);
	DIO_vsetPINDir(LCD_PORT,LCD_THIRD_PIN,1);
	DIO_vsetPINDir(LCD_PORT,LCD_FOURTH_PIN,1);
	DIO_vsetPINDir(LCD_PORT,LCD_FIFTH_PIN,1);
	DIO_vsetPINDir(LCD_PORT,LCD_SIXTH_PIN,1);
	DIO_vsetPINDir(LCD_PORT,LCD_SEVENTH_PIN,1);
	DIO_vsetPINDir(LCD_PORT,LCD_EIGHTH_PIN,1);
	/*****************************/
	/* set LCD control pins as output pins in case of 8 bit LCD */
	DIO_vsetPINDir(LCD_CONTROL_PORT,LCD_EN_PIN,1);
	DIO_vsetPINDir(LCD_CONTROL_PORT,LCD_RW_PIN,1);
	DIO_vsetPINDir(LCD_CONTROL_PORT,LCD_RS_PIN,1);
	/*****************************/
	DIO_write(LCD_CONTROL_PORT,LCD_RW_PIN,0); 
	LCD_vSend_cmd(EIGHT_BIT_MODE);  
	LCD_vSend_cmd(CLR_SCREEN); 
	LCD_vSend_cmd(DISPLAY_ON_CURSOR_ON); // enable cursor and display on 
	_delay_ms(10); //small delay to ensure LCD finish the initialization
	
}


static void send_falling_edge(void)
{
	DIO_write(LCD_CONTROL_PORT,LCD_EN_PIN,1);
	_delay_ms(2); //period of high
	DIO_write(LCD_CONTROL_PORT,LCD_EN_PIN,0);
	_delay_ms(2); // period of low
}
void LCD_vSend_cmd(uint8 cmd)
{
	
	DIO_write_port(LCD_PORT,cmd); //send the command to LCD
	DIO_write(LCD_CONTROL_PORT,LCD_RS_PIN,0);  // data register loaded with instruction 
	send_falling_edge(); 
	
}
void LCD_vSend_char(uint8 data)
{
	DIO_write_port(LCD_PORT,data); 
	DIO_write(LCD_CONTROL_PORT,LCD_RS_PIN,1); // data register loaded with data 
	send_falling_edge(); 
}

void LCD_vSend_string(char *data)
{
	while((*data)!=0) 
	{
		LCD_vSend_char(*data);
		data++; 
	}
}

void LCD_clearscreen(void)
{
	LCD_vSend_cmd(CLR_SCREEN); 
}
void LCD_movecursor(uint8 row,uint8 coloumn)
{
	uint8 data ; 
	if(row>2||row<1||coloumn>16||coloumn<1)
	{
		data=0x80;
	}
	if(row==1)
	{
		data=0x80+coloumn-1 ; // 0x80 force cursor to start from first row
	}
	else if (row==2)
	{
		data=0xc0+coloumn-1; // 0x8c force cursor to start from second row
	}
	LCD_vSend_cmd(data); 
}
