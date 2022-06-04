#include <avr/io.h>
#include "std_macros.h"
#include "STD_Types.h"
void ADC_vinit(void)
{
	/*configure VREF to the internal voltage(2.56 V)*/
	SET_BIT(ADMUX,REFS0);
	SET_BIT(ADMUX,REFS1);

	CLR_BIT(ADMUX, MUX0);
	CLR_BIT(ADMUX, MUX1);
	CLR_BIT(ADMUX, MUX2);
	CLR_BIT(ADMUX, MUX3);
	CLR_BIT(ADMUX, MUX4);
	
	SET_BIT(ADCSRA,ADEN); // ENable ADC
	
	// ADC clock = XTAL / 64
	SET_BIT(ADCSRA,ADPS2);
	SET_BIT(ADCSRA,ADPS1);

}
uint16 ADC_u16Read(void)
{
	uint16 read_val; 
	SET_BIT(ADCSRA,ADSC); 
	while(IS_BIT_CLR(ADCSRA,ADIF));//stay in your position till ADIF become 1 when conversion complete
	SET_BIT(ADCSRA,ADIF); // clear ADIF
	read_val=(ADCL); 
	read_val|=(ADCH<<8);
	return read_val ;
}
