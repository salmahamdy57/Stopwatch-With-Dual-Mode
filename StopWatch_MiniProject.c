/****************************************************************************************************************************************
 Name        : StopWatch_MiniProject.c
 Author      : Salma Hamdy
 Description : Stop watch with Dual Mode (Increment and Decrement) Using ATmega32 and Seven-Segment Display.
 ****************************************************************************************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/******************************************** Variable Declaration and Initialization ***************************************************/

unsigned char second = 0;                       /* Seconds variable */
unsigned char second_ones = 0;                  /* Seconds digit to the right */
unsigned char second_tens = 0;                  /* Seconds digit to the left */

unsigned char minute = 0;                       /* Minutes variable */
unsigned char minute_ones = 0;                  /* Minutes digit to the right */
unsigned char minute_tens = 0;                  /* Minutes digit to the left */

unsigned char hour = 0;                         /* Hours variable */
unsigned char hour_ones = 0;                    /* Hours digit to the right */
unsigned char hour_tens = 0;                    /* Hours digit to the left */

unsigned char Flag_Mode = 0;                    /* Flag to indicate increment or decrement mode */
unsigned char Flag_toggle_event = 0;            /* Flag to indicate toggle is done once when button is pressed */

unsigned char Flag_hour_increment = 0;          /* Flag to indicate hour increment is done once when button is pressed */
unsigned char Flag_hour_decrement = 0;          /* Flag to indicate hour decrement is done once when button is pressed */
unsigned char Flag_minute_increment = 0;        /* Flag to indicate minute increment is done once when button is pressed */
unsigned char Flag_minute_decrement = 0;        /* Flag to indicate minute decrement is done once when button is pressed */
unsigned char Flag_second_increment = 0;        /* Flag to indicate second increment is done once when button is pressed */
unsigned char Flag_second_decrement = 0;        /* Flag to indicate second decrement is done once when button is pressed */

/************************************************ Timer 1 (Increment/Decrement) *******************************************************/

/* Timer Description:
 * For System Clock = 16MHz and Pre-scaler F_CPU/1024.
 * Timer frequency will be around 15.625kHz, Ttimer = 64us
 * For compare value equals to 15625 the timer will generate compare match interrupt every 1s.
 * Compare interrupt will be generated every 1s, so we need 1 compare interrupt to count 1 second.
 */

void Timer1_Init_CTC_Mode(void)
{
	TCNT1 = 0;		                             /* Set Timer1 initial count to zero */
	OCR1A = 15625;                               /* Set compare value to 15625 */
	TIMSK |= (1<<OCIE1A);                        /* Enable Timer1 Compare A Interrupt */

	/* Configure timer control register TCCR1A
	 * 1. Disconnect OC1A and OC1B  COM1A1 = 0  COM1A0 = 0  COM1B0 = 0  COM1B1 = 0
	 * 2. FOC1A = 1 FOC1B = 0
	 * 3. CTC Mode  WGM11 = 0 WGM10 = 0 (Mode Number 4)
	 */
	TCCR1A = (1<<FOC1A);

	/* Configure timer control register TCCR1B
	 * 1. CTC Mode WGM12 = 1 WGM13 = 0 (Mode Number 4)
	 * 2. Pre-scaler = F_CPU/1024 CS10 = 1 CS11 = 0 CS12 = 1
	 */
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
}

