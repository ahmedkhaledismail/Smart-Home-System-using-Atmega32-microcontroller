
#include <avr/io.h>
#include "std_macros.h"
#include "DIO.h"
void DIO_vsetPINDir(uint8 portname,uint8 pinnumber,uint8 direction)
{
	switch(portname)
	{
		case 'A':
		if(direction==1)
		{
			SET_BIT(DDRA,pinnumber);
		}
		else
		{
			CLR_BIT(DDRA,pinnumber);
		}
		break;
		case 'B':
		if(direction==1)
		{
			SET_BIT(DDRB,pinnumber);
		}
		else
		{
			CLR_BIT(DDRB,pinnumber);
		}
		break;
		case 'C':
		if(direction==1)
		{
			SET_BIT(DDRC,pinnumber);
		}
		else
		{
			CLR_BIT(DDRC,pinnumber);
		}
		break;
		case 'D':
		if(direction==1)
		{
			SET_BIT(DDRD,pinnumber);
		}
		else
		{
			CLR_BIT(DDRD,pinnumber);
		}
		break;
		default: break;
	}
}


void DIO_write(uint8 portname,uint8 pinnumber,uint8 outputvalue)
{
	switch(portname)
	{
		case 'A' :
		
		if(outputvalue==1)
		{
			SET_BIT(PORTA,pinnumber);
		}
		else
		{
			CLR_BIT(PORTA,pinnumber);
		}
		break ;
		case 'B':
		
		if(outputvalue==1)
		{
			SET_BIT(PORTB,pinnumber);
		}
		else
		{
			CLR_BIT(PORTB,pinnumber);
		}
		break ;
		case 'C' :
		
		if(outputvalue==1)
		{
			SET_BIT(PORTC,pinnumber);
		}
		else
		{
			CLR_BIT(PORTC,pinnumber);
		}
		break ;
		case 'D':
		
		if(outputvalue==1)
		{
			SET_BIT(PORTD,pinnumber);
		}
		else
		{
			CLR_BIT(PORTD,pinnumber);
		}
		break ;
		default: break ;
	}
}


uint8 DIO_u8read(uint8 portname,uint8 pinnumber)
{
	uint8 return_value=0;
	switch(portname)
	{
		case 'A' :
		return_value=Read_bIT(PINA,pinnumber);
		break;
		
		case 'B' :
		return_value=Read_bIT(PINB,pinnumber);
		break;
		
		case 'C' :
		return_value=Read_bIT(PINC,pinnumber);
		break;
		
		case 'D' :
		return_value=Read_bIT(PIND,pinnumber);
		break;
		default: break;
	}
	return return_value ;
}
void DIO_toggle(uint8 portname,uint8 pinnumber)
{
	switch(portname)
	{
		case 'A':
		TOG_BIT(PORTA,pinnumber);
		break;
		case 'B':
		TOG_BIT(PORTB,pinnumber);
		break;
		case 'C':
		TOG_BIT(PORTC,pinnumber);
		break;
		case 'D':
		TOG_BIT(PORTD,pinnumber);
		break;
		default: break;
	}
}
void DIO_write_port(uint8 portname,uint8 portvalue)
{
	switch(portname)
	{
		case 'A' :
		PORTA=portvalue; 
		break ;
		case 'B':
		PORTB=portvalue; 
		break ;
		case 'C' :
		PORTC=portvalue;
		break ;
		case 'D':
		PORTD=portvalue;
		break ;
		default: break ;
	}
}

void DIO_vconnectpullup(uint8 portname ,uint8 pinnumber, uint8 connect_pullup)
{
	switch(portname)
	{
		/* Connect or disconnect the pull up resistance to the given pin in port A */
		case 'A':
		if(connect_pullup==1)
		{
			CLR_BIT(SFIOR,PUD);
			CLR_BIT(DDRA,pinnumber);
			SET_BIT(PORTA,pinnumber);
		}
		else
		{
			CLR_BIT(PORTA,pinnumber);
		}
		break;
		/* Connect or disconnect the pull up resistance to the given pin in port B */
		case 'B':
		if(connect_pullup==1)
		{
			CLR_BIT(SFIOR,PUD);
			CLR_BIT(DDRB,pinnumber);
			SET_BIT(PORTB,pinnumber);
		}
		else
		{
			CLR_BIT(PORTB,pinnumber);
		}
		break;
		/* Connect or disconnect the pull up resistance to the given pin in port C */
		case 'C':
		if(connect_pullup==1)
		{
			CLR_BIT(SFIOR,PUD);
			CLR_BIT(DDRC,pinnumber);
			SET_BIT(PORTC,pinnumber);
		}
		else
		{
			CLR_BIT(PORTC,pinnumber);
		}
		break;
		/* Connect or disconnect the pull up resistance to the given pin in port D */
		case 'D':
		if(connect_pullup==1)
		{
			CLR_BIT(SFIOR,PUD);
			CLR_BIT(DDRD,pinnumber);
			SET_BIT(PORTD,pinnumber);
		}
		else
		{
			CLR_BIT(PORTD,pinnumber);
		}
		break;
		
	}
}
