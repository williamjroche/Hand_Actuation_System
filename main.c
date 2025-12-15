#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
//custom header files
#include "servo.h"
#include "servo_position.h"

//servo1 -- index finger (and thumb1)
//servo2 -- middle finger
//servo3 -- ring finger
//servo4 -- pinky finger
//servo5 -- thumb2

int main(void){
	servo_setup();
	
	while(1){
		DDRC &= ~(1 << PORTC0);
		DDRC &= ~(1 << PORTC1);
		/*stepper motor test code
		DDRB |= (1 << PORTB4) | (1 << PORTB5);
		PORTC |= 0x01;
		for(int counter = 0; counter <= 6400 ; counter++){
			PORTC != PORTC;
			_delay_ms(1);
		}*/
		if((PINC & (1 << 0)) != 0){
			_delay_ms(500);
			ServoMove all_servos_open[] = {
				{1, 2500, 1500},  //servo 1: 180 to 90
				{2, 2500, 1200},  //servo 2: 180 to 90
				{3, 0, 1700},  //servo 3: 180 to 90
				{4, 0, 1700},  //servo 4: 180 to 90
				{5, 2500, 1500}   //servo 5: 180 to 90
			};
			move_servos_together(10, all_servos_open, 5);
		}
		if((PINC & (1 << 1)) != 0){
			_delay_ms(500);
			ServoMove all_servos_close[] = {
				{1, 1500, 2500},  //servo 1: 90 to 180
				{2, 1200, 2500},  //servo 2: 90 to 180
				{3, 1700, 0},  //servo 3: 90 to 180
				{4, 1700, 0},  //servo 4: 90 to 180
				{5, 1500, 2500}   //servo 5: 90 to 180
			};
			move_servos_together(10, all_servos_close, 5);
		}
		
		
	}
	
	return 0;
}