ISR(TIMER1_COMPA_vect)
{
	/*************************************************** Increment Mode ******************************************/

	if(Flag_Mode == 0)
	{
		PORTD &= ~(1<<PD5);  		             /* Set PD5 with value 0 to turn OFF YELLOW LED */
		PORTD |= (1<<PD4);  		             /* Set PD4 with value 1 to turn ON RED LED */

		second++;
		if (second > 59)
		{
			second = 0;
			minute++;
		}

		if (minute > 59)
		{
			minute = 0;
			hour++;
		}
		if (hour > 24)
		{
			hour = 0;
		}

		/* To display the time */
		second_ones = second % 10;
		second_tens = second / 10;

		minute_ones = minute % 10;
		minute_tens = minute / 10;

		hour_ones = hour % 10;
		hour_tens = hour / 10;
	}

	/*************************************************** Decrement Mode ******************************************/

	else if(Flag_Mode == 1)
	{
		PORTD &= ~(1<<PD4);  		              /* Set PD4 with value 0 to turn OFF RED LED */
		PORTD |= (1<<PD5);  		              /* Set PD5 with value 1 to turn ON YELLOW LED */

		if(second > 0)
		{
			second--;
		}
		if(hour == 0 && minute == 0 && second == 0)
		{
			TCCR1B = 0;                           /* No clock source (timer stops) */
			PORTD |= (1<<PD0);                    /* Set PD0 with value 1 to turn ON Alarm */
		}
		else if (second == 0 && minute == 0)
		{
			hour--;
			second = 59;
			minute = 59;
		}
		else if (second == 0)
		{
			second = 59;
			minute--;
		}

		/* To display the time */
		second_ones = second % 10;
		second_tens = second / 10;

		minute_ones = minute % 10;
		minute_tens = minute / 10;

		hour_ones = hour % 10;
		hour_tens = hour / 10;
	}
}

/******************************************************** RESET STOPWATCH ****************************************************************/
void Reset_INT0_Init(void)
{
	DDRD  &= ~(1<<PD2);                            /* Configure INT0/PD2 as input pin */
	PORTD |=(1<<PD2);                              /* Enable internal pull up resistor */
	MCUCR |= (1<<ISC01);                           /* Trigger INT0 with falling edge */
	GICR |= (1<<INT0);                             /* Enable external interrupt pin INT0 */
}

/* Stop watch time should be reset if INT0 is triggered on falling edge */
ISR(INT0_vect)
{
	PORTC &= 0xF0;                                 /* Set decoder value to zero */

	second = 0;                                    /* Display zero seconds */
	second_ones = 0;
	second_tens = 0;

	minute = 0;                                    /* Display zero minutes */
	minute_ones = 0;
	minute_tens = 0;

	hour = 0;                                      /* Display zero hours */
	hour_ones = 0;
	hour_tens = 0;
}

/******************************************************** PAUSE STOPWATCH ****************************************************************/
void Pause_INT1_Init(void)
{
	DDRD  &= ~(1<<PD3);                            /* Configure INT1/PD3 as input pin */
	MCUCR |= (1<<ISC11) | (1<<ISC10);              /* Trigger INT1 with rising edge */
	GICR |= (1<<INT1);                             /* Enable external interrupt pin INT1 */
}

/* Stop watch time should be paused if INT1 is triggered on rising edge */
ISR(INT1_vect)
{
	TCCR1B = 0;                                    /* No clock source (timer stops) */
}

/******************************************************* RESUME STOPWATCH ****************************************************************/

void Resume_INT2_Init(void)
{
	//DDRB  &= (~(1<<PB2));                        /* Configure INT2/PB2 as input pin */
	//PORTB |=(1<<PB2);                            /* Enable internal pull up resistor */
	MCUCR &= ~(1<<ISC2);                           /* Trigger INT2 with falling edge */
	GICR |= (1<<INT2);                             /* Enable external interrupt pin INT1 */
}

/* Stop watch time should be resumed if INT2 is triggered on falling edge */
ISR(INT2_vect)
{
	/* Re-Configure timer control register TCCR1B
	 * 1. CTC Mode WGM12 = 1 WGM13 = 0 (Mode Number 4)
	 * 2. Pre-scaler = F_CPU/1024 CS10 = 1 CS11 = 0 CS12 = 1
	 */
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
}

/********************************************* Time Adjustment Function (Decrement) *******************************************************/

