#include <avr/io.h>
#include "SPI.h"
#include "STD_MESSAGES.h"
#include "std_macros.h"

void SPI_vInitMaster (void)
{
	DDRB |=(1<<SPI_MOSI) |(1<<SPI_SS)|(1<<SPI_SCK);
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);//Enable the SPI,master mode ,Set the clock speed to F/16
}

void SPI_vInitSlave (void)
{
	DDRB |= (1<<SPI_MISO) ;
	SPCR |= (1<<SPE);
}

uint8 SPI_ui8TransmitRecive (uint8 data)
{
	SPDR = data;
	
	//in case of master send data and wait till the data are fully transmitted
	//in case of slave wait the data to be transmitted by the master
	while (((SPSR&(1<<SPIF))>>SPIF)==0);
	
	return SPDR;
}
