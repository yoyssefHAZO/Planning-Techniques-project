#include"HAL/lcd.h"
#include"HAL/7seg.h"
#include"string.h"
#include<avr/interrupt.h>
#include<avr/io.h>
#include<stdio.h>//this library is used to convert int to str
//Time remaining for the client
uint8 time=30;
//Number of clients in the queue
uint8 clients=0;
//Initializing interrupt 1
void INT0_init(void)
{
	DDRD  &= (~(1<<PD2));               // Configure INT0/PD2 as input pin
	GICR  |= (1<<INT0);                 // It's the (module interrupt) Enable external interrupt pin INT0
	MCUCR |= (1<<ISC00) | (1<<ISC01);   // It's the register that configure the options of the interrupt, Trigger INT0 with the raising edge
	SREG  |= (1<<7);                    // Enable interrupts by setting I-bit
}
//The code that we want to implement with interrupt 1 'decreasing row'
ISR(INT0_vect){
	if(clients>0){
		time=30;
		clients--;
		write_number(clients);
		SET_BIT(PORTC,1);//Green led
		SET_BIT(PORTC,5);//Buzzer
		_delay_ms(1000);
		CLEAR_BIT(PORTC,5);
		CLEAR_BIT(PORTC,1);
	}
	else {
		clients=0;
		SET_BIT(PORTC,2);//Blue led
		_delay_ms(1000);
		CLEAR_BIT(PORTC,2);
	}

}

//Initializing the interrupt 2
void INT1_init(void)
{
	DDRD  &= (~(1<<PD3));               // Configure INT0/PD2 as input pin
	GICR  |= (1<<INT1);                 // It's the (module interrupt) Enable external interrupt pin INT0
	MCUCR |= (1<<ISC11) | (1<<ISC10);   // It's the register that configure the options of the interrupt, Trigger INT0 with the raising edge
	SREG  |= (1<<7);                    // Enable interrupts by setting I-bit
}
//The code that we want to implement with interrupt 2 'increasing row'
ISR (INT1_vect){
	if(clients<9) {
		clients++;
		write_number(clients);
	}
	else {
		clients=9;
		SET_BIT(PORTC,0);//Red led on
		SET_BIT(PORTC,5);//Buzzer
		_delay_ms(1000);
		CLEAR_BIT(PORTC,5);
		CLEAR_BIT(PORTC,0);
	}
}

int main(void)

{	//Configure (red led),(green led), and (buzzer) direction
	DDRC=0xFF;

	//Configure 7 seg pins to be output pins
	DDRA |= 0xFF;
	write_number(clients);
	_delay_ms(400);

	/* initialize LCD */
	LCD_init();
	LCD_displayString("TIME REMAININg:");
	//The array used to store the number after converting it to string
	char numbers[2];

	//Configure interrupts
	INT0_init();
	INT1_init();
	clients=0;
	write_number(clients);
    while(1)
    {
    //The for loop of time (30 seconds)
    	while(clients>0 && time>=0){
    		if(time==0){
    			time=30;
    			clients--;
    			SET_BIT(PORTC,1);
    			SET_BIT(PORTC,5);
    			_delay_ms(1000);
    			CLEAR_BIT(PORTC,5);
    			CLEAR_BIT(PORTC,1);
    		}

    		LCD_goToRowColumn(1,0);
    		sprintf(numbers,"%d",time);
    		LCD_displayString(numbers);
    		_delay_ms(1000);
    		time--;
    	}

    }
}