void Adjust_Countdown_Start_Time(void)
{
	/********* To Increment Hours **********/
	if(!(PINB & (1<<PB1)))                          /* Check if hour increment button is pressed */
	{
		_delay_ms(30);                              /* To avoid switch de-bouncing */

		if(!(PINB & (1<<PB1)))
		{
			if(Flag_hour_increment == 0)            /* Check so increment is done once when button is pressed */
			{
				hour++;                             /* Increment Hours */

				/* To display the time */
				hour_ones = hour % 10;
				hour_tens = hour / 10;

				Flag_hour_increment = 1;
			}
		}
	}
	else
		Flag_hour_increment = 0;


	/********* To Decrement Hours **********/
	if(!(PINB & (1<<PB0)))                           /* Check if hour decrement button is pressed */
	{
		_delay_ms(30);                               /* To avoid switch de-bouncing */

		if(!(PINB & (1<<PB0)))
		{
			if(Flag_hour_decrement == 0)             /* Check so decrement is done once when button is pressed */
			{
				hour--;                              /* Decrement Hours */

				/* To display the time */
				hour_ones = hour % 10;
				hour_tens = hour / 10;

				Flag_hour_decrement = 1;
			}
		}
	}
	else
		Flag_hour_decrement = 0;


	/********* To Increment Minutes**********/
	if(!(PINB & (1<<PB4)))                           /* Check if minute increment button is pressed */
	{
		_delay_ms(30);                               /* To avoid switch de-bouncing */

		if(!(PINB & (1<<PB4)))
		{
			if(Flag_minute_increment == 0)           /* Check so increment is done once when button is pressed */
			{
				minute++;                            /* Increment Minutes */

				/* To display the time */
				minute_ones = minute % 10;
				minute_tens = minute / 10;

				Flag_minute_increment = 1;
			}
		}
	}
	else
		Flag_minute_increment = 0;


	/********* To Decrement Minutes **********/
	if(!(PINB & (1<<PB3)))                           /* Check if minute decrement button is pressed */
	{
		_delay_ms(30);                               /* To avoid switch de-bouncing */

		if(!(PINB & (1<<PB3)))
		{
			if(Flag_minute_decrement == 0)           /* Check so decrement is done once when button is pressed */
			{
				minute--;                            /* Decrement Minutes */

				/* To display the time */
				minute_ones = minute % 10;
				minute_tens = minute / 10;

				Flag_minute_decrement = 1;
			}
		}
	}
	else
		Flag_minute_decrement = 0;


	/********* To Increment Seconds**********/
	if(!(PINB & (1<<PB6)))                          /* Check if second increment button is pressed */
	{
		_delay_ms(30);                              /* To avoid switch de-bouncing */

		if(!(PINB & (1<<PB6)))
		{
			if(Flag_second_increment == 0)          /* Check so increment is done once when button is pressed */
			{
				second++;                           /* Increment Seconds */

				/* To display the time */
				second_ones = second % 10;
				second_tens = second / 10;

				Flag_second_increment = 1;
			}
		}
	}
	else
		Flag_second_increment = 0;


	/********* To Decrement Seconds**********/
	if(!(PINB & (1<<PB5)))                          /* Check if second decrement button is pressed */
	{
		_delay_ms(30);                              /* To avoid switch de-bouncing */

		if(!(PINB & (1<<PB5)))
		{
			if(Flag_second_decrement == 0)          /* Check so increment is done once when button is pressed */
			{
				second--;                           /* Decrement Seconds */

				/* To display the time */
				second_ones = second % 10;
				second_tens = second / 10;

				Flag_second_decrement = 1;
			}
		}
	}
	else
		Flag_second_decrement = 0;
}


/************************************************************* MAIN ********************************************************************/

int main(void)
{
	/********************************************** Port Configuration **************************************************/

	DDRC  |= 0x0F;                                  /* Configure PC0 to PC3 as output pins for decoder */
	PORTC &= 0xF0;                                  /* Initialize the 7-segment display to zero at the beginning */

	DDRA |= 0x3F;                                   /* Configure PA0 to PA5 as output pins to enable 7-segment display */

	DDRD |= (1<<PD4);                               /* Configure PD4 as output for RED LED (count up mode) */
	DDRD |= (1<<PD5);                               /* Configure PD5 as output for YELLOW LED (count down mode) */
	DDRD |= (1<<PD0);                               /* Configure PD0 as output for Alarm */
	PORTD &=~(1<<PD0);                              /* Initialize PD0 with value 0 (Alarm OFF)*/

	DDRB &= 0x00;                                   /* Configure PORTB as input port */
	PORTB |= 0xFF;                                  /* Enable internal pull up resistors */

	SREG  |= (1<<7);                                /* Enable global interrupts */


	/*********************************************** Function Call *******************************************************/

	Timer1_Init_CTC_Mode();                         /* Start the timer. */
	Reset_INT0_Init();                              /* Enable external INT0 */
	Pause_INT1_Init();                              /* Enable external INT1 */
	Resume_INT2_Init();                             /* Enable external INT2 */

	/********************************************************************************************************************/

    while(1)
    {
    	/************************************* Increment/Decrement Mode toggle ***************************************/

    	if(!(PINB & (1<<PB7)))                      /* Check if mode button is pressed then toggle mode */
    	{
    		_delay_ms(30);                          /* To avoid switch de-bouncing */

    		if(!(PINB & (1<<PB7)))
    		{
    			if(Flag_toggle_event == 0)          /* Check so toggle is done once when button is pressed */
    			{
    				Flag_Mode ^= (1<<0);            /* Toggle mode of operation */
    				Flag_toggle_event = 1;
    			}
    		}
    	}
		else
			Flag_toggle_event = 0;

    	/************************************* Time Adjustment (Decrement Mode) **************************************/

    	Adjust_Countdown_Start_Time();

    	/************************************* Display Time on 7-Segment Display *************************************/

    	_delay_ms(2);

    	/*  Display ones digit of seconds */
    	PORTA &= ~(1<<PA4) & ~(1<<PA3) & ~(1<<PA2) & ~(1<<PA1) & ~(1<<PA0);     /* Disable the rest of the 7-segments */
    	PORTA |= (1<<PA5) ;                                                     /* Enable second_ones digit 7-segment */
    	PORTC = (PORTC & 0xF0) | (second_ones & 0x0F);                          /* Display number on the 7-segment */
    	_delay_ms(2);

    	/* Display tens digit of seconds */
       	PORTA &= ~(1<<PA5) & ~(1<<PA3) & ~(1<<PA2) & ~(1<<PA1) & ~(1<<PA0);     /* Disable the rest of the 7-segments */
    	PORTA |= (1<<PA4);                                                      /* Enable second_tens digit 7-segment */
    	PORTC = (PORTC & 0xF0) | (second_tens & 0x0F);                          /* Display number on the 7-segment */
    	_delay_ms(2);


    	/* Display ones digit of minutes */
       	PORTA &= ~(1<<PA5) & ~(1<<PA4) & ~(1<<PA2) & ~(1<<PA1) & ~(1<<PA0);     /* Disable the rest of the 7-segments */
    	PORTA |= (1<<PA3) ;                                                     /* Enable minute_ones digit 7-segment */
    	PORTC = (PORTC & 0xF0) | (minute_ones & 0x0F);                          /* Display number on the 7-segment */
    	_delay_ms(2);

    	/* Display tens digit of minutes */
       	PORTA &= ~(1<<PA5) & ~(1<<PA4) & ~(1<<PA3) & ~(1<<PA1) & ~(1<<PA0);     /* Disable the rest of the 7-segments */
    	PORTA |= (1<<PA2);                                                      /* Enable minute_tens digit 7-segment */
    	PORTC = (PORTC & 0xF0) | (minute_tens & 0x0F);                          // Display number on the 7-segment */
    	_delay_ms(2);


    	/* Display ones digit of hours */
       	PORTA &= ~(1<<PA5) & ~(1<<PA4) & ~(1<<PA3) & ~(1<<PA2) & ~(1<<PA0);     /* Disable the rest of the 7-segments */
    	PORTA |= (1<<PA1);                                                      /* Enable hour_ones digit 7-segment */
    	PORTC = (PORTC & 0xF0) | (hour_ones & 0x0F);                            /* Display number on the 7-segment */
    	_delay_ms(2);

    	/* Display tens digit of hours */
     	PORTA &= ~(1<<PA5) & ~(1<<PA4) & ~(1<<PA3) & ~(1<<PA2) & ~(1<<PA1);     /* Disable the rest of the 7-segments */
    	PORTA |= (1<<PA0);                                                      /* Enable hour_tens digit 7-segment */
    	PORTC = (PORTC & 0xF0) | (hour_tens & 0x0F);                            /* Display number on the 7-segment */
    	_delay_ms(2);

    }
}
